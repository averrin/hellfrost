#ifndef __GENERATOR_H_
#define __GENERATOR_H_
#include <thread>

#include <lss/game/cell.hpp>
#include <lss/game/location.hpp>
#include <lss/game/region.hpp>
#include <lss/gameData.hpp>
#include <entt/entt.hpp>

class Generator {
  LibLog::Logger &log = LibLog::Logger::getInstance();
public:
  Generator() {}
  std::shared_ptr<Location> getLocation();
  std::shared_ptr<Location> location;

  void makeCavePassage(std::shared_ptr<Region>);
  void placeCaves(std::shared_ptr<Region>);
  void makeExterior(std::shared_ptr<Region>);
  void placeDoors(std::shared_ptr<Region>);
  // std::shared_ptr<Location>
//   getRandomLocation(std::shared_ptr<Player>, int depth = -1,
//                     std::shared_ptr<Cell> enter = nullptr);
};

#endif // __GENERATOR_H_
