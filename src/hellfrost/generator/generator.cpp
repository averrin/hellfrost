#include <hellfrost/generator/generator.hpp>

namespace hellfrost {
std::shared_ptr<Location> Generator::getLocation() {
  return std::make_shared<Location>();
}

void Generator::start() {
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  log.start("Location generation");
  for (auto i = 0; i < 100; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // auto cell = location->getRandomCell();

    emitter->publish<location_update_event>();
  }
  log.stop("Location generation");
}
} // namespace hellfrost
