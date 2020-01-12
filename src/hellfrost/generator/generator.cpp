#include <hellfrost/generator/generator.hpp>

namespace hellfrost {
std::shared_ptr<Location> Generator::getLocation() {
  location = std::make_shared<Location>();
  location->name = "location";
  return location;
}

void Generator::start() {
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  log.start("Location generation");
  for (auto i = 0; i < 10; i++) {
    // location->resize(i, i, CellType::FLOOR);
    auto gen = entt::service_locator<R::Generator>::get().lock();
    auto r = Region::random(10, 30, 10, 30);
    location->place(r, gen->R(15, 70), gen->R(0, 70), 0);
    auto c = gen->R(0, 3);
    for (auto n = 0; n < c; n++) {
      auto i = Region::random(3, 10, 3, 10, CellType::GROUND);
      r->place(i, gen->R(0, 20), gen->R(0, 20), 0);
      i->name = fmt::format("{}.{} @ {}.{}", i->width, i->height, i->x, i->y);
    }
    r->name = fmt::format("{}.{} @ {}.{}", r->width, r->height, r->x, r->y);
    location->update();
    // auto cell = location->getRandomCell();

    emitter->publish<location_update_event>();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  log.stop("Location generation");
}
} // namespace hellfrost
