#include "lss/game/equipment.hpp"
#include <iostream>

Equipment::Equipment() {}

bool Equipment::equip(std::shared_ptr<Slot> slot, std::shared_ptr<Item> item) {
  slot->equip(item);
  if (item->type.wearableType == WearableType::WEAPON_TWOHANDED) {
    auto secondary = std::find_if(
        slots.begin(), slots.end(), [slot](std::shared_ptr<Slot> s) {
          return s != slot &&
                 std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                           WearableType::WEAPON_LIGHT) != s->acceptTypes.end();
        });
    if (secondary != slots.end()) {
      (*secondary)->unequip();
      (*secondary)->equip(item);
    }
  }
  return true;
}
bool Equipment::unequip(std::shared_ptr<Slot> slot) {
  auto item = slot->item;
  slot->unequip();
  if (item->type.wearableType == WearableType::WEAPON_TWOHANDED) {
    auto secondary = std::find_if(
        slots.begin(), slots.end(), [slot](std::shared_ptr<Slot> s) {
          return s != slot &&
                 (std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                            WearableType::WEAPON) != s->acceptTypes.end() ||
                  std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                            WearableType::WEAPON_LIGHT) != s->acceptTypes.end());
        });
    if (secondary != slots.end()) {
      (*secondary)->unequip();
    }
  }
  return true;
}
