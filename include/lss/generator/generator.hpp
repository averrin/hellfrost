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

  std::vector<std::shared_ptr<Cell>> getPath(std::shared_ptr<Location> location,
                                             std::shared_ptr<Cell> start,
                                             std::shared_ptr<Cell> end);
  void makeCavePassage(std::shared_ptr<Location> location);
  static std::vector<std::shared_ptr<Room>>
  placeCaves(std::shared_ptr<Location> location, int min = 7, int max = 25,
             int w = 100, int h = 100, int x = 0, int y = 0);
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
  static void makePassageBetweenCells(std::shared_ptr<Location> location,
                                      std::shared_ptr<Cell> sc,
                                      std::shared_ptr<Cell> ec);
  static void makePassageBetweenRooms(std::shared_ptr<Location> location,
                                      std::shared_ptr<Room> room1,
                                      std::shared_ptr<Room> room2);
  static bool placeStairs(std::shared_ptr<Location> location);
  static void makePassages(std::shared_ptr<Location> location,
                           std::vector<std::shared_ptr<Room>> d_rooms);
  static std::vector<std::shared_ptr<Room>>
  placeRooms(std::shared_ptr<Location> location, int min = 7, int max = 25,
             int w = 100, int h = 100, int x = 0, int y = 0);
  static void cleanWalls(std::shared_ptr<Location> location);
  static void placeEnemies(std::shared_ptr<Location> location, int threat);
  static void placeLoot(std::shared_ptr<Location> location, int threat);
  void execTemplates(std::shared_ptr<Location>, std::string, int, int);
};
#endif // __GENERATOR_H_
