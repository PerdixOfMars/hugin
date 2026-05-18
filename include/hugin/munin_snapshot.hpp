#pragma once

#include <munin/component.hpp>

namespace hugin {

inline auto capture_snapshot(munin::component const &component)
{
    return component.to_json();
}

}  // namespace hugin
