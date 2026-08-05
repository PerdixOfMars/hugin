#pragma once

#include <memory>

namespace munin {
class component;
}

namespace two_page_compass {

[[nodiscard]] auto make_content() -> std::shared_ptr<munin::component>;

}  // namespace two_page_compass
