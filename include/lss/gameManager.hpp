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
  EntityWrapper(entt::entity e)
      : entity(e) {}
  std::string getId() {
    auto &registry = entt::locator<entt::registry>::value();
    auto meta = registry.get<hf::meta>(entity);
    return meta.id;
  }

  int getX() {
    auto &registry = entt::locator<entt::registry>::value();
    auto p = registry.get<hf::position>(entity);
    return p.x;
  }
  int getY() {
    auto &registry = entt::locator<entt::registry>::value();
    auto p = registry.get<hf::position>(entity);
    return p.y;
  }
  int getZ() {
    auto &registry = entt::locator<entt::registry>::value();
    auto p = registry.get<hf::position>(entity);
    return p.z;
  }
  void select() {
    auto &registry = entt::locator<entt::registry>::value();
    if (!registry.all_of<hf::ineditor>(entity))
      return;
    auto ie = registry.get<hf::ineditor>(entity);
    ie.selected = true;
    registry.emplace_or_replace<hf::ineditor>(entity, ie);
  }
  void unselect() {
    auto &registry = entt::locator<entt::registry>::value();
    if (!registry.all_of<hf::ineditor>(entity))
      return;
    auto ie = registry.get<hf::ineditor>(entity);
    ie.selected = false;
    registry.emplace_or_replace<hf::ineditor>(entity, ie);
  }
  void center() {
    auto &emitter = entt::locator<event_emitter>::value();
    emitter.publish(center_event{getX(), getY()});
  }

  void move(const int x, const int y, const int z) {
    auto &registry = entt::locator<entt::registry>::value();
    registry.emplace_or_replace<hf::position>(entity, x, y, z);
  }

  void hide() {
    auto &registry = entt::locator<entt::registry>::value();
    // if (!registry.all_of<hf::renderable>(entity))
    //   return;
    // auto ie = registry.get<hf::renderable>(entity);
    // ie.hidden = true;
    // registry.emplace_or_replace<hf::renderable>(entity, ie);
  }
  void show() {
    auto &registry = entt::locator<entt::registry>::value();
    // auto ie = registry.get<hf::renderable>(entity);
    // ie.hidden = false;
    // registry.emplace_or_replace<hf::renderable>(entity, ie);
  }

  void remove() {
    auto &registry = entt::locator<entt::registry>::value();
    registry.destroy(entity);
    auto &emitter = entt::locator<event_emitter>::value();
    emitter.publish(redraw_event{});
  }
};

class GameManager {
  ll::Logger &log = ll::Logger::getInstance();

public:
  GameManager(fs::path);
  std::unique_ptr<Generator> generator;
  int seed;
  std::shared_ptr<Location> location;
  fs::path path;
  std::map<std::string, std::shared_ptr<RoomTemplate>> templates;
  std::map<std::string, std::shared_ptr<Feature>> features;
  std::map<std::string, std::shared_ptr<LocationSpec>> locationSpecs;

  std::shared_ptr<Location> getLocation() { return location; }
  GameData &getData() {
    auto &data = entt::locator<GameData>::value();
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
  int size() {
    return entt::locator<entt::registry>::value()
        .storage<entt::entity>()
        .size();
  }
  std::shared_ptr<EntityWrapper> createInLua(std::shared_ptr<Location> location,
                                             const std::string id, const int x,
                                             const int y, const int z);

  void loadData() {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    // cereal::BinaryInputArchive iarchive(file);
    cereal::JSONInputArchive iarchive(file);
    auto &data = entt::locator<GameData>::emplace();
    iarchive(data);
  }
  void saveData() {
    std::ofstream file(path, std::ios::out | std::ios::binary);
    // cereal::BinaryOutputArchive oarchive(file);
    cereal::JSONOutputArchive oarchive(file);
    auto &data = entt::locator<GameData>::value();
    oarchive(data);
  }

  std::shared_ptr<entt::observer> player;
  void serve();

  bool moveCreature(std::shared_ptr<Creature>, Direction);
  void destroyEntity(entt::entity e);
};

#endif // __GAMEMANAGER_H_
