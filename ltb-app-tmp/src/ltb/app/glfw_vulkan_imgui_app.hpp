#pragma once

#include "fwd.hpp"
#include "ltb/util/result.hpp"

#include <vulkan/vulkan.hpp>

namespace ltb::app {

class GlfwVulkanImguiApp {
public:
    explicit GlfwVulkanImguiApp();
    virtual ~GlfwVulkanImguiApp();

    auto init_window(ApplicationInitialization settings) -> util::Result<GlfwVulkanImguiApp*>;
    auto init_vulkan(std::vector<vk::ExtensionProperties> const& extensions = {}) -> util::Result<GlfwVulkanImguiApp*>;
    auto run() -> util::Result<GlfwVulkanImguiApp*>;

    static auto force_redraw() -> void;

    auto               window() -> GLFWwindow*;
    [[nodiscard]] auto app_data() const -> ApplicationData const&;
    auto               app_data() -> ApplicationData&;

    static auto get_extensions() -> util::Result<std::vector<vk::ExtensionProperties>>;
    static auto get_validation_layers() -> util::Result<std::vector<vk::LayerProperties>>;

private:
    virtual auto update() -> void                                                       = 0;
    virtual auto render() -> void                                                       = 0;
    virtual auto resized() -> void                                                      = 0;
    virtual auto dropped_files(std::vector<std::filesystem::path> const& paths) -> void = 0;

    /// \brief RAII object to handle the initialization and destruction of GLFW
    std::shared_ptr<int> glfw_;

    /// \brief Data about the application window and update loop.
    std::shared_ptr<ApplicationData> app_data_;

    /// \brief RAII object to handle the initialization and destruction of a GLFW
    /// window.
    std::shared_ptr<GLFWwindow> window_;

    // Vulkan
    auto create_instance(std::vector<vk::ExtensionProperties> const& extensions) -> util::Result<GlfwVulkanImguiApp*>;

    vk::UniqueInstance instance_;
};

} // namespace ltb::app
