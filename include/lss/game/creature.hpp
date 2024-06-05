#pragma once
#include <map>
#include <memory>
#include "lss/components.hpp"
#include "lss/game/cell.hpp"

class Creature {
  LibLog::Logger &log = LibLog::Logger::getInstance();
public:
  std::string getId() { return name; }
  std::vector<Trait> traits;
  // std::shared_ptr<Location> currentLocation;
  Creature();
  ~Creature();
  entt::entity entity;
  std::shared_ptr<Cell> currentCell;
  std::vector<std::shared_ptr<Cell>> viewField;
  std::string name = "Unnamed";
};
