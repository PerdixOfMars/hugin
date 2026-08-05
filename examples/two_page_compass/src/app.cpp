#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/filled_box.hpp>
#include <munin/image.hpp>
#include <munin/view.hpp>
#include <two_page_compass/app.hpp>

namespace two_page_compass {

auto make_content() -> std::shared_ptr<munin::component>
{
    return munin::view(
        munin::make_compass_layout(),
        munin::make_image("Page One"),
        munin::compass_layout::heading::north,
        munin::make_fill(' '),
        munin::compass_layout::heading::centre,
        munin::view(
            munin::make_compass_layout(),
            munin::make_fill(' '),
            munin::compass_layout::heading::centre,
            munin::make_button(" Next "),
            munin::compass_layout::heading::east),
        munin::compass_layout::heading::south);
}

}  // namespace two_page_compass
