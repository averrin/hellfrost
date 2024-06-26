#include <cmath>
#include <variant>
// #include <fmt/format.h>

#include "lss/game/creature.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/events.hpp"
#include "lss/game/fov.hpp"
// #include "lss/game/player.hpp"
#include "lss/generator/room.hpp"
#include "lss/utils.hpp"

#include "EventBus/EventBus.hpp"

float NIGHT_VISION_DISTANCE = 10;

Creature::Creature() {
  passThrough = false;
  seeThrough = false;

  zIndex = 2;
}

void Creature::commit(std::string reason, int ap, bool s) {}

// TODO: implement attribute setter (for hp_boost spell)
float Attribute::operator()(Creature *c) {
  float base = 0;
  switch (type) {
  case AttributeType::VISIBILITY_DISTANCE:
    base = c->visibility_distance;
    break;
  case AttributeType::SPEED:
    base = c->speed;
    break;
  case AttributeType::HP:
    base = c->hp;
    break;
  case AttributeType::HP_MAX:
    base = c->hp_max * c->STRENGTH(c);
    break;
  case AttributeType::DEFENSE:
    base = c->defense;
    break;
  case AttributeType::CRIT_CHANCE:
    base = c->crit_chance;
    break;
  case AttributeType::MP:
    base = c->mp;
    break;
  case AttributeType::MP_MAX:
    base = c->mp_max * c->INTELLIGENCE(c);
    break;
  case AttributeType::INTELLIGENCE:
    base = c->intelligence;
    break;
  case AttributeType::STRENGTH:
    base = c->strength;
    break;
  }

  auto effects = c->getEffects(type);
  for (auto e : effects) {
    if (std::dynamic_pointer_cast<OverTimeEffect>(e))
      continue;
    if (e->type != type)
      continue;
    auto mod = e->getModifier();
    if (auto m = std::get_if<int>(&mod)) {
      base += *m;
    } else if (auto m = std::get_if<float>(&mod)) {
      base += *m;
    }
  }
  if (type == AttributeType::VISIBILITY_DISTANCE && base == 0) {
    base = c->default_visibility;
  }
  return base;
}

bool Creature::interact(std::shared_ptr<Object> actor) { return false; }

LeaveCellEvent::LeaveCellEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}
EnterCellEvent::EnterCellEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}
DropEvent::DropEvent(eb::ObjectPtr s, std::shared_ptr<Item> i)
    : eb::Event(s), item(i) {}
ItemTakenEvent::ItemTakenEvent(eb::ObjectPtr s, std::shared_ptr<Item> i)
    : eb::Event(s), item(i) {}

std::optional<std::tuple<std::shared_ptr<Slot>, DamageSpec>>
Creature::getPrimaryDmg() {

  auto primarySlot = std::find_if(
      equipment->slots.begin(), equipment->slots.end(),
      [](std::shared_ptr<Slot> s) {
        return s->item != nullptr &&
               std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                         WearableType::WEAPON) != s->acceptTypes.end();
      });
  if (primarySlot != equipment->slots.end()) {
    auto primaryWeapon = (*primarySlot)->item;
    auto meleeDmg = std::find_if(
        primaryWeapon->effects.begin(), primaryWeapon->effects.end(),
        [](std::shared_ptr<Effect> e) {
          return e->special && std::dynamic_pointer_cast<MeleeDamage>(e);
        });
    if (meleeDmg != primaryWeapon->effects.end()) {
      auto dmg = std::dynamic_pointer_cast<MeleeDamage>(*meleeDmg);
      return std::make_tuple(*primarySlot, dmg->dmgSpec);
    }
  }

  return std::nullopt;
}

