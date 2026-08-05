#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/container.hpp>
#include <munin/filled_box.hpp>
#include <munin/image.hpp>
#include <munin/view.hpp>
#include <two_page_compass/app.hpp>

namespace two_page_compass {
namespace {

enum class page
{
    one,
    two
};

struct app_state
{
    std::shared_ptr<munin::container> root;
    std::shared_ptr<munin::component> current_page;
};

void show_page(std::shared_ptr<app_state> const &state, page target);

auto make_navigation(
    std::shared_ptr<app_state> const &state,
    page target,
    terminalpp::string const &label,
    munin::compass_layout::heading button_heading)
{
    auto button = munin::make_button(label);
    button->on_click.connect([state, target] { show_page(state, target); });

    return munin::view(
        munin::make_compass_layout(),
        munin::make_fill(' '),
        munin::compass_layout::heading::centre,
        button,
        button_heading);
}

auto make_page(std::shared_ptr<app_state> const &state, page current)
{
    auto const title = current == page::one ? "Page One" : "Page Two";
    auto const button_label = current == page::one ? " Next " : " Back ";
    auto const target = current == page::one ? page::two : page::one;
    auto const button_heading = current == page::one
                                  ? munin::compass_layout::heading::east
                                  : munin::compass_layout::heading::west;

    return munin::view(
        munin::make_compass_layout(),
        munin::make_image(title),
        munin::compass_layout::heading::north,
        munin::make_fill(' '),
        munin::compass_layout::heading::centre,
        make_navigation(state, target, button_label, button_heading),
        munin::compass_layout::heading::south);
}

void show_page(std::shared_ptr<app_state> const &state, page target)
{
    if (state->current_page != nullptr)
    {
        state->root->remove_component(state->current_page);
    }

    state->current_page = make_page(state, target);
    state->root->add_component(
        state->current_page, munin::compass_layout::heading::centre);
}

}  // namespace

auto make_content() -> std::shared_ptr<munin::component>
{
    auto state = std::make_shared<app_state>();
    state->root = munin::view(munin::make_compass_layout());
    show_page(state, page::one);
    return state->root;
}

}  // namespace two_page_compass
