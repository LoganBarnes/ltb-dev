// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/app/application_data.hpp"

// standard
#include <cmath>

namespace ltb::app {

auto samples_with_monitor_scale(int requested_samples, glm::vec2 const& monitor_scale) -> int {
    auto sqrt_samples   = std::sqrt(static_cast<float>(requested_samples));
    auto approx_samples = (sqrt_samples / monitor_scale.x) * (sqrt_samples / monitor_scale.y);
    auto aa_samples     = static_cast<int>(std::round(approx_samples));
    return std::max(1, aa_samples);
}

ApplicationData::ApplicationData(ApplicationInitialization initialization, glm::vec2 primary_monitor_scale)
    : initialization_(std::move(initialization)),
      primary_monitor_scale_(primary_monitor_scale),
      aa_samples_per_pixel_(
          samples_with_monitor_scale(initialization.preferred_antialiasing_samples, primary_monitor_scale_)) {}

auto ApplicationData::initialization() const -> ApplicationInitialization const& {
    return initialization_;
}
auto ApplicationData::primary_monitor_scale() const -> glm::vec2 const& {
    return primary_monitor_scale_;
}
auto ApplicationData::aa_samples_per_pixel() const -> int const& {
    return aa_samples_per_pixel_;
}

} // namespace ltb::app
