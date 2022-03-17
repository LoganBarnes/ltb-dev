// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "glfw_vulkan_imgui_app.hpp"

#include "application_data.hpp"

#include <GLFW/glfw3.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/view/transform.hpp>
#include <spdlog/spdlog.h>

#define CHECK_AND_ASSIGN(var, func)                                                                                    \
    {                                                                                                                  \
        auto result_value = (func);                                                                                    \
        if (result_value.result != vk::Result::eSuccess) {                                                             \
            return tl::make_unexpected(LTB_MAKE_ERROR(#func "failed: " + vk::to_string(result_value.result)));         \
        }                                                                                                              \
        var = std::move(result_value.value);                                                                           \
    }

namespace ltb::app {

GlfwVulkanImguiApp::GlfwVulkanImguiApp()  = default;
GlfwVulkanImguiApp::~GlfwVulkanImguiApp() = default;

auto GlfwVulkanImguiApp::init_window(ApplicationInitialization settings) -> util::Result<GlfwVulkanImguiApp*> {

    // Set the error callback before any GLFW calls so we see when things go wrong.
    glfwSetErrorCallback([](int error, char const* description) { spdlog::error("ERROR: {}", description); });

    glfw_ = std::shared_ptr<int>(new int(glfwInit()), [](auto* p) {
        spdlog::debug("Terminating GLFW");
        glfwTerminate(); // safe even if glfwInit() fails
        delete p;
    });

    if (*glfw_ == 0) {
        return tl::make_unexpected(LTB_MAKE_ERROR("glfwInit() failed"));
    }
    spdlog::debug("GLFW Initialized");

    {
        auto primary_monitor_scale = glm::vec2{};

        if (GLFWmonitor* monitor = glfwGetPrimaryMonitor()) {
            glfwGetMonitorContentScale(monitor, &primary_monitor_scale[0], &primary_monitor_scale[1]);
        }
        spdlog::debug("Monitor scale: {}x{}", primary_monitor_scale[0], primary_monitor_scale[1]);

        app_data_ = std::make_unique<ApplicationData>(std::move(settings), primary_monitor_scale);
    }

    spdlog::debug("AA Samples: {}", app_data().aa_samples_per_pixel());

    // Apply requested window settings.
    glfwWindowHint(GLFW_SAMPLES, app_data().aa_samples_per_pixel());
    glfwWindowHint(GLFW_RESIZABLE, static_cast<int>(app_data().initialization().window_type == WindowType::Resizable));
    glfwWindowHint(GLFW_VISIBLE, static_cast<int>(app_data().initialization().window_type != WindowType::Hidden));

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    app_data().framebuffer_size = app_data().initialization().initial_window_size;

    // Create a fullscreen window if width or height is zero.
    if (app_data().framebuffer_size[0] == 0 || app_data().framebuffer_size[1] == 0) {
        GLFWvidmode const* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        app_data().framebuffer_size = {mode->width, mode->height};
    }

    app_data().framebuffer_size
        = glm::ivec2(glm::round(glm::vec2(app_data().framebuffer_size) * app_data().primary_monitor_scale()));

    auto const& title = app_data().initialization().title;

    window_ = std::shared_ptr<GLFWwindow>(glfwCreateWindow(app_data().framebuffer_size.x,
                                                           app_data().framebuffer_size.y,
                                                           title.c_str(),
                                                           nullptr,
                                                           nullptr),
                                          [](auto* p) {
                                              if (p) {
                                                  spdlog::debug("Destroying GLFW window");
                                                  glfwDestroyWindow(p);
                                              }
                                          });
    if (window_ == nullptr) {
        return tl::make_unexpected(LTB_MAKE_ERROR("GLFW window creation failed"));
    }
    spdlog::debug("GLFW window created ({}x{})", app_data().framebuffer_size[0], app_data().framebuffer_size[1]);

    return this;
}

auto GlfwVulkanImguiApp::init_vulkan(std::vector<vk::ExtensionProperties> const& extensions)
    -> util::Result<GlfwVulkanImguiApp*> {
    // clear all previous vulkan stuff first?
    return create_instance(extensions);
}

auto GlfwVulkanImguiApp::run() -> util::Result<GlfwVulkanImguiApp*> {
    // Trigger the initial screen size updates.
    resized();

    auto current_time = std::chrono::steady_clock::now();

    // The accumulator maintains the relationship between cpu time and
    // simulation time. It is manipulated in two ways:
    //
    // - On each frame the accumulator increases by the time it took
    //   to run the previous frame (cpu time).
    // - After each simulation update the accumulator is decremented
    //   by the constant simulation time step (sim time).
    //
    // This loop will continuously update the simulation until the
    // simulation's time has caught up to the expected cpu "real time".
    // This is done by comparing the accumulator's value to the expected
    // simulation time step. If the accumulator is less than the time step
    // interval, the loop will be allowed to continue on to rendering.
    util::Duration accumulator = util::duration_seconds(0.0);

    do {
        auto new_time   = std::chrono::steady_clock::now();
        auto frame_time = new_time - current_time;
        current_time    = new_time;

        frame_time = std::min(app_data().minimum_update_time_step, frame_time);

        // Ignore updates when paused.
        if (!app_data().pause_updates) {
            if (app_data().update_mode == ApplicationUpdateMode::RunNoFasterThanRealtime) {
                accumulator += frame_time;

                // Continually update the simulation until the sim
                // time is within one time step of the cpu time.
                while (accumulator >= util::duration_seconds(0.0)) {
                    app_data().cumulative_time += app_data().update_time_step;
                    update();
                    accumulator -= app_data().update_time_step;

                    // `interpolant_between_updates` is the normalized [0,1) interpolation value
                    // between the last update and the current update when this render call is made
                    // (optionally used to interpolate between previous and current states).
                    app_data().interpolant_between_updates
                        = util::to_seconds<double>(accumulator) / util::to_seconds<double>(app_data().update_time_step);
                }
            } else {
                app_data().interpolant_between_updates = 1.0;
                app_data().cumulative_time += app_data().update_time_step;
                update();
            }
        }

        render();

        // glfwSwapBuffers(window());

        if (app_data().update_mode == ApplicationUpdateMode::UpdateOnUserInputOnly || app_data().pause_updates) {
            // blocking input check when requested or paused
            glfwWaitEvents();
        } else {
            // non-blocking input check
            glfwPollEvents();
        }

    } while (!glfwWindowShouldClose(window()));

    return this;
}

auto GlfwVulkanImguiApp::get_extensions() -> util::Result<std::vector<vk::ExtensionProperties>> {
    auto extensions = std::vector<vk::ExtensionProperties>{};
    CHECK_AND_ASSIGN(extensions, vk::enumerateInstanceExtensionProperties(nullptr));
    return extensions;
}

auto GlfwVulkanImguiApp::get_validation_layers() -> util::Result<std::vector<vk::LayerProperties>> {
    auto layers = std::vector<vk::LayerProperties>{};
    CHECK_AND_ASSIGN(layers, vk::enumerateInstanceLayerProperties());
    return layers;
}

auto GlfwVulkanImguiApp::force_redraw() -> void {
    glfwPostEmptyEvent();
}

auto GlfwVulkanImguiApp::window() -> GLFWwindow* {
    return window_.get();
}

auto GlfwVulkanImguiApp::app_data() const -> ApplicationData const& {
    return *app_data_;
}

auto GlfwVulkanImguiApp::app_data() -> ApplicationData& {
    return *app_data_;
}

auto GlfwVulkanImguiApp::create_instance(std::vector<vk::ExtensionProperties> const& extensions)
    -> util::Result<GlfwVulkanImguiApp*> {
    auto app_info = vk::ApplicationInfo{}
                        .setPApplicationName("Hello Triangle")
                        .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
                        .setPEngineName("No Engine")
                        .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
                        .setApiVersion(VK_API_VERSION_1_0);

    auto extension_names = extensions //
        | ranges::views::transform([](vk::ExtensionProperties const& props) -> char const* {
                               return props.extensionName;
                           }) //
        | ranges::to<std::vector<char const*>>();

    // Add required GLFW extensions to the list.
    {
        uint32_t     glfw_extensions_count = 0;
        char const** glfw_extensions;

        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

        for (auto i = 0ul; i < glfw_extensions_count; ++i) {
            char const* glfw_extension = glfw_extensions[i];
            if (std::find(extension_names.begin(), extension_names.end(), glfw_extension) == extension_names.end()) {
                extension_names.emplace_back(glfw_extension);
            }
        }
    }

    auto create_info = vk::InstanceCreateInfo{}
                           .setPApplicationInfo(&app_info)
                           .setEnabledExtensionCount(extension_names.size())
                           .setPpEnabledExtensionNames(extension_names.data())
                           .setEnabledLayerCount(0);

    CHECK_AND_ASSIGN(instance_, vk::createInstanceUnique(create_info));

    return this;
}

} // namespace ltb::app