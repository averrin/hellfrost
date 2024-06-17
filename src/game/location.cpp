#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>

// #include "EventBus/EventBus.hpp"
// #include "lss/game/enemy.hpp"
// #include "lss/game/item.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"
// #include "lss/game/player.hpp"
#include "lss/game/terrain.hpp"
// #include <lss/game/aiManager.hpp>
#include "lss/generator/room.hpp"
#include "lss/utils.hpp"
#include <lss/gameData.hpp>
#include <lss/generator/mapUtils.hpp>
// #include "lss/generator/spawnTable.hpp"

float getDistance(std::shared_ptr<Cell> c, std::shared_ptr<Cell> cc) {
  return sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
}

Location::~Location() { clearHandlers(); }

void Location::addRoom(std::shared_ptr<Room> room, int x, int y) {
  mapUtils::paste(room->cells, this, x, y);
  rooms.push_back(room);
  room->x = x;
  room->y = y;

  for (auto e : room->entities) {
    if (!registry.valid(e) || !registry.all_of<hf::position>(e))
        continue;
    auto &p = registry.get<hf::position>(e);
    registry.emplace_or_replace<hf::position>(e, p.x + x, p.y + y, p.z);
  }
  for (auto c : room->cells) {
    c->x = c->_x + room->x;
    c->y = c->_y + room->y;
  }
}

std::vector<entt::entity> Location::getEntities(std::shared_ptr<Cell> cell) {
  if (cellEntities.find(cell) != cellEntities.end()) {
    return cellEntities[cell];
  }
  std::vector<entt::entity> result;
  auto ents = registry.group(entt::get<hf::position, hf::meta>);
  for (auto e : ents) {
    auto p = registry.get<hf::position>(e);
    if (p.movable && p.x == cell->x && p.y == cell->y) {
      result.push_back(e);
    }
  }
  cellEntities[cell] = result;
  return result;
}

void Location::invalidateVisibilityCache(std::shared_ptr<Cell> cell) {
  std::lock_guard<std::mutex> lock(visibilityCacheMutex);
  std::vector<std::pair<std::shared_ptr<Cell>, float>> hits;
  for (auto [p, _] : visibilityCache) {
    if (p.first == cell ||
        std::find(visibilityCache[p].begin(), visibilityCache[p].end(), cell) !=
            visibilityCache[p].end()) {
      hits.push_back(p);
    }
  }
  for (auto lsk : hits) {
    auto lsi = visibilityCache.find(lsk);
    if (lsi != visibilityCache.end()) {
      visibilityCache.erase(lsi);
    }
  }
  // if (ignore_player) {
  //   std::map<std::pair<std::shared_ptr<Cell>, float>,
  //            std::vector<std::shared_ptr<Cell>>>
  //       _visibilityCache;

  //   for (auto [p, _] : visibilityCache) {
  //     if (p.first == player->currentCell) {
  //       _visibilityCache[p] = visibilityCache[p];
  //     }
  //   }
  //   visibilityCache = _visibilityCache;
  // } else {
  //   // visibilityCache.clear();
  // }
}

// void Location::onEvent(DoorOpenedEvent &e) {
//   invalidateVisibilityCache(player->currentCell);
//   invalidate("door open");
//   player->viewField = player->calcViewField(true);
//   updateLight(player);
//   updateView(player);
// }

void Location::invalidate(std::string reason) {
  log.info(lu::yellow("MAP"),
           fmt::format("location invalidate [{}]", lu::magenta(reason)));
  invalidate();
}

// void Location::onEvent(CommitEvent &e) {
//   // TODO: and all enemies on location (not only this maybe)
//   auto effects = player->activeEffects;
//   for (auto ef : effects) {
//     if (auto le = std::dynamic_pointer_cast<LastingEffect>(ef)) {
//       le->currentDuration -= e.actionPoints;
//       if (le->currentDuration <= 0) {
//         player->activeEffects.erase(std::remove(
//             player->activeEffects.begin(), player->activeEffects.end(), ef));
//       }

