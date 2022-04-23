// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/app/fwd.hpp"
#include "ltb/util/duration.hpp"

// external
#include <glm/glm.hpp>

// standard
#include <string>

struct GLFWwindow;

namespace ltb::app {

enum class WindowType {
    Resizable,
    NonResizable,
    Hidden,
};

/// \brief Settings used to initialize an application
struct ApplicationInitialization {
    std::string title                          = "LTB Application";
    glm::ivec2  initial_window_size            = {0, 0};
    WindowType  window_type                    = WindowType::Resizable;
    int         preferred_antialiasing_samples = 4;
};

auto samples_with_monitor_scale(int requested_samples, glm::vec2 const& monitor_scale) -> int;

enum class ApplicationUpdateMode {
    UpdateOnUserInputOnly,
    RunNoFasterThanRealtime,
    RunAsFastAsPossible,
};

/// \brief Application information passed to children applications.
struct ApplicationData {
    explicit ApplicationData(ApplicationInitialization initialization, glm::vec2 primary_monitor_scale);

    [[nodiscard]] auto initialization() const -> ApplicationInitialization const&;
    [[nodiscard]] auto primary_monitor_scale() const -> glm::vec2 const&;
    [[nodiscard]] auto aa_samples_per_pixel() const -> int const&;

    // Graphics
    glm::ivec2 framebuffer_size    = {0, 0};
    bool       enable_gl_debugging = false;

    // Update Loop
    ApplicationUpdateMode update_mode                 = ApplicationUpdateMode::UpdateOnUserInputOnly;
    bool                  pause_updates               = false;
    util::Duration        cumulative_time             = util::duration_seconds(0.0);
    util::Duration        update_time_step            = util::duration_seconds(1.0 / 60.0);
    util::Duration        minimum_update_time_step    = util::duration_seconds(0.1);
    double                interpolant_between_updates = 0.0; ///< 0.0 is previous update, 1.0 is the next update.

private:
    // Static window data
    ApplicationInitialization initialization_        = {};
    glm::vec2                 primary_monitor_scale_ = {1.f, 1.f};
    int                       aa_samples_per_pixel_  = 4;
};

} // namespace ltb
