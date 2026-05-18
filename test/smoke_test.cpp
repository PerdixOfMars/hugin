#include <cstdlib>

#include <munin/container.hpp>

int main()
{
    munin::container container;
    auto const snapshot = container.to_json();

    if (snapshot["type"] != "container")
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