//       if (auto eot = std::dynamic_pointer_cast<OverTimeEffect>(le->effect)) {
//         auto m = eot->getModifier(e.actionPoints);
//         player->applyEoT(eot->type, m);
//       }
//     }
//     if (auto eot = std::dynamic_pointer_cast<OverTimeEffect>(ef)) {
//       auto m = eot->getModifier(e.actionPoints);
//       player->applyEoT(eot->type, m);
//     }
//   }

//   if (player->HP(player.get()) <= 0) {
//     HeroDiedEvent e2(player);
//     eb::EventBus::FireEvent(e2);
//   }

//   auto creatures = utils::castObjects<Creature>(objects);
//   for (auto c : creatures) {
//     if (c->bgThread.joinable()) {
//       c->bgThread.join();
//     }
//   }

//   auto _objects = objects;
//   for (auto o : _objects) {
//     if (o->apLeft > 0) {
//       o->apLeft -= e.actionPoints;
//       if (o->apLeft <= 0) {
//         removeObject(o);
//       }
//     }
//   }

//   for (auto r : cells) {
//     for (auto c : r) {
//       auto items = utils::castObjects<Item>(getObjects(c));
//       if (items.size() > 1) {
//         std::vector<std::shared_ptr<Item>> so;
//         for (auto o : items) {
//           if (std::find(so.begin(), so.end(), o) != so.end()) continue;
//           so.clear();
//           std::copy_if(items.begin(), items.end(), std::back_inserter(so),
//           [&o](std::shared_ptr<Item> no) {
//             return no != o && no->name == o->name && no->type == o->type;
//           });
//           for (auto no : so) {
//             o->count += no->count;
//             removeObject(no);
//           }
//         }
//       }
//     }
//   }

//   apAccomulator += e.actionPoints;
//   if (apAccomulator >= 100000 / (type.threat+1) &&
//   !player->hasTrait(Traits::MIND_SIGHT)) {
//     apAccomulator = 0;
//     auto room = rooms[rand() % rooms.size()];
//     auto c = room->cells[rand() % room->cells.size()];
//     if (player->canSee(c)) {
//       while(player->canSee(c)) {
//         room = rooms[rand() % rooms.size()];
//         c = room->cells[rand() % room->cells.size()];
//       }
//     }
//     auto table = SpawnTable::DUNGEON.at(type.threat);
//     auto item = table.begin();
//     std::advance(item, rand() % table.size());
//     auto [etype, _] = *item;

//     auto enemy = std::make_shared<Enemy>(etype);
//     enemy->setCurrentCell(c);
//     enemy->currentLocation = player->currentLocation;
//     addObject<Enemy>(enemy);
//     log.info(lu::yellow("MAP"),
//            fmt::format("new enemy spawned at {}.{}", c->x, c->y));
//   }

//   if (e.actionPoints > 0) {
//     aiManager->processCommit(creatures, e.actionPoints);
//   }

//   LocationChangeEvent ec(nullptr);
//   eb::EventBus::FireEvent(ec);
// }

// void Location::onEvent(DropEvent &e) {
//   if (e.item == nullptr)
//     return;
//   auto item = e.item->clone();
//   item->currentCell =
//       std::dynamic_pointer_cast<Creature>(e.getSender())->currentCell;
//   addObject(item);
// }

// void Location::onEvent(EnemyDiedEvent &e) {
//   auto sender = e.getSender();
//   if (auto enemy = std::dynamic_pointer_cast<Enemy>(sender)) {
//     enemy->currentCell->features.push_back(CellFeature::BLOOD);
//     auto loot = enemy->drop();
//     if (loot != std::nullopt) {
//       for (auto item : *loot) {
//         item->setCurrentCell(enemy->currentCell);
//         addObject(item);
//       }
//     }
//     invalidateVisibilityCache(enemy->currentCell);
//     removeObject(enemy);
//   }

//   invalidate("enemy died");
// }

// void Location::onEvent(ItemTakenEvent &e) { removeObject(e.item); }

