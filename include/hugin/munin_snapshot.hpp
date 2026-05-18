#pragma once

#include <munin/component.hpp>

namespace hugin {

nlohmann::json capture_snapshot(munin::component const &component);

}  // namespace hugin