std::optional<std::tuple<std::shared_ptr<Slot>, DamageSpec>>
Creature::getSecondaryDmg(std::shared_ptr<Slot> primarySlot) {
  if (primarySlot == nullptr) {
    primarySlot = *std::find_if(
        equipment->slots.begin(), equipment->slots.end(),
        [](std::shared_ptr<Slot> s) {
          return std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                           WearableType::WEAPON) != s->acceptTypes.end();
        });
  }
  auto secondarySlot = std::find_if(
      equipment->slots.begin(), equipment->slots.end(),
      [primarySlot](std::shared_ptr<Slot> s) {
        return s->item != nullptr && s != primarySlot &&
               std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                         WearableType::WEAPON_LIGHT) != s->acceptTypes.end();
      });
  if (secondarySlot == equipment->slots.end()) {
    return std::nullopt;
  }
  auto secondaryWeapon = (*secondarySlot)->item;

  auto secondaryMeleeDmg = std::find_if(
      secondaryWeapon->effects.begin(), secondaryWeapon->effects.end(),
      [](std::shared_ptr<Effect> e) {
        return e->special && std::dynamic_pointer_cast<MeleeDamage>(e);
      });
  if (secondaryMeleeDmg != secondaryWeapon->effects.end()) {
    auto dmg = std::dynamic_pointer_cast<MeleeDamage>(*secondaryMeleeDmg);
    return std::make_tuple(*secondarySlot, dmg->dmgSpec);
  }

  return std::nullopt;
}

std::shared_ptr<Damage> Creature::updateDamage(std::shared_ptr<Damage> damage,
                                               DamageSpec spec) {
  auto inShadow = !currentCell->illuminated;
  float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  if (inShadow && hasTrait(Traits::DEADLY_SHADOWS)) {
    damage->traits.push_back(Traits::DEADLY_SHADOWS);
    damage->damage += spec.criticalHit();
    damage->isCritical = true;
    return damage;
  // } else if (currentCell->hasFeature(CellFeature::BLOOD) &&
  //            hasTrait(Traits::BLOOD_THIRST)) {
  //   damage->traits.push_back(Traits::BLOOD_THIRST);
  //   damage->damage += spec.criticalHit();
  //   damage->isCritical = true;
  //   return damage;
  } else if (r < CRIT(this)) {
    damage->damage += spec.criticalHit();
    damage->isCritical = true;
    return damage;
  }
  damage->damage += spec.hitRoll();
  return damage;
}

std::shared_ptr<Damage> Creature::getDamage(std::shared_ptr<Object>) {
  // TODO: get damageType from creature
  auto damage =
      std::make_shared<Damage>(DamageSpec(0, 0, 0, DamageType::WEAPON));
  // auto primaryDmg = getPrimaryDmg();
  // if (primaryDmg != std::nullopt) {
  //   auto [primarySlot, spec] = *primaryDmg;
  //   damage = updateDamage(damage, spec);
  // }
  // auto haveLeft =
  //     std::count_if(
  //         equipment->slots.begin(), equipment->slots.end(),
  //         [](std::shared_ptr<Slot> s) {
  //           return s->item != nullptr &&
  //                  s->item->type.wearableType !=
  //                      WearableType::WEAPON_TWOHANDED &&
  //                  std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
  //                            WearableType::WEAPON_LIGHT) != s->acceptTypes.end() &&
  //                  std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
  //                            WearableType::WEAPON) == s->acceptTypes.end();
  //         }) > 0;
  // auto secondaryDmg = getSecondaryDmg(nullptr);
  // if (secondaryDmg != std::nullopt && haveLeft) {
  //   auto [secondarySlot, spec] = *secondaryDmg;
  //   if (hasTrait(Traits::DUAL_WIELD)) {
  //     damage = updateDamage(damage, spec);
  //   } else {
  //     damage->damage += spec.modifier;
  //   }
  // }
  // if (damage->damage == 0) {
  //   damage = updateDamage(damage, dmgSpec);
  // }
  // if (hasTrait(Traits::MOB)) {
  //   auto nbrs = currentLocation->getNeighbors(currentCell);
  //   if (std::find_if(nbrs.begin(), nbrs.end(), [&](std::shared_ptr<Cell> c) {
  //         auto enemies =
  //             utils::castObjects<Enemy>(currentLocation->getObjects(c));
  //         return enemies.size() > 0 && enemies.front()->hasTrait(Traits::MOB);
  //       }) != nbrs.end()) {
  //     damage->traits.push_back(Traits::MOB);
  //   }
  // }
  // damage->damage *= STRENGTH(this);
  return damage;
}

