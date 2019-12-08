#include "backward.hpp"
#define BACKWARD_HAS_DW 1

namespace backward {
backward::SignalHandling sh;
} // namespace backward

#include <iostream>

#include "app/application.hpp"

std::string VERSION = "0.2.0";
std::string APP_NAME = "Hellfrost";

int main(int argc, char *argv[]) {
    auto seed = time(NULL);
    if (argc > 1) {
        seed = std::atoi(argv[1]);
    }
    Application app(APP_NAME, VERSION, seed);
    app.log.start(APP_NAME);
    app.serve();
    app.log.stop(APP_NAME);
    return EXIT_SUCCESS;
}
