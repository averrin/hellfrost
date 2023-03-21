#ifndef __GENERATOR_H_
#define __GENERATOR_H_
#include <entt/entt.hpp>
#include <lss/game/cell.hpp>
#include <lss/game/location.hpp>
#include <lss/gameData.hpp>
#include <lss/generator/room.hpp>

typedef std::function<void(std::shared_ptr<Location>)> FeatureGenerator;
class Feature {
  FeatureGenerator generator;
  sol::function luaGenerator;
  bool isLua = false;

public:
  int stage = 5;
  Feature(FeatureGenerator g, int s = 5) : generator(g), stage(s) {}
  Feature(sol::function g, int s = 5) : luaGenerator(g), stage(s) {
    isLua = true;
  }
  void generate(std::shared_ptr<Location> location) {
    if (isLua) {
      return generator(location);
    }
    return generator(location);
  }
};

class Generator {
  LibLog::Logger &log = LibLog::Logger::getInstance();

public:
  Generator() {}
  std::shared_ptr<Location> getLocation(LocationSpec);

  std::map<std::string, std::shared_ptr<RoomTemplate>> templates;
  std::map<std::string, std::shared_ptr<Feature>> features;

  void makeCavePassage(std::shared_ptr<Location> location);
  void placeCaves(std::shared_ptr<Location> location);
  void makeExterior(std::shared_ptr<Location> location);
  static void placeDoors(std::shared_ptr<Location> location);
  static void placeWalls(std::shared_ptr<Location> location);
  static void placeTemplateInRoom(std::shared_ptr<Location> location,
                                  std::shared_ptr<RoomTemplate> rtp);
  static void fixOverlapped(std::shared_ptr<Location> location);

  static void makeRiver(std::shared_ptr<Location> location);
  static void placeTorches(std::shared_ptr<Location> location);
  static std::vector<std::shared_ptr<Cell>>
  scatter(std::shared_ptr<Location> location, std::string tid, int min, int max,
          bool stickWalls);
  // std::shared_ptr<Location>
  //   getRandomLocation(std::shared_ptr<Player>, int depth = -1,
  //                     std::shared_ptr<Cell> enter = nullptr);

static void placeEnemies(std::shared_ptr<Location> location, int threat);
static void placeLoot(std::shared_ptr<Location> location, int threat);
  void execTemplates(std::shared_ptr<Location> location, int level) {
    for (auto [k, v] : location->type.templateMap) {
      if (templates.find(k) != templates.end()) {
        auto tpl = templates[k];
        if (tpl->stage == level && Random::get<bool>(v)) {
          location->log.debug("Executing template: {}", k);
          Generator::placeTemplateInRoom(location, tpl);
          location->tags.add(k);
        }
      } else if (features.find(k) != features.end()) {
        auto tpl = features[k];
        if (tpl->stage == level && Random::get<bool>(v)) {
          tpl->generate(location);
          location->tags.add(k);
        }
      }
    }
  }
};

#endif // __GENERATOR_H_
