#ifndef __GAMEMANAGER_H_
#define __GAMEMANAGER_H_
#include "app/scriptWrappers.hpp"
#include <entt/entt.hpp>
#include <lss/action.hpp>
#include <lss/components.hpp>
#include <lss/game/itemSpec.hpp>
#include <lss/game/location.hpp>
#include <lss/game/terrain.hpp>
#include <lss/gameData.hpp>
#include <lss/generator/generator.hpp>
#include <lss/utils.hpp>
// #include "lss/game/enemy.hpp"
#include "lss/generator/room.hpp"
#include <sequentity.hpp>

namespace hf = hellfrost;

class EntityWrapper {
public:
  entt::entity entity;
  std::shared_ptr<entt::registry> registry;
  EntityWrapper(entt::entity e) : entity(e) {}
  std::string getId() {
    auto meta = registry->get<hf::meta>(entity);
    return meta.id;
  }

  int getX() {
    auto p = registry->get<hf::position>(entity);
    return p.x;
  }
  int getY() {
    auto p = registry->get<hf::position>(entity);
    return p.y;
  }
  int getZ() {
    auto p = registry->get<hf::position>(entity);
    return p.z;
  }
  void select() {
    if (!registry->has<hf::ineditor>(entity))
      return;
    auto ie = registry->get<hf::ineditor>(entity);
    ie.selected = true;
    registry->assign_or_replace<hf::ineditor>(entity, ie);
  }
  void unselect() {
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
    // if (!registry->has<hf::renderable>(entity))
    //   return;
    // auto ie = registry->get<hf::renderable>(entity);
    // ie.hidden = true;
    // registry->assign_or_replace<hf::renderable>(entity, ie);
  }
  void show() {
    // auto ie = registry->get<hf::renderable>(entity);
    // ie.hidden = false;
    // registry->assign_or_replace<hf::renderable>(entity, ie);
  }

  void remove() {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    registry->destroy(entity);
    emitter->publish<redraw_event>();
  }
};

class GameManager {
  ll::Logger &log = ll::Logger::getInstance();

public:
  GameManager(fs::path);
  std::unique_ptr<Generator> generator;
  int seed;
  std::shared_ptr<entt::registry> registry = std::make_shared<entt::registry>();
  std::shared_ptr<Location> location;
  fs::path path;
  std::map<std::string, std::shared_ptr<RoomTemplate>> templates;
  std::map<std::string, std::shared_ptr<Feature>> features;
  std::map<std::string, std::shared_ptr<LocationSpec>> locationSpecs;

  std::shared_ptr<Location> getLocation() { return location; }
  std::shared_ptr<GameData> getData() {
    auto data = entt::service_locator<GameData>::get().lock();
    return data;
  }

  lss::Action currentAction;
  std::map<entt::entity, std::vector<std::shared_ptr<lss::Action>>> timeline;
  void commit(lss::Action action);

  int cursor = 0;
  void exec(int points);

  // void EsToProto(EnemySpec es, std::shared_ptr<GameData> data, int n);
  void applyData();
  void reset();
  void gen(LocationSpec);
  void setSeed(int s) { seed = s; }
  int size() { return registry->size(); }
  std::shared_ptr<EntityWrapper> createInLua(std::shared_ptr<Location> location,
                                             const std::string id, const int x,
                                             const int y, const int z);

  void loadData() {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    cereal::BinaryInputArchive iarchive(file);
    GameData data;

    iarchive(data);
    entt::service_locator<GameData>::set(data);
  }
  void saveData() {
    std::ofstream file(path, std::ios::out | std::ios::binary);
    cereal::BinaryOutputArchive oarchive(file);
    auto data = entt::service_locator<GameData>::ref();
    oarchive(data);
  }

  std::shared_ptr<entt::observer> player;
  void serve();

  bool moveCreature(std::shared_ptr<Creature>, Direction);
};

#endif // __GAMEMANAGER_H_