bool Creature::drop(std::shared_ptr<Item> item) {
  inventory.erase(std::remove(inventory.begin(), inventory.end(), item),
                  inventory.end());
  DropEvent me(shared_from_this(), item);
  eb::EventBus::FireEvent(me);
  return true;
}

bool Creature::pick(std::shared_ptr<Item> item) {
  auto ptr = shared_from_this();

  ItemTakenEvent e(ptr, item);
  eb::EventBus::FireEvent(e);

  if (auto it = std::find_if(inventory.begin(), inventory.end(),
                             [item](std::shared_ptr<Item> i) {
                               return item->getTitle(true) == i->getTitle(true);
                             });
      it != inventory.end() && item != 0 && item->count != 0) {
    (*it)->count += item->count;
  } else {
    item->identified = isIdentified(item);
    inventory.push_back(item);
  }

  return true;
}

bool Creature::attack(Direction d) {
  // auto nc = currentLocation->getCell(currentCell, d);
  // if (!nc)
  //   return false;
  // auto opit = std::find_if(
  //     currentLocation->objects.begin(), currentLocation->objects.end(),
  //     [&](std::shared_ptr<Object> o) {
  //       return o->currentCell == nc && (std::dynamic_pointer_cast<Creature>(o) || std::dynamic_pointer_cast<Terrain>(o));
  //     });
  // if (opit == currentLocation->objects.end() &&
  //     currentLocation->player->currentCell != nc) {
  //   MessageEvent me(shared_from_this(), "There is no target.");
  //   eb::EventBus::FireEvent(me);
  //   return false;
  // }
  // if (auto terrain = std::dynamic_pointer_cast<Terrain>(*opit); terrain) {
  //   auto ptr = std::dynamic_pointer_cast<Creature>(shared_from_this());
  //   for (auto t : terrain->triggers) {
  //       auto trigger = std::dynamic_pointer_cast<AttackTrigger>(t);
  //       if (trigger) {
  //           trigger->activate(ptr);
  //       }
  //   }

  // }
  // auto opponent =
  //     opit == currentLocation->objects.end() ? currentLocation->player : *opit;
  // opponent->interact(shared_from_this());
  return true;
}

// TODO: apply durning effect with non-zero fire damage
void Creature::applyDamage(std::shared_ptr<Object> a,
                           std::shared_ptr<Damage> damage) {
  // fmt::print("ad start\n");
  auto def = R::Z(0, DEF(this));
  if (damage->spec.type == DamageType::ACID && !hasTrait(Traits::ACID_IMMUNE)) {
    def /= 2;
  }
  if (damage->damage - def < 0) {
    damage->damage = 0;
  } else {
    damage->damage -= def;
  }
  damage->deflected = def;

  if (hasImmunity(damage->spec.type)) {
    damage->damage = 0;
    damage->defTraits.push_back(IMMUNITIES.at(damage->spec.type));
  }
  if (hasResist(damage->spec.type)) {
    damage->damage /= 2;
    damage->defTraits.push_back(RESISTS.at(damage->spec.type));
  }
  if (hasVulnerable(damage->spec.type)) {
    damage->damage *= 1.5;
    damage->defTraits.push_back(VULNERABLES.at(damage->spec.type));
  }
  // fmt::print("ad mid\n");

  hp -= damage->damage;
  if (auto attacker = std::dynamic_pointer_cast<Creature>(a);
    attacker) {
    onDamage(attacker, damage);
  }
  if (hp <= 0) {
    onDie();
  }
  // fmt::print("ad end\n");
}

