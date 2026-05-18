#include "hugin/munin_snapshot.hpp"

namespace hugin {

nlohmann::json capture_snapshot(munin::component const &component)
{
    return component.to_json();
}

}
