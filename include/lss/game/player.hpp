#ifndef __PLAYER_H_
#define __PLAYER_H_
#include <map>
#include <memory>

#include "lss/components.hpp"
#include "lss/game/cell.hpp"
// #include "lss/game/creature.hpp"
// #include "lss/game/report.hpp"

// typedef std::vector<std::shared_ptr<Item>> Items;

class Player /*: public Creature*/ {
  LibLog::Logger &log = LibLog::Logger::getInstance();
public:
  Player();
  ~Player();
  entt::entity entity;
  std::shared_ptr<Cell> currentCell;
  std::vector<std::shared_ptr<Cell>> viewField;
  // bool monsterSense = false;
  // Report report;

  // bool equip(std::shared_ptr<Slot>, std::shared_ptr<Item>);
  // bool equip(std::shared_ptr<Item>);
  // bool unequip(std::shared_ptr<Slot>);
  // void commit(std::string reason, int ap, bool s = false);
  // void commitWaited(std::string reason);
  // bool interact(std::shared_ptr<Object> actor) override;

  // void onDamage(std::shared_ptr<Creature>, std::shared_ptr<Damage>) override;
  // void onDie() override;
  // void increaseIntelligence(float val);
  // void increaseStrength(float val);
  // std::map<std::shared_ptr<Object>, std::set<std::shared_ptr<Cell>>>
  // getLightMap();

};

#endif // __PLAYER_H_