std::shared_ptr<Slot> Creature::getSlot(WearableType type) {
  return *std::find_if(equipment->slots.begin(), equipment->slots.end(),
                       [type](std::shared_ptr<Slot> s) {
                         return std::find(s->acceptTypes.begin(),
                                          s->acceptTypes.end(),
                                          type) != s->acceptTypes.end();
                       });
}

std::optional<std::shared_ptr<Slot>> Creature::getSlot(WearableType type,
                                                       bool busy) {
  auto slot = std::find_if(
      equipment->slots.begin(), equipment->slots.end(),
      [type, busy](std::shared_ptr<Slot> s) {
        return (busy ? s->item != nullptr : s->item == nullptr) &&
               std::find(s->acceptTypes.begin(), s->acceptTypes.end(), type) !=
                   s->acceptTypes.end();
      });
  if (slot != equipment->slots.end()) {
    return *slot;
  }
  return std::nullopt;
}

bool Creature::move(Direction d, bool autoAction) {
  // auto cc = currentCell;
  // auto nc = currentLocation->getCell(currentCell, d);
  // if (!nc)
  //   return false;

  // if (hasTrait(Traits::JUMPY) && R::R() < 0.01) {
  //   auto room = currentLocation->rooms[rand() % currentLocation->rooms.size()];
  //   nc = room->cells[rand() % room->cells.size()];

  //   MessageEvent me(nullptr, "Your feel jumpy.");
  //   eb::EventBus::FireEvent(me);
  // }

  // auto obstacle = std::find_if(
  //     currentLocation->objects.begin(), currentLocation->objects.end(),
  //     [&](std::shared_ptr<Object> o) {
  //       return o->currentCell == *nc && !o->passThrough;
  //     });
  // auto hasObstacles = obstacle != currentLocation->objects.end();
  // auto hasPlayer = currentLocation->player->currentCell == *nc;
  // // fmt::print("{} - {} - {}.{} -> {}.{}\n", d, hasObstacles, cc->x, cc->y,
  // // nc->x, nc->y);
  // if (!(*nc)->canPass(traits) || hasObstacles || hasPlayer) {
  //   if (autoAction && hasObstacles && !(*obstacle)->passThrough) {
  //     if (std::dynamic_pointer_cast<Enemy>(*obstacle)) {
  //       return attack(d);
  //     }
  //     if (auto t = std::dynamic_pointer_cast<Terrain>(*obstacle); t) {
  //       auto ptr = std::dynamic_pointer_cast<Creature>(shared_from_this());
  //       return t->interact(ptr);
  //     }
  //     return !(*obstacle)->interact(shared_from_this());
  //   }
  //   return false;
  // }

  // setCurrentCell(*nc);

  // LeaveCellEvent le(shared_from_this(), cc);
  // eb::EventBus::FireEvent(le);
  // EnterCellEvent ee(shared_from_this(), currentCell);
  // eb::EventBus::FireEvent(ee);
  return true;
}

// FIXME: fix doors!
std::vector<std::shared_ptr<Cell>> Creature::calcViewField(bool force) {
  // if (cachedCell == currentCell && !force) {
  //   return;
  // }
  // currentLocation->updateLight(currentLocation->player);
  // auto vd = VISIBILITY_DISTANCE(this);
  // if (hasTrait(Traits::NIGHT_VISION)) {
  //   vd = NIGHT_VISION_DISTANCE;
  // }
  // // fmt::print("gv: {}.{}\n", currentCell->x, currentCell->y);
  // return currentLocation->getVisible(currentCell, vd);
  return {};

}

// TODO: migrate to emit
bool Creature::hasLight() {
  auto glow = getGlow();
  if (glow) {
    return true;
  }
  return false;
}

void Creature::applyEoT(EoT eot, int modifier) {
  switch (eot) {
  case EoT::HEAL: {
    hp += modifier;
    auto max = HP_MAX(this);
    if (hp > max)
      hp = max;
  } break;
  case EoT::MANA_RESTORE: {
    mp += modifier;
    auto max = MP_MAX(this);
    if (mp > max)
      mp = max;
  } break;
  case EoT::POISON:
    hp -= modifier;
    break;
  }
}

