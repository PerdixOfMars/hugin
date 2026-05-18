#include <cstdlib>

#include <hugin/munin_snapshot.hpp>
#include <hugin/snapshot_queries.hpp>
#include <munin/button.hpp>
#include <munin/container.hpp>

int main()
{
    munin::container container;
    container.add_component(munin::make_button(" OK "));

    auto const snapshot = hugin::capture_snapshot(container);

    if (snapshot["type"] != "container")
    {
        return EXIT_FAILURE;
    }

    if (hugin::count_components_of_type(snapshot, "button") != 1)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