// void Location::reveal() {
//   for (auto r : cells) {
//     for (auto c : r) {
//       c->setVisibilityState(VisibilityState::VISIBLE);
//     }
//   }
//   invalidate("reveal");
// };

// void Location::onEvent(DigEvent &e) {
//   e.cell->type = CellType::FLOOR;
//   e.cell->passThrough = true;
//   e.cell->seeThrough = true;

//   if (R::R() < 0.6f) {
//     auto rock = std::make_shared<Item>(ItemType::ROCK);
//     rock->setCurrentCell(e.cell);
//     addObject(rock);
//   }

//   for (auto c : getNeighbors(e.cell)) {
//     if (c->type == CellType::UNKNOWN) {
//       c->type = CellType::WALL;
//     }
//   }
//   invalidate();
//   LocationChangeEvent ec(nullptr);
//   eb::EventBus::FireEvent(ec);
// }

// ItemsFoundEvent::ItemsFoundEvent(eb::ObjectPtr s, Items i)
//     : eb::Event(s), items(i) {}

// void Location::onEvent(EnterCellEvent &e) {
//   if (auto hero = std::dynamic_pointer_cast<Player>(e.getSender())) {
//     auto items = utils::castObjects<Item>(getObjects(e.cell));
//     if (items.size() > 0) {
//       ItemsFoundEvent ie(nullptr, items);
//       eb::EventBus::FireEvent(ie);
//     }
//   }
//   invalidateVisibilityCache(e.cell);
//   updateView(player);
//   invalidate("enter cell");
// }
// void Location::onEvent(LeaveCellEvent &e) {
// invalidateVisibilityCache(e.cell); }

// void Location::enter(std::shared_ptr<Player> hero, std::shared_ptr<Cell>
// cell) {
//   invalidate("enter location");
//   player = hero;
//   hero->setCurrentCell(cell);

//   for (auto o : objects) {
//     if (auto enemy = std::dynamic_pointer_cast<Enemy>(o)) {
//       enemy->handlers.push_back(
//           eb::EventBus::AddHandler<CommitEvent>(*enemy, hero));
//     }
//   }
//   hero->viewField = hero->calcViewField();
//   updateView(hero);

//   hero->commit("location enter", 0);

//   handlers.push_back(eb::EventBus::AddHandler<CommitEvent>(*this));
//   handlers.push_back(eb::EventBus::AddHandler<EnemyDiedEvent>(*this));
//   handlers.push_back(eb::EventBus::AddHandler<ItemTakenEvent>(*this));
//   handlers.push_back(eb::EventBus::AddHandler<EnterCellEvent>(*this));
//   handlers.push_back(eb::EventBus::AddHandler<LeaveCellEvent>(*this));
//   handlers.push_back(eb::EventBus::AddHandler<DigEvent>(*this, hero));
//   handlers.push_back(eb::EventBus::AddHandler<DropEvent>(*this));
//   handlers.push_back(eb::EventBus::AddHandler<DoorOpenedEvent>(*this));

//   aiManager = std::make_shared<AiManager>(hero->currentLocation);
// }

// void Location::leave(std::shared_ptr<Player> hero) {
//   clearHandlers();

//   for (auto o : objects) {
//     if (auto enemy = std::dynamic_pointer_cast<Enemy>(o)) {
//       enemy->clearHandlers();
//     }
//   }
// }

// void Location::updateLight(std::shared_ptr<Player> hero) {
//   if (!needUpdateLight)
//     return;
//   std::string label = lu::yellow("update light");
//   log.start(lu::yellow("MAP"), label);
//   auto heroVD = hero->VISIBILITY_DISTANCE(hero.get());
//   auto enemies = utils::castObjects<Enemy>(objects);
//   std::vector<std::shared_ptr<Object>> torches;

//   std::vector<std::shared_ptr<Cell>> darkness;
//   for (auto t : utils::castObjects<Terrain>(objects)) {
//     if (t->type == TerrainType::DARKNESS) {
//       darkness.push_back(t->currentCell);
//     }
//   }

