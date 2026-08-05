#include <boost/asio/io_context.hpp>
#include <munin/console_application.hpp>
#include <terminalpp/terminal.hpp>
#include <two_page_compass/app.hpp>

namespace {

auto make_behaviour()
{
    terminalpp::behaviour behaviour;
    behaviour.supports_basic_mouse_tracking = true;
    behaviour.supports_window_title_bel = true;
    return behaviour;
}

}  // namespace

int main()
{
    boost::asio::io_context io_context;
    auto content = two_page_compass::make_content();

    munin::console_application app{make_behaviour(), io_context, content};

    app.terminal() << terminalpp::enable_mouse() << terminalpp::hide_cursor()
                   << terminalpp::use_alternate_screen_buffer()
                   << terminalpp::set_window_title("two_page_compass");

    content->set_focus();
    io_context.run();

    app.terminal() << terminalpp::use_normal_screen_buffer()
                   << terminalpp::show_cursor() << terminalpp::disable_mouse();
}
