#ifndef __SCRIPTWRAPPERS_H_
#define __SCRIPTWRAPPERS_H_
#include <lss/deps.hpp>

namespace hf = hellfrost;

class EntityWrapper {
  entt::entity entity;

public:
  EntityWrapper(entt::entity e) : entity(e) {}
  std::string getId() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    auto meta = registry->get<hf::meta>(entity);
    return meta.id;
  }

  int getX() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    auto p = registry->get<hf::position>(entity);
    return p.x;
  }
  int getY() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    auto p = registry->get<hf::position>(entity);
    return p.y;
  }
  int getZ() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    auto p = registry->get<hf::position>(entity);
    return p.z;
  }
  void select() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    if (!registry->has<hf::ineditor>(entity))
      return;
    auto ie = registry->get<hf::ineditor>(entity);
    ie.selected = true;
    registry->assign_or_replace<hf::ineditor>(entity, ie);
  }
  void unselect() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    if (!registry->has<hf::ineditor>(entity))
      return;
    auto ie = registry->get<hf::ineditor>(entity);
    ie.selected = false;
    registry->assign_or_replace<hf::ineditor>(entity, ie);
  }
  void center() {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    emitter->publish<center_event>(getX(), getY());
  }

  void move(const int x, const int y, const int z) {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    registry->assign_or_replace<hf::position>(entity, x, y, z);
  }

  void hide() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    if (!registry->has<hf::renderable>(entity))
      return;
    auto ie = registry->get<hf::renderable>(entity);
    ie.hidden = true;
    registry->assign_or_replace<hf::renderable>(entity, ie);
  }
  void show() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    auto ie = registry->get<hf::renderable>(entity);
    ie.hidden = false;
    registry->assign_or_replace<hf::renderable>(entity, ie);
  }

  void remove() {
    auto registry = entt::service_locator<entt::registry>::get().lock();
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    registry->destroy(entity);
    emitter->publish<redraw_event>();
  }
};
class RegistryWrapper {
  std::shared_ptr<entt::registry> registry;

public:
  RegistryWrapper(std::shared_ptr<entt::registry> r) : registry(r) {}
  auto size() { return registry->size(); }

  auto create(const std::string id, const int x, const int y, const int z) {
    auto mutex = entt::service_locator<std::mutex>::get().lock();
    mutex->lock();
    auto data = entt::service_locator<GameData>::get().lock();
    entt::entity e;
    data->prototypes->each([&](auto entity) {
      auto meta = data->prototypes->get<hf::meta>(entity);
      if (meta.id == id) {
        e = entity;
      }
    });

    auto ne = registry->create();
    registry->assign<hf::ingame>(ne);
    registry->assign<hf::position>(ne, x, y, z);
    auto meta = data->prototypes->get<hf::meta>(e);
    meta.id = fmt::format("{}_{}", meta.id, (int)registry->entity(ne));
    registry->assign_or_replace<hf::meta>(ne, meta);
    registry->stomp(
        ne, e, *data->prototypes,
        entt::exclude<entt::tag<"proto"_hs>, hf::meta, hf::position>);
    mutex->unlock();
    return std::make_shared<EntityWrapper>(ne);
  }
};

#endif // __SCRIPTWRAPPERS_H_