// TODO: get not visible, but reachable
std::vector<std::shared_ptr<Cell>> Creature::getInRadius(float distance) {
  std::vector<std::shared_ptr<Cell>> cells(viewField.size());
  auto it = std::copy_if(
      viewField.begin(), viewField.end(), cells.begin(), [&](auto c) {
        auto d =
            sqrt(pow(c->x - currentCell->x, 2) + pow(c->y - currentCell->y, 2));
        return c != currentCell && d <= distance && c->type.passThrough;
      });
  cells.resize(std::distance(cells.begin(), it));
  return cells;
}

// TODO: apply effect on throw and maybe not full damage
// TODO: show animation if enemy throw
bool Creature::throwItem(std::shared_ptr<Item> item,
                         std::shared_ptr<Cell> cell) {
  // std::shared_ptr<Item> throwed;
  // if (item->count == 0) {
  //   throwed = item;
  //   inventory.erase(std::remove(inventory.begin(), inventory.end(), item),
  //                   inventory.end());
  // } else {
  //   item->count--;
  //   throwed = item->clone();
  //   throwed->count = 1;
  // }
  // throwed->setCurrentCell(currentCell);
  // currentLocation->addObject(throwed);
  // auto cells = currentLocation->getLine(currentCell, cell);
  // auto a = std::make_shared<MoveAnimation>(throwed, cells, cells.size());
  // a->animationCallback = [=]() {
  //   auto e =
  //       std::find_if(item->effects.begin(), item->effects.end(), [](auto ef) {
  //         return std::dynamic_pointer_cast<MeleeDamage>(ef);
  //       });
  //   if (e != item->effects.end()) {
  //     auto objects = currentLocation->getObjects(cell);
  //     if (currentLocation->player->currentCell == cell) {
  //       currentLocation->addObject(currentLocation->player);
  //     }
  //     for (auto o : objects) {
  //       if (auto creature = std::dynamic_pointer_cast<Creature>(o)) {
  //         creature->applyDamage(
  //             shared_from_this(),
  //             std::dynamic_pointer_cast<MeleeDamage>(*e)->dmgSpec.getDamage());
  //       }
  //     }
  //   }
  // };
  // AnimationEvent ae(a);
  // eb::EventBus::FireEvent(ae);
  return true;
}

AiState Creature::getAiState(std::shared_ptr<Object> target) {
  std::string label = "get ai state";
  // log.start(lu::red("ENEMY"), label, true);
  AiState s;
  s.exit = false;
  s.target = target;
  s.viewField = calcViewField();
  s.canSeeTarget = canSee(target->currentCell, s.viewField);
  s.targetCell = target->currentCell;
  s.canSeeTargetCell = canSee(s.targetCell, s.viewField);

  if (s.canSeeTarget) {
    lastTargetCell = s.targetCell;
  } else {
    s.targetCell = lastTargetCell;
  }
  if (s.targetCell == nullptr || s.targetCell == currentCell) {
    s.exit = true;
    // log.stop(label, 20.f);
    return s;
  }

  if (!s.canSeeTargetCell && path.size() > 2) {
    auto it = path.end() - 2;
    for (auto n = 0; n < path.size() - 2; n++) {
      if (it == path.begin()) {
        s.exit = true;
        return s;
      }
      auto cell = *it;
      if (canSee(cell, s.viewField)) {
        s.targetCell = cell;
        break;
      }
    }
  }

  s.targetInTargetCell = target->currentCell == s.targetCell;
  s.path = currentLocation->getLine(currentCell, s.targetCell);
  s.nearTargetCell = s.canSeeTarget && s.path.size() <= 2;
  s.inThrowRange = s.canSeeTarget && s.path.size() - 1 <= getThrowRange();
  s.canThrow = s.inThrowRange &&
               std::find_if(inventory.begin(), inventory.end(), [](auto i) {
                 return i->type.category == ItemCategories::THROWABLE;
               }) != inventory.end();

  s.nearTarget = s.nearTargetCell;
  if (s.nearTarget) {
    s.neighbors = currentLocation->getNeighbors(currentCell);
    s.nearTarget =
        std::find_if(s.neighbors.begin(), s.neighbors.end(), [&target](auto n) {
          return n == target->currentCell;
        }) != s.neighbors.end();
  }

  s.canReachTarget =
      s.nearTarget || std::find_if(s.path.begin(), s.path.end(), [&](auto c) {
                        return c != currentCell && c != s.targetCell &&
                               !c->canPass(getTraits());
                      }) == s.path.end();
  if (!s.canReachTarget) {
    // fmt::print("can reach: {} (will use pather)\n", s.canReachTarget);
    s.path = findPath(s.targetCell);
    s.canReachTarget = s.path.size() > 1;
    // fmt::print("can reach: {}\n", s.canReachTarget);
  }

  // log.stop(label, 20.f);
  return s;
}