//   auto heroInDark = std::find(darkness.begin(), darkness.end(),
//                               hero->currentCell) != darkness.end();

//   for (auto ts : objects) {
//     if (std::find(darkness.begin(), darkness.end(), ts->currentCell) !=
//         darkness.end()) {
//       continue;
//     }
//     if (ts->getGlow()) {
//       torches.push_back(ts);
//     }
//   }
//   for (auto r : cells) {
//     for (auto c : r) {
//       if (c->type == CellType::UNKNOWN)
//         continue;
//       c->lightSources.clear();
//       c->seeThrough = c->type.seeThrough;
//       if (std::find_if(objects.begin(), objects.end(),
//                        [c](std::shared_ptr<Object> o) {
//                          return o->currentCell == c && !o->seeThrough;
//                        }) != objects.end() ||
//           c == player->currentCell) {
//         c->seeThrough = false;
//       }
//       c->passThrough = c->type.passThrough;
//       if (std::find_if(objects.begin(), objects.end(),
//                        [c](std::shared_ptr<Object> o) {
//                          return o->currentCell == c && !o->passThrough;
//                        }) != objects.end() ||
//           c == player->currentCell) {
//         c->passThrough = false;
//       }
//       c->setIlluminated(false);
//       // FIXME: not in distance, but only visible
//       if (!heroInDark && hero->getGlow() &&
//           getDistance(c, hero->currentCell) <= heroVD) {
//         c->setIlluminated(true);
//         c->lightSources.insert(hero);
//         continue;
//       }
//     }
//   }

//   std::vector<std::shared_ptr<Object>> ts;
//   for (auto t : torches) {
//     ts.push_back(t);
//     auto glow = *t->getGlow();
//     for (auto c : getVisible(t->currentCell, glow.distance)) {
//       auto d = sqrt(pow(t->currentCell->x - c->x, 2) +
//                     pow(t->currentCell->y - c->y, 2));
//       if (d <= glow.distance) {
//         c->lightSources.insert(t);
//         c->setIlluminated(true);
//       }
//     }
//   }
//   for (auto e : enemies) {
//     if (e->hasLight()) {
//       auto glow = *e->getGlow();
//       for (auto c : getVisible(e->currentCell, glow.distance)) {
//         c->lightSources.insert(e);
//         c->setIlluminated(true);
//       }
//     }
//   }

//   for (auto r : cells) {
//     for (auto c : r) {
//       if (!c->illuminated) {
//         c->setIllumination(Cell::DEFAULT_LIGHT);
//         continue;
//       }
//       std::vector<std::shared_ptr<Object>> lss;
//       for (auto ls : c->lightSources) {
//         if (ls->getGlow()) {
//           lss.push_back(ls);
//         }
//       }
//       auto d = std::numeric_limits<int>::max();
//       if (lss.size() == 0) {
//         c->setIllumination(Cell::DEFAULT_LIGHT);
//         continue;
//       }
//       for (auto ls : lss) {
//         auto td = sqrt(pow(c->x - ls->currentCell->x, 2) +
//                        pow(c->y - ls->currentCell->y, 2));
//         if (td < d) {
//           d = td;
//           c->nearestLightEmitter = ls;
//         }
//         auto strength = (*ls->getGlow()).distance;
//         if (ls == player) {
//           strength = TORCH_DISTANCE;
//         }
//         c->setIllumination(((strength - td) / strength * 80) +
//         Cell::DEFAULT_LIGHT);
//       }
//       if (c->illumination < Cell::MINIMUM_LIGHT) {
//         c->setIllumination(Cell::MINIMUM_LIGHT);
//       } else if (c->illumination > 100) {
//         c->setIllumination(100);
//       }
//     }
//   }

//   log.stop(label);
//   needUpdateLight = false;
// }

