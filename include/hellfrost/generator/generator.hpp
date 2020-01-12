#ifndef __GENERATOR_H_
#define __GENERATOR_H_

#include <hellfrost/deps.hpp>
#include <hellfrost/game/location.hpp>

namespace hellfrost {
class Generator {
  std::shared_ptr<Location> location;

public:
    Generator() {
      log.setParent(&LibLog::Logger::getInstance());
      std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();
      entt::service_locator<R::Generator>::set(gen);
    }
  LibLog::Logger log = LibLog::Logger(fmt::color::dark_sea_green, "GEN");
  std::shared_ptr<Location> getLocation();
  void start();
};
} // namespace hellfrost
#endif // __GENERATOR_H_
