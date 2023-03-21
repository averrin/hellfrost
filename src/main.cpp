#include <selfpath.hpp>
#include "backward.hpp"

namespace backward {
backward::SignalHandling sh;
} // namespace backward

#include "app/application.hpp"
#include "lss/deps.hpp"

std::string VERSION = "0.2.0";
std::string APP_NAME = "Hellfrost";

int main(int argc, char *argv[]) {
  auto seed = time(NULL);
  if (argc > 1) {
    seed = std::atoi(argv[1]);
  }
  auto path = get_selfpath();
  Application app(APP_NAME, path, VERSION, seed);
  app.log.start(APP_NAME);
  app.serve();
  app.log.stop(APP_NAME);
  return EXIT_SUCCESS;
}
