#include <cstdlib>

#include <hugin/munin_snapshot.hpp>
#include <munin/container.hpp>

int main()
{
    munin::container container;
    auto const snapshot = hugin::capture_snapshot(container);

    if (snapshot["type"] != "container")
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
