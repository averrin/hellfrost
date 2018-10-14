#include <iostream>

#include "app/application.hpp"

std::string VERSION = "0.0.0";

int main(int argc, char *argv[]) {
    Application app(VERSION);
    return app.serve();
}
