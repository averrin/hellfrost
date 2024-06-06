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
  entt::registry& registry;
  EntityWrapper(entt::entity e, entt::registry& registry) : entity(e), registry(registry) {}
  std::string getId() {
    auto meta = registry.get<hf::meta>(entity);
    return meta.id;
  }

  int getX() {
    auto p = registry.get<hf::position>(entity);
    return p.x;
  }
  int getY() {
    auto p = registry.get<hf::position>(entity);
    return p.y;
  }
  int getZ() {
    auto p = registry.get<hf::position>(entity);
    return p.z;
  }
  void select() {
    if (!registry.all_of<hf::ineditor>(entity))
      return;
    auto ie = registry.get<hf::ineditor>(entity);
    ie.selected = true;
    registry.emplace_or_replace<hf::ineditor>(entity, ie);
  }
  void unselect() {
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
    // if (!registry.all_of<hf::renderable>(entity))
    //   return;
    // auto ie = registry.get<hf::renderable>(entity);
    // ie.hidden = true;
    // registry.emplace_or_replace<hf::renderable>(entity, ie);
  }
  void show() {
    // auto ie = registry.get<hf::renderable>(entity);
    // ie.hidden = false;
    // registry.emplace_or_replace<hf::renderable>(entity, ie);
  }

  void remove() {
    auto &emitter = entt::locator<event_emitter>::value();
    registry.destroy(entity);
    emitter.publish(redraw_event{});
  }
};

class GameManager {
  ll::Logger &log = ll::Logger::getInstance();

public:
  GameManager(fs::path);
  std::unique_ptr<Generator> generator;
  int seed;
  entt::registry& registry = entt::locator<entt::registry>::emplace();
  std::shared_ptr<Location> location;
  fs::path path;
  std::map<std::string, std::shared_ptr<RoomTemplate>> templates;
  std::map<std::string, std::shared_ptr<Feature>> features;
  std::map<std::string, std::shared_ptr<LocationSpec>> locationSpecs;

  std::shared_ptr<Location> getLocation() { return location; }
  GameData& getData() {
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
  int size() { return registry.storage<entt::entity>().size(); }
  std::shared_ptr<EntityWrapper> createInLua(std::shared_ptr<Location> location,
                                             const std::string id, const int x,
                                             const int y, const int z);

  void loadData() {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    cereal::BinaryInputArchive iarchive(file);
    entt::registry p{};
    auto &data = entt::locator<GameData>::emplace(p);

    iarchive(data);
  }
  void saveData() {
    std::ofstream file(path, std::ios::out | std::ios::binary);
    cereal::BinaryOutputArchive oarchive(file);
    auto &data = entt::locator<GameData>::value();
    oarchive(data);
  }

  std::shared_ptr<entt::observer> player;
  void serve();

  bool moveCreature(std::shared_ptr<Creature>, Direction);
};

#endif // __GAMEMANAGER_H_