std::vector<std::shared_ptr<Cell>>
Creature::findPath(std::shared_ptr<Cell> targetCell) {
  std::vector<std::shared_ptr<Cell>> resultPath = {};
  if (pather == nullptr) {
    pather = new micropather::MicroPather(currentLocation.get());
  }
  pather->Reset();
  float totalCost = 0;
  micropather::MPVector<void *> raw_path;
  int result =
      pather->Solve(currentCell.get(), targetCell.get(), &raw_path, &totalCost);
  if (result == micropather::MicroPather::SOLVED) {
    for (auto i = 0; i < raw_path.size(); i++) {
      auto c = (Cell *)(raw_path[i]);
      auto ptr = currentLocation->cells[c->y][c->x];
      if (ptr != nullptr) {
        resultPath.push_back(ptr);
      }
    }
  }

  return resultPath;
}

// TODO: fix shields
std::string Creature::getDmgDesc() {
  auto primaryDmg = getPrimaryDmg();

  auto haveLeft =
      std::count_if(
          equipment->slots.begin(), equipment->slots.end(),
          [](std::shared_ptr<Slot> s) {
            return s->item != nullptr &&
                   s->item->type.wearableType !=
                       WearableType::WEAPON_TWOHANDED &&
                   std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                             WearableType::WEAPON_LIGHT) != s->acceptTypes.end() &&
                   std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                             WearableType::WEAPON) == s->acceptTypes.end();
          }) > 0;

  if (primaryDmg != std::nullopt && haveLeft) {
    auto [primarySlot, spec] = *primaryDmg;
    auto secondaryDmg = getSecondaryDmg(primarySlot);
    if (secondaryDmg != std::nullopt) {
      auto [secondarySlot, spec2] = *secondaryDmg;
      return fmt::format("{:+d} {}d{}{}", spec.modifier, spec.dices, spec.edges,
                         hasTrait(Traits::DUAL_WIELD)
                             ? fmt::format(" ({:+d} {}d{})", spec2.modifier,
                                           spec2.dices, spec2.edges)
                             : fmt::format(" ({:+d})", spec2.modifier));
    }
  } else if (haveLeft) {
    auto secondaryDmg = getSecondaryDmg(nullptr);
    if (secondaryDmg != std::nullopt) {
      auto [secondarySlot, spec2] = *secondaryDmg;
      return hasTrait(Traits::DUAL_WIELD)
                 ? fmt::format("~ {:+d} {}d{}", spec2.modifier, spec2.dices,
                               spec2.edges)
                 : fmt::format("~ {:+d}", spec2.modifier);
    }
  } else if (primaryDmg != std::nullopt) {
    auto [primarySlot, spec] = *primaryDmg;
    return fmt::format("{:+d} {}d{}", spec.modifier, spec.dices, spec.edges);
  }
  return fmt::format("{:+d} {}d{}", dmgSpec.modifier, dmgSpec.dices,
                     dmgSpec.edges);
}