// void Location::updateView(std::shared_ptr<Player> hero) {
//   for (auto r : cells) {
//     for (auto c : r) {
//       if (c->type == CellType::UNKNOWN)
//         continue;
//       if (hero->canSee(c) || hero->hasTrait(Traits::MIND_SIGHT)) {
//         c->setVisibilityState(VisibilityState::VISIBLE);
//       } else if (c->visibilityState == VisibilityState::VISIBLE) {
//         c->setVisibilityState(VisibilityState::SEEN);
//       }
//     }
//   }
// }

// TODO: define cost in cell types
float Location::LeastCostEstimate(void *stateStart, void *stateEnd) {
  auto c = static_cast<Cell *>(stateStart);
  auto cc = static_cast<Cell *>(stateEnd);
  auto d = sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
  if (c->type == CellType::WATER || cc->type == CellType::WATER) {
    d += 100;
  }
  return d;
}
void Location::AdjacentCost(void *state,
                            MP_VECTOR<micropather::StateCost> *neighbors) {
  auto cell = static_cast<Cell *>(state);

  for (auto n : getNeighbors(cell)) {
    if (!n->passThrough)
      continue;
    micropather::StateCost nodeCost = {
        (void *)&(*n),
        LeastCostEstimate(state, (void *)&(*n)),
    };
    neighbors->push_back(nodeCost);
  }
}
void Location::PrintStateInfo(void *state){};

std::vector<std::shared_ptr<Cell>>
Location::getVisible(std::shared_ptr<Cell> start, float distance,
                     bool useIllumination = true) {
  visibilityCacheMutex.lock();
  auto it = visibilityCache.find({start, distance});
  if (it != visibilityCache.end()) {
    auto res = visibilityCache.at({start, distance});
    visibilityCacheMutex.unlock();
    return res;
  }
  visibilityCacheMutex.unlock();
  std::vector<std::shared_ptr<Cell>> result;
  fov::Vec creaturePoint{start->x, start->y};
  fov::Vec bounds{(int)cells.front().size(), (int)cells.size()};
  auto os = start->seeThrough;
  start->seeThrough = true;
  auto field = fov::refresh(creaturePoint, bounds, cells);
  for (auto v : field) {
    auto c = cells.at(v.y).at(v.x);
    auto d = sqrt(pow(start->x - c->x, 2) + pow(start->y - c->y, 2));
    if ((useIllumination && c->illuminated) || d <= distance) {
      result.push_back(c);
    }
  }
  auto ip = std::unique(result.begin(), result.end(), [](auto a, auto b) {
    return a->x == b->x && a->y == b->y;
  });
  result.resize(std::distance(result.begin(), ip));
  start->seeThrough = os;
  visibilityCacheMutex.lock();
  visibilityCache[{start, distance}] = result;
  visibilityCacheMutex.unlock();
  return result;
}

std::vector<std::shared_ptr<Cell>> Location::getLine(std::shared_ptr<Cell> c1,
                                                     std::shared_ptr<Cell> c2) {
  std::vector<std::shared_ptr<Cell>> result;
  auto xn = c1->x;
  auto xk = c2->x;
  auto yn = c1->y;
  auto yk = c2->y;
  int dx, dy, s, sx, sy, kl, swap, incr1, incr2;

  /* Вычисление приращений и шагов */
  sx = 0;
  if ((dx = xk - xn) < 0) {
    dx = -dx;
    --sx;
  } else if (dx > 0)
    ++sx;
  sy = 0;
  if ((dy = yk - yn) < 0) {
    dy = -dy;
    --sy;
  } else if (dy > 0)
    ++sy;
  /* Учет наклона */
  swap = 0;
  if ((kl = dx) < (s = dy)) {
    dx = s;
    dy = kl;
    kl = s;
    ++swap;
  }
  s = (incr1 = 2 * dy) - dx; /* incr1 - констан. перевычисления */
  /* разности если текущее s < 0  и  */
  /* s - начальное значение разности */
  incr2 = 2 * dx; /* Константа для перевычисления    */
                  /* разности если текущее s >= 0    */
  result.push_back(cells[yn][xn]);
  while (--kl >= 0) {
    if (s >= 0) {
      if (swap)
        xn += sx;
      else
        yn += sy;
      s -= incr2;
    }
    if (swap)
      yn += sy;
    else
      xn += sx;
    s += incr1;
    result.push_back(cells[yn][xn]);
  }
  return result;
}

