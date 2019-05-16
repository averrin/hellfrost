#include <iostream>

#include "app/application.hpp"

std::string VERSION = "0.0.0";
std::string APP_NAME = "Hellfrost";

int main(int argc, char *argv[]) {
    Application app(APP_NAME, VERSION);
    app.log.start(APP_NAME);
    app.serve();
    app.log.stop(APP_NAME);
    return EXIT_SUCCESS;
}