entt::entity Location::addEntity(std::string typeKey,
                                 std::shared_ptr<Cell> cell) {
  auto &data = entt::locator<GameData>::value();
  auto &registry = entt::locator<entt::registry>::value();
  auto &emitter = entt::locator<event_emitter>::value();
  //if (data.prototypes == nullptr) {
  if (!registry.view<entt::tag<"proto"_hs>>().size() ) {
      log.error("No prototypes");
      return entt::null;
  }
  // log.debug("Add entity: {} @ {}.{}", typeKey, cell->x, cell->y);
  auto e = registry.create();

  auto ne = registry.create();
  // for (auto e : data.prototypes.group(entt::get<entt::tag<"proto"_hs>)) {
  for (auto e : registry.view<entt::tag<"proto"_hs>,hf::meta>()) {
    if (registry.get<hf::meta>(e).id == typeKey) {
      // registry.push(ne, e, *data.prototypes,
      //                entt::exclude<entt::tag<"proto"_hs>>);

      for (auto [id, storage] : registry.storage()) {
        if (storage.contains(e)) {
          storage.push(ne, storage.value(e));
        }
      }
      registry.emplace<hf::ingame>(ne);
      registry.remove<entt::tag<"proto"_hs>>(ne);
      registry.emplace<hf::position>(ne, cell->x, cell->y, 0);
      auto meta = registry.get<hf::meta>(e);
      meta.id = fmt::format("{}_{}", meta.id, (int)ne);
      registry.emplace_or_replace<hf::meta>(ne, meta);
      // log.debug("Add entity: {} @ {}.{}", meta.id, cell->x, cell->y);
      if (registry.any_of<hf::script>(e)) {
        auto script = registry.get<hf::script>(e);
        emitter.publish(script_event{e, script.path, "init"});
      }
      break;
    }
  }
  if (!registry.valid(ne)) {
    log.error("Cannot create: {} @ {}", typeKey, cell->getSId());
  }
  return ne;
}

entt::entity Location::addTerrain(std::string typeKey,
                                  std::shared_ptr<Cell> cell) {
  auto &data = entt::locator<GameData>::value();
  // if (data.prototypes == nullptr) {
  // if (!data.prototypes.storage<entt::entity>().size() ) {
  if (!registry.view<entt::tag<"proto"_hs>>().size() ) {
      log.error("No prototypes");
      return entt::null;
  }
  auto e = registry.create();

  auto ne = registry.create();
  // for (auto e : registry.view<entt::tag<"proto"_hs>, entt::tag<"terrain"_hs>>()) {
  for (auto e : registry.view<entt::tag<"proto"_hs>>()) {
    if (registry.get<hf::meta>(e).id == typeKey) {
      // registry.push(ne, e, *data.prototypes,
      //                 entt::exclude<entt::tag<"proto"_hs>>);
      for (auto [id, storage] : registry.storage()) {
        if (storage.contains(e)) {
          storage.push(ne, storage.value(e));
        }
      }
      registry.emplace<hf::ingame>(ne);
      registry.remove<entt::tag<"proto"_hs>>(ne);
      registry.emplace<hf::position>(ne, cell->x, cell->y, 0);
      auto meta = registry.get<hf::meta>(e);
      meta.id = fmt::format("{}_{}", meta.id, (int)ne);
      registry.emplace_or_replace<hf::meta>(ne, meta);
      break;
    }
  }
  if (!registry.valid(ne)) {
    log.error("Cannot create: {} @ {}", typeKey, cell->getSId());
  }
  return ne;
}

std::optional<std::shared_ptr<Cell>> Location::getCellByEntity(entt::entity e) {
  std::optional<std::shared_ptr<Cell>> cell;
  auto &registry = entt::locator<entt::registry>::value();
  auto p = registry.get<hf::position>(e);
  cell = cells[p.y][p.x];
  return cell;
}
