#include <chrono>
#include <fmt/format.h>
#include <iostream>
#include <set>

#include <algorithm>
#include <execution>

// #include "EventBus/EventBus.hpp"
// #include "lss/game/trigger.hpp"
#include "lss/game/content/enemies.hpp"
#include "lss/game/door.hpp"
// #include "lss/game/enemy.hpp"
#include "lss/game/content/items.hpp"
#include "lss/game/item.hpp"
// #include "lss/game/player.hpp"
#include "lss/game/region.hpp"
#include "lss/game/terrain.hpp"
#include "lss/generator/generator.hpp"
#include "lss/generator/lootTable.hpp"
#include "lss/generator/mapUtils.hpp"
#include "lss/generator/spawnTable.hpp"
#include "lss/utils.hpp"
#include <liblog/liblog.hpp>

#ifndef GEN_DEBUG
#define GEN_DEBUG false
#endif

void dlog(std::string msg) {
  if (GEN_DEBUG)
    fmt::print("{}\n", msg);
}

int WIDTH = 100;
int HEIGHT = 100;

void placeWalls(std::shared_ptr<Region> region) {
  dlog("place walls");
  auto _cells = region->getCells();
  for (auto r : *_cells) {
    for (auto cell : r) {
      if (cell->type == CellType::UNKNOWN) {
        auto n = region->getNeighbors(cell);
        if (auto nc = std::find_if(n.begin(), n.end(),
                                   [](std::shared_ptr<Cell> c) {
                                     return c->passThrough ||
                                            c->type == CellType::DOWNSTAIRS ||
                                            c->type == CellType::WATER ||
                                            c->type == CellType::VOID ||
                                            c->type == CellType::UPSTAIRS;
                                   });
            nc != n.end()) {
          cell->type = CellType::WALL;
          cell->seeThrough = false;
          cell->passThrough = false;
          cell->features = (*nc)->features;
        }
      }
      if (cell == r.front() || cell == r.back()) {
        if (cell->type != CellType::UNKNOWN) {
          cell->type = CellType::WALL;
          cell->seeThrough = false;
          cell->passThrough = false;
        }
      }
    }
  }
  for (auto cell : _cells->front()) {
    if (cell->type != CellType::UNKNOWN) {
      cell->type = CellType::WALL;
      cell->seeThrough = false;
      cell->passThrough = false;
    }
  }
  for (auto cell : _cells->back()) {
    if (cell->type != CellType::UNKNOWN) {
      cell->type = CellType::WALL;
      cell->seeThrough = false;
      cell->passThrough = false;
    }
  }
}

std::shared_ptr<Region> makePassage(std::shared_ptr<Cell> start, Direction dir,
                                    int length) {
  auto rh = 1;
  auto rw = 1;
  switch (dir) {
  case Direction::N:
  case Direction::S:
    rh = length;
    break;
  case Direction::W:
  case Direction::E:
    rw = length;
    break;
  default:
    // TODO: fix
    rh = length;
  }
  auto cells = mapUtils::fill(rh, rw, CellType::FLOOR);
  auto room = std::make_shared<Region>(std::make_shared<Cells>(cells));

  // for (auto r : cells) {
  //   for (auto c : r) {
  //     c->room = room;
  //   }
  // }
  return room;
}

void makePassageBetweenRooms(std::shared_ptr<Region> region,
                             std::shared_ptr<Region> room1,
                             std::shared_ptr<Region> room2) {
  dlog("passages");
  auto sc = room1->getRandomCell();
  auto ec = room2->getRandomCell();
  while (sc == ec) {
    ec = room2->getRandomCell();
  }

  if (std::find(room1->features.begin(), room1->features.end(),
                RegionFeature::DUNGEON) == room1->features.end())
    room1->features.push_back(RegionFeature::DUNGEON);
  if (std::find(room2->features.begin(), room2->features.end(),
                RegionFeature::DUNGEON) == room2->features.end())
    room2->features.push_back(RegionFeature::DUNGEON);

  sc->x += room1->x;
  sc->y += room1->y;
  ec->x += room2->x;
  ec->y += room2->y;
  if (sc->y > ec->y) {
    std::swap(sc, ec);
  }

  auto passage = makePassage(sc, S, ec->y - sc->y + 1);
  region->place(passage, sc->x, sc->y);
  passage->features.push_back(RegionFeature::DUNGEON);

  auto _cells = passage->getCells();
  sc = _cells->back().back();
  sc->x += passage->x;
  sc->y += passage->y;
  if (sc != ec) {
    if (sc->x > ec->x) {
      std::swap(sc, ec);
    }
    passage = makePassage(sc, E, ec->x - sc->x + 1);
    region->place(passage, sc->x, sc->y);
    passage->features.push_back(RegionFeature::DUNGEON);
  }
  dlog("passages end");
}

std::pair<std::shared_ptr<Cell>, Cells>
dig(std::shared_ptr<Cell> start, Direction dir, int length, int minWidth,
    int jWidth, int wind = 0, CellSpec type = CellType::GROUND) {
  auto cell = start;
  std::vector<CellFeature> f = {CellFeature::CAVE};
  Cells dCells = mapUtils::fill(HEIGHT, WIDTH, CellType::UNKNOWN);
  for (auto step = 0; step < length; step++) {
    auto width = (jWidth > 1 ? rand() % jWidth : 0) + minWidth;
    // TODO: make wind configurable without crazy magic
    auto w = wind == -1 ? rand() % (width >= 3 ? width * 3 / 4 : 2) : wind;
    if (w == 1) {
      w = rand() % 2 ? 0 : 1;
    }
    auto offset = rand() % 2 ? -1 : 1;
    auto ox = cell->x;
    auto oy = cell->y;
    auto nx = ox;
    auto ny = oy;
    switch (dir) {
    case Direction::N:
      ox -= width / 2;
      for (auto n = 0; n < width; n++) {
        ox++;
        if (oy < 0 || ox < 0 || oy > HEIGHT - 1 || ox > WIDTH - 1)
          continue;
        dCells.at(oy)[ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      ny -= 1;
      nx += offset * w;
      break;
    case Direction::S:
      ox -= width / 2;
      for (auto n = 0; n < width; n++) {
        ox++;
        if (oy < 0 || ox < 0 || oy > HEIGHT - 1 || ox > WIDTH - 1)
          continue;
        dCells.at(oy)[ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      ny += 1;
      nx += offset * w;
      break;
    case Direction::W:
      oy -= width / 2;
      for (auto n = 0; n < width; n++) {
        oy++;
        if (oy < 0 || ox < 0 || oy > HEIGHT - 1 || ox > WIDTH - 1)
          continue;
        dCells.at(oy)[ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      nx -= 1;
      ny += offset * w;
      break;
    case Direction::E:
      oy -= width / 2;
      for (auto n = 0; n < width; n++) {
        oy++;
        if (oy < 0 || ox < 0 || oy > HEIGHT - 1 || ox > WIDTH - 1)
          continue;
        dCells.at(oy)[ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      nx += 1;
      ny += offset * w;
      break;
    }
    if (ny < 0 || nx < 0 || ny > HEIGHT - 1 || nx > WIDTH - 1) {
      break;
    };
    cell = dCells.at(ny)[nx];
  }
  return {cell, dCells};
}

std::pair<std::shared_ptr<Cell>, Cells> randomDig(std::shared_ptr<Cell> start,
                                                  Direction dir, int length) {
  int minWidth = 1;
  int jWidth = 6;
  bool wind = -1;
  return dig(start, dir, length, minWidth, jWidth, wind);
}

// TODO: better room merge
void fixOverlapped(std::shared_ptr<Region> region) {
  // dlog("fix overlapped");
  // std::map<std::shared_ptr<Region>, std::vector<std::shared_ptr<Cell>>>
  // cache; for (auto r : region->regions) {
  //   cache[r] = {};
  // }
  // for (auto r = 0; r < HEIGHT; r++) {
  //   for (auto c = 0; c < WIDTH; c++) {
  //     if (region->cells->at(r)[c]->type == CellType::UNKNOWN) {
  //       continue;
  //     }
  //     std::vector<std::shared_ptr<Region>> rooms(region->regions.size());
  //     auto it = std::copy_if(region->regions.begin(), region->regions.end(),
  //                            rooms.begin(), [&](auto room) {
  //                              auto result =
  //                                  room->x <= c && room->x + room->width > c
  //                                  && room->y <= r && room->y + room->height
  //                                  > r;
  //                              return result;
  //                            });
  //     rooms.resize(std::distance(rooms.begin(), it));
  //     if (rooms.size() == 1) {
  //       cache[rooms.front()].push_back(region->cells->at(r)[c]);
  //       region->cells->at(r)[c]->room = rooms.front();
  //     } else if (rooms.size() != 0) {

  //       std::vector<std::shared_ptr<Region>> halls(rooms.size());
  //       auto it =
  //           std::copy_if(rooms.begin(), rooms.end(), halls.begin(),
  //                        [](auto r) { return r->type != RegionType::PASSAGE;
  //                        });
  //       halls.resize(std::distance(halls.begin(), it));
  //       if (halls.size() == 0) {
  //         cache[rooms.front()].push_back(region->cells->at(r)[c]);
  //         region->cells->at(r)[c]->room = rooms.front();
  //       } else if (halls.size() == 1) {
  //         cache[halls.front()].push_back(region->cells->at(r)[c]);
  //         region->cells->at(r)[c]->room = halls.front();
  //       } else {
  //         // TODO: select hall by neighbors
  //         cache[halls.front()].push_back(region->cells->at(r)[c]);
  //         region->cells->at(r)[c]->room = halls.front();
  //       }
  //     }
  //   }
  // }
  // auto rooms = region->regions;
  // for (auto room : rooms) {
  //   // FIXME: remove orphaned objects
  //   room->cells = cache[room];
  //   if (room->cells.size() == 0) {
  //     region->regions.erase(
  //         std::remove(region->regions.begin(), region->regions.end(), room));
  //   }
  // }
}

void Generator::placeDoors(std::shared_ptr<Region> region) {
  // auto data = entt::service_locator<GameData>::get().lock();
  // dlog("place doors");
  // auto d = 0;
  // for (auto r : region->cells) {
  //   for (auto c : r) {
  //     if (c->type == CellType::FLOOR) {
  //       if ((region->cells->at(c->y - 1)[c->x]->type == CellType::WALL &&
  //            region->cells->at(c->y + 1)[c->x]->type == CellType::WALL) ||
  //           (region->cells->at(c->y)[c->x - 1]->type == CellType::WALL &&
  //            region->cells->at(c->y)[c->x + 1]->type == CellType::WALL)) {
  //         auto n = region->getNeighbors(c);
  //         if (std::count_if(n.begin(), n.end(), [&](std::shared_ptr<Cell> nc)
  //         {
  //               return nc->type == CellType::WALL;
  //             }) < 6) {
  //           auto o = region->getObjects(c);
  //           if (R::R() > data->probability["DOOR"] ||
  //           c->hasFeature(CellFeature::CAVE) ||
  //               o.size() > 0)
  //             continue;
  //           auto nd = false;
  //           for (auto nc : n) {
  //             auto no = region->getObjects(nc);
  //             if (std::find_if(no.begin(), no.end(),
  //                              [](std::shared_ptr<Object> ob) {
  //                                return std::dynamic_pointer_cast<Door>(ob);
  //                              }) != no.end()) {
  //               nd = true;
  //               break;
  //             }
  //           }
  //           if (nd)
  //             continue;
  //           auto door = std::make_shared<Door>();
  //           if (R::R() < data->probability["SECRET_DOOR"]) {
  //             door->hidden = true;
  //           }
  //           door->setCurrentCell(c);
  //           c->seeThrough = false;
  //           region->addObject<Door>(door);
  //           d++;
  //         }
  //       }
  //     }
  //   }
  // }
}

void placeLoot(std::shared_ptr<Region> region, int threat) {
  dlog("place loot");
  auto floor = region->type.floor;
  auto table = LootBoxes::ZERO;
  if (region->type.type == RegionType::DUNGEON) {
    if (threat >= LootTable::DUNGEON.size())
      threat = LootTable::DUNGEON.size() - 1;
    table = LootTable::DUNGEON.at(threat);
  } else if (region->type.type == RegionType::CAVERN) {
    // TODO: use cavern table
    if (threat >= int(LootTable::DUNGEON.size()))
      threat = LootTable::DUNGEON.size() - 1;
    table = LootTable::DUNGEON.at(threat);
  }

  // TODO: gen count and get loot from 100% lootbox
  for (auto room : region->regions) {
    auto rc = room->getRandomCell(floor);
    std::shared_ptr<Cell> c;
    if (rc)
      c = *rc;
    else
      continue;
    auto n = 0;
    // TODO: fix crash in getObjects
    // while (region->getObjects(c).size() != 0 && n < 20) {
    //   rc = room->getRandomCell(floor);
    //   if (rc)
    //     c = *rc;
    //   else
    //     continue;
    //   n++;
    // }
    if (n == 20) {
      continue;
    }

    auto loot = table.open();
    for (auto item : loot) {
      auto new_item = item->roll();
      // fmt::print("Place {} at {}.{}\n", new_item->getTitle(true), c->x,
      // c->y);
      // new_item->setCurrentCell(c);
      // region->addObject<Item>(new_item);

      // location->addItem("BONES", 1, c);
    }
  }
}

void placeEnemies(std::shared_ptr<Region> region, int threat) {
  dlog("place enemies");
  std::map<EnemySpec, float> table;
  // auto pather = new micropather::MicroPather(location.get());
  // micropather::MPVector<void *> path;
  float totalCost = 0;

  if (region->type.type == RegionType::DUNGEON) {
    if (threat >= int(SpawnTable::DUNGEON.size()))
      threat = SpawnTable::DUNGEON.size() - 1;
    table = SpawnTable::DUNGEON.at(threat);
  } else if (region->type.type == RegionType::CAVERN) {
    // TODO: use cavern table
    if (threat >= int(SpawnTable::DUNGEON.size()))
      threat = SpawnTable::DUNGEON.size() - 1;
    table = SpawnTable::DUNGEON.at(threat);
  }
  for (auto room : region->regions) {
    if (room->type.type == RegionType::PASSAGE)
      continue;
    auto __cells = room->getCells();
    std::vector<std::shared_ptr<Cell>> _cells;
    for (auto r : *__cells) {
      for (auto c : r) {
        _cells.push_back(c);
      }
    }
    std::vector<std::shared_ptr<Cell>> cells(_cells.size());
    auto it = std::copy_if(
        _cells.begin(), _cells.end(), cells.begin(),
        [&region](auto cell) { return cell->type == region->type.floor; });
    cells.resize(std::distance(cells.begin(), it));
    if (cells.size() == 0)
      continue;
    int count = R::N(1, 2);
    if (count < 0)
      count = 0;
    for (auto n = cells.size() / 64; n > 0; n--) {
      count += R::Z(0, 3);
    }
    for (; count != 0; count--) {
      auto p = R::R();
      EnemySpec type;
      float fullP = 0;
      std::for_each(table.begin(), table.end(),
                    [&](auto rec) { fullP += rec.second; });
      p *= fullP;
      float ap = 0;
      for (auto [et, tp] : table) {
        ap += tp;
        if (p <= ap) {
          type = et;
          break;
        }
      }
      auto c = cells[rand() % cells.size()];

      // pather->Reset();
      // int result =
      //     pather->Solve(c.get(), region->enterCell.get(), &path, &totalCost);
      // if (result != micropather::MicroPather::SOLVED)
      //   break;

      // auto enemy = mapUtils::makeEnemy(location, c, type);
      // region->addObject<Enemy>(enemy);
    }
  }
}

void makeRiver(std::shared_ptr<Region> region) {
  dlog("place river");
  auto _cells = region->getCells();
  auto river = dig(
      _cells->at(
          0)[_cells->front().size() / 3 + rand() % _cells->front().size() / 3],
      Direction::S, _cells->size() - 2, 2, 2, 1, CellType::WATER);
  auto room = std::make_shared<Region>(std::make_shared<Cells>(river.second));
  region->place(room, 0, 0);
}

// TODO: respect void on prev location
// std::shared_ptr<Location>
// Generator::getRandomLocation(std::shared_ptr<Player> hero, int depth,
//                              std::shared_ptr<Cell> enter) {
//   if (depth == -1 && hero->currentLocation == nullptr) {
//     depth = 0;
//   }
//   if (depth == -1) {
//     depth = hero->currentregion->depth + 1;
//   }
//   auto spec = LocationSpec{"Dungeon"};
//   if (R::R() < data->probability["CAVERN"]&& depth >= 3) {
//     spec.type = RegionType::CAVERN;
//     spec.cellFeatures.push_back(CellFeature::CAVE);
//     spec.name = "Cavern";
//   }
//   if (spec.type == RegionType::DUNGEON && R::R() <
//   data->probability["CAVE_PASSAGE"]) {
//     spec.features.push_back(RegionFeature::CAVE_PASSAGE);
//   }
//   if (spec.type == RegionType::CAVERN && R::R() < data->probability["RIVER"])
//   {
//     spec.features.push_back(RegionFeature::RIVER);
//   }
//   if (R::R() < data->probability["TORCHES"]) {
//     spec.features.push_back(RegionFeature::TORCHES);
//   }
//   if (R::R() < data->probability["STATUE"]) {
//     spec.features.push_back(RegionFeature::STATUE);
//   }
//   if (spec.type == RegionType::DUNGEON && R::R() <
//   data->probability["ALTAR"]) {
//     spec.features.push_back(RegionFeature::ALTAR);
//   }
//   if (spec.type == RegionType::DUNGEON && R::R() <
//   pdata->robabilities["TREASURE_SMALL"]) {
//     spec.features.push_back(RegionFeature::TREASURE_SMALL);
//   }
//   if (R::R() < data->probability["VOID"]) {
//     spec.features.push_back(RegionFeature::VOID);
//   }
//   if (R::R() < data->probability["ICE"]) {
//     spec.features.push_back(RegionFeature::ICE);
//   }
//   if (R::R() < data->probability["BONES_FIELD"]) {
//     spec.features.push_back(RegionFeature::BONES_FIELD);
//   }
//   if (R::R() < data->probability["CORRUPT"]) {
//     spec.features.push_back(RegionFeature::CORRUPT);
//   }
//   if (R::R() < data->probability["HEAL"]) {
//     spec.features.push_back(RegionFeature::HEAL);
//   }
//   if (R::R() < data->probability["MANA"]) {
//     spec.features.push_back(RegionFeature::MANA);
//   }
//   if (spec.type == RegionType::CAVERN && R::R() < data->probability["LAKE"])
//   {
//     spec.features.push_back(RegionFeature::LAKE);
//   }
//   spec.threat = depth;
//   spec.enterCell = enter;
//   auto l = getLocation(spec);
//   l->depth = depth;
//   return l;
// }

// std::shared_ptr<Terrain> makeTorch() {
//     auto data = entt::service_locator<GameData>::get().lock();
//     auto torch =
//     std::make_shared<Terrain>(data->terrainSpecs["TORCH_STAND"]);
//     //
//     torch->triggers.push_back(std::make_shared<PickTrigger>([=](std::shared_ptr<Creature>
//     actor){
//     //   auto hero = std::dynamic_pointer_cast<Player>(actor);
//     //   return Triggers::TORCH_STAND_TRIGGER(hero, torch);
//     // }));
//     return torch;
// }

void placeTorches(std::shared_ptr<Region> region) {
  // dlog("place torches");
  // auto floor = region->type.floor;
  // auto tc = rand() % 7 + 3;
  // int n;
  // for (n = 0; n < tc;) {
  //   auto room = region->regions[rand() % region->regions.size()];
  //   auto cell = room->cells[rand() % room->cells.size()];
  //   if (cell->type != floor || region->getObjects(cell).size() > 0)
  //     continue;
  //   auto ngs = region->getNeighbors(cell);
  //   if (std::find_if(ngs.begin(), ngs.end(), [](std::shared_ptr<Cell> nc) {
  //         return nc->type == CellType::WALL;
  //       }) == ngs.end())
  //     continue;
  //   region->addTerrain("TORCH_STAND", cell);
  //   n++;
  // }
  // auto nbrs = region->getNeighbors(region->exitCell);
  // region->addTerrain("TORCH_STAND", nbrs[rand() % nbrs.size()]);

  // nbrs = region->getNeighbors(region->enterCell);
  // region->addTerrain("TORCH_STAND", nbrs[rand() % nbrs.size()]);

  // fmt::print("Torches: {}\n", n);
}

bool placeStairs(std::shared_ptr<Region> region) {
  // dlog("place stairs");

  // auto floor = region->type.floor;
  // auto entr = getRandomCell(location, floor);
  // if (region->type.enterCell != nullptr) {
  //   auto e = region->type.enterCell;
  //   entr =
  //       std::make_optional<std::shared_ptr<Cell>>(region->cells->at(e->y)[e->x]);
  // }
  // dlog("entr found 1");
  // auto exir = getRandomCell(location, floor);
  // dlog("exir found 1");
  // while (!entr) {
  //   entr = getRandomCell(location, floor);
  // }
  // dlog("entr found");
  // while (!exir) {
  //   exir = getRandomCell(location, floor);
  // }
  // dlog("exir found");
  // region->enterCell = *entr;
  // region->exitCell = *exir;
  // // region->dump();

  // auto pather = new micropather::MicroPather(location.get());
  // micropather::MPVector<void *> path;
  // float totalCost = 0;
  // pather->Reset();
  // int result = pather->Solve(region->enterCell.get(),
  //                            region->exitCell.get(), &path, &totalCost);

  // auto i = 0;
  // while (result != micropather::MicroPather::SOLVED ||
  //        (totalCost < 20 && i < 10)) {
  //   // entr = mapUtils::getRandomCell(location, CellType::FLOOR);
  //   exir = getRandomCell(location, floor);
  //   // while (!entr) {
  //   // entr = mapUtils::getRandomCell(location, CellType::FLOOR);
  //   // }
  //   while (!exir) {
  //     exir = getRandomCell(location, floor);
  //   }
  //   region->enterCell = *entr;
  //   region->exitCell = *exir;
  //   pather->Reset();
  //   result = pather->Solve(region->enterCell.get(), region->exitCell.get(),
  //                          &path, &totalCost);
  //   i++;
  //   if (i == 30) {
  //     region->log.warn(lu::green("MAPGEN"), "cannot place exit");
  //     delete pather;
  //     return false;
  //   }
  // }
  // // fmt::print("{}.{} -> {}.{} = {}\n", region->enterCell->x,
  // //            region->enterCell->y, region->exitCell->x,
  // //            region->exitCell->y, totalCost);

  // delete pather;

  // auto _objects = region->objects;
  // for (auto o: _objects) {
  //     if (o->currentCell != nullptr && (o->currentCell == region->exitCell ||
  //         o->currentCell == region->enterCell)) {
  //       region->removeObject(o);
  //     }
  // }

  // region->exitCell->type = CellType::DOWNSTAIRS;
  // auto n = region->getNeighbors(region->exitCell);
  // // FIXME: crash
  // std::for_each(n.begin(), n.end(), [location](auto c) {
  //   mapUtils::makeFloor(c);
  //   c->room = region->exitCell->room;
  //   if (c->room) {
  //     c->room->cells.push_back(c);
  //   }
  // });
  // region->enterCell->type = CellType::UPSTAIRS;
  // n = region->getNeighbors(region->enterCell);
  // std::for_each(n.begin(), n.end(), [location](auto c) {
  //   mapUtils::makeFloor(c);
  //   c->room = region->enterCell->room;
  //   if (c->room) {
  //     c->room->cells.push_back(c);
  //   }
  // });
  return true;
}

void makePassages(std::shared_ptr<Region> region) {
  dlog("place passages");
  auto pc = rand() % 10;
  auto n = 0;
  std::set<std::shared_ptr<Region>> rooms;
  std::vector<std::shared_ptr<Region>> halls = region->regions;

  while (n < pc) {
    auto room1 = halls[rand() % halls.size()];
    auto room2 = halls[rand() % halls.size()];
    if (room1 == room2)
      continue;
    if (std::find(halls.begin(), halls.end(), room1) != halls.end())
      rooms.insert(room1);
    if (std::find(halls.begin(), halls.end(), room2) != halls.end())
      rooms.insert(room2);
    n++;
    makePassageBetweenRooms(region, room1, room2);
  }
  for (auto r : halls) {
    if (std::find(rooms.begin(), rooms.end(), r) == rooms.end()) {
      auto room2 = region->regions[rand() % region->regions.size()];
      while (room2 == r) {
        room2 = region->regions[rand() % region->regions.size()];
      }
      makePassageBetweenRooms(region, r, room2);
    }
    // for (auto c : r->cells) {
    //   c->room = r;
    // }
  }
}

void Generator::makeExterior(std::shared_ptr<Region> region) {
  auto data = entt::service_locator<GameData>::get().lock();
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  dlog("place caves");
  auto rc = rand() % 20 + 35;

  region->resize(WIDTH * 2, HEIGHT * 2, CellType::UNKNOWN);
  for (auto n = 0; n < rc; n++) {
    auto room = Region::createRandomRegion(34, 12, 34, 12, CellType::WALL);

    auto _cells = room->getCells();
    for (auto r : *_cells) {
      for (auto c : r) {
        if (R::R() > 0.45) {
          c->type = CellType::GROUND;
          c->passThrough = true;
        }
      }
    }
    auto ry = rand() % (region->height - room->height - 8) + 8;
    auto rx = rand() % (region->width - room->width - 8) + 8;
    region->place(room, rx, ry);
    room->threat = rand() % 4;
    room->features.push_back(RegionFeature::CAVE);
  }
  fixOverlapped(region);

  emitter->publish<location_update_event>();
  makePassages(region);

  emitter->publish<location_update_event>();
  region->flatten();
  // region->printRegion();
  fmt::print("ex generating\n");
  auto _cells = region->getCells();
  for (auto n = 0; n < rand() % 8 + 4; n++) {
    // std::shuffle(_cells.begin(), _cells.end(),
    //              std::default_random_engine(rand()));
    fmt::print("<");
    std::for_each(
        std::execution::par, _cells->begin(), _cells->end(), [&](auto r) {
          std::for_each(std::execution::par, r.begin(), r.end(), [&](auto c) {
            auto fn = region->countNeighbors(c, [=](std::shared_ptr<Cell> nc) {
              return nc->type != CellType::UNKNOWN ||
                     nc->type == CellType::EMPTY;
            });
            if (c->type == CellType::UNKNOWN || c->type == CellType::EMPTY) {
              if ((fn >= 4 && rand() % 10 < 5) || fn == 3) {
                region->updateCell(c->x, c->y, c->z, CellType::GROUND,
                                   c->features);
              }
            }
          });
          fmt::print(".");
          region->invalidate();
        });
    fmt::print(">\n");
    emitter->publish<location_update_event>();
  }
  // region->printRegion();

  std::for_each(
      std::execution::par, _cells->begin(), _cells->end(), [&](auto r) {
        std::for_each(std::execution::par, r.begin(), r.end(), [&](auto c) {
          if (c->type != CellType::UNKNOWN && c->type != CellType::EMPTY) {
            region->updateCell(c->x, c->y, c->z, CellType::GROUND, c->features);
            if (R::R() < data->probability["EXT_BUSH"]) {
              location->addTerrain("BUSH", c);
            } else if (R::R() < data->probability["EXT_TREE"]) {
              location->addTerrain("TREE", c);
            } else if (R::R() < data->probability["EXT_GRASS"]) {
              // auto grass = Prototype::GRASS->clone();
              // grass->setCurrentCell(c);
              // region->addObject<Item>(grass);
              // region->addTerrain("GRASS", c);
              // auto grass = Prototype::GRASS->clone();
            }
          }
        });
      });
  emitter->publish<location_update_event>();
}

void placeRooms(std::shared_ptr<Region> region) {
  dlog("place rooms");
  auto rc = rand() % 12 + 7;

  region->resize(WIDTH, HEIGHT, CellType::UNKNOWN);
  // TODO: ensure place in location borders
  // if (region->type.enterCell != nullptr) {
  //   auto room = Region::createRandomRegion(5, 3, 5, 3);
  //   auto e = region->type.enterCell;
  //   auto rx = e->x - room->width / 2;
  //   auto ry = e->y - room->height / 2;
  //   mapUtils::paste(room->cells, location, rx, ry);
  //   room->x = rx;
  //   room->y = ry;
  //   room->threat = rand() % 4;
  //   region->regions.push_back(room);
  // }

  auto _cells = region->getCells();
  for (auto n = 0; n < rc; n++) {
    auto room = Region::createRandomRegion();
    auto ry = rand() % (region->height - room->height - 2) + 2;
    auto rx = rand() % (region->width - room->width - 2) + 2;
    region->place(room, rx, ry);
    room->threat = rand() % 4;
  }
}

void Generator::placeCaves(std::shared_ptr<Region> region) {
  auto data = entt::service_locator<GameData>::get().lock();
  dlog("place caves");
  auto rc = rand() % 12 + 7;

  region->resize(WIDTH, HEIGHT, CellType::UNKNOWN);
  auto _cells = region->getCells();
  for (auto n = 0; n < rc; n++) {
    auto room = Region::createRandomRegion(44, 12, 44, 12, CellType::WALL);

    for (auto r : *_cells) {
      for (auto c : r) {
        if (R::R() > data->probability["CAVERN_WALL"]) {
          c->type = CellType::GROUND;
          c->passThrough = true;
        }
      }
    }
    auto ry = rand() % (_cells->size() - room->height - 2) + 2;
    auto rx = rand() % (_cells->front().size() - room->width - 2) + 2;
    region->place(room, rx, ry);
    room->threat = rand() % 4;
    room->features.push_back(RegionFeature::CAVE);
  }
  fixOverlapped(region);

  for (auto i = 0; i < 5; i++) {
    for (auto room : region->regions) {
      room->type.type = RegionType::CAVERN;
      auto _cells = room->getCells();
      for (auto r : *_cells) {
        for (auto c : r) {
          auto n = region->getNeighbors(c);
          auto fn =
              std::count_if(n.begin(), n.end(), [](std::shared_ptr<Cell> nc) {
                return nc->type == CellType::GROUND;
              });

          if (c->type == CellType::GROUND) {
            if (fn < 4) {
              c->type = CellType::WALL;
              c->passThrough = false;
            }
          } else if (c->type == CellType::WALL) {
            if (fn >= 6) {
              c->type = CellType::GROUND;
              c->passThrough = true;
            }
          }
        }
      }
    }
  }

  for (auto room : region->regions) {
    auto _cells = room->getCells();
    for (auto r : *_cells) {
      for (auto c : r) {
        if (c->type == CellType::UNKNOWN)
          continue;
        auto n = region->getNeighbors(c);
        auto fn =
            std::count_if(n.begin(), n.end(), [](std::shared_ptr<Cell> nc) {
              return nc->type == CellType::GROUND;
            });
        if (fn == 0) {
          c->type = CellType::UNKNOWN;
        } else if (fn != 8) {
          if (c->type == CellType::GROUND &&
              R::R() < data->probability["CAVE_ROCK"]) {
            auto rock = Prototype::ROCK->clone();
            rock->setCurrentCell(c);
            // region->addObject<Item>(rock);
            continue;
          }
        } else if (c->type == CellType::GROUND &&
                   R::R() < data->probability["CAVE_GRASS"]) {
          if (R::R() <
              data->probability["CAVE_BUSH"]) { /*||
      std::find_if(n.begin(), n.end(), [&](auto nc) {
        auto t = utils::castObjects<Terrain>(region->getObjects(nc));
        return t.size() == 1 && t.front()->type == data->terrainSpecs["BUSH"];
      }) != n.end()*/
            location->addTerrain("BUSH", c);
            // s->triggers.push_back(std::make_shared<EnterTrigger>([=](std::shared_ptr<Creature>
            // actor){
            //   return Triggers::BUSH_TRIGGER(c, location);
            // }));
          } else {
            auto grass = Prototype::GRASS->clone();
            grass->setCurrentCell(c);
            // region->addObject<Item>(grass);
          }
          continue;
        }
      }
    }
  }
}

void Generator::makeCavePassage(std::shared_ptr<Region> region) {
  auto data = entt::service_locator<GameData>::get().lock();
  dlog("place cave passage");
  std::vector<Direction> ds{N, E, S, W};
  auto room = region->regions[rand() % region->regions.size()];
  auto cell = room->getRandomCell();
  for (auto n = 0; n < rand() % 6 + 2; n++) {
    auto res = randomDig(cell, ds[rand() % ds.size()], rand() % 30);
    cell = res.first;
    auto newRoom =
        std::make_shared<Region>(std::make_shared<Cells>(res.second));
    room->features.push_back(RegionFeature::CAVE);
    region->place(newRoom, 0, 0);
    newRoom->threat = rand() % 4;
    for (auto r : *newRoom->getCells()) {
      for (auto c : r) {
        if (c->type == CellType::GROUND &&
            R::R() < data->probability["CAVE_ROCK"]) {
          auto rock = Prototype::ROCK->clone();
          rock->setCurrentCell(c);
          // region->addObject<Item>(rock);
        } else if (c->type == CellType::GROUND &&
                   R::R() < data->probability["CAVE_GRASS"]) {
          if (R::R() < data->probability["CAVE_BUSH"]) {
            location->addTerrain("BUSH", c);
          } else {
            auto grass = Prototype::GRASS->clone();
            grass->setCurrentCell(c);
            // region->addObject<Item>(grass);
          }
          continue;
        }
      }
    }
  }
}

/*
void placeTemplateInRoom(std::shared_ptr<Region> region,
                         std::shared_ptr<RoomTemplate> rtp) {
  //   auto room = rtp->generate(region);
  //   // Room::printRoom(location, room);

  //   //TODO: shuffle suitable rooms and select
  //   auto target = region->regions[rand() % region->regions.size()];
  //   auto n = 0;
  //   while (target->width - 2 < room->width || target->height -2 <
  //   room->height || target->x == 0) {
  //     target = region->regions[rand() % region->regions.size()];
  //     if (n > 30) {
  //       region->log.warn(lu::green("MAPGEN"), "cannot place template");
  //       return;
  //     }
  //     n++;
  //   }

  //   mapUtils::paste(room->cells, location, target->x+1, target->y+1);

  //   region->regions.push_back(room);
  }

  void placeTemplate(std::shared_ptr<Region> region,
                     std::shared_ptr<RoomTemplate> rtp) {
    // auto room = rtp->generate(region);
    // placeRoom(location, room);
  }

  void placeStartTemplateInRoom(std::shared_ptr<Region> region,
                                std::shared_ptr<RoomTemplate> rtp) {
    // auto room = rtp->generate(region);
    // placeStartInRoom(location, room);
  }

  void placeStatue(std::shared_ptr<Region> region) {
    // placeTemplateInRoom(location, RoomTemplates::STATUE_ROOM);
  }

  void placeAltar(std::shared_ptr<Region> region) {
    // placeTemplateInRoom(location, RoomTemplates::ALTAR_ROOM);
  }
*/
// std::shared_ptr<Region> placeLake(std::shared_ptr<Region> region) {
// return placeBlob(location, CellType::WATER);
// auto room = Room::makeBlob(location, 12, 3, 12, 3, CellType::WATER,
// CellType::UNKNOWN, true); placeInRoom(location, room); return room;
// }

std::shared_ptr<Location> Generator::getLocation() {

  dlog("gen start");
  using milliseconds = std::chrono::duration<double, std::milli>;
  auto data = entt::service_locator<GameData>::get().lock();

  auto t0 = std::chrono::system_clock::now();
  std::map<std::string, milliseconds> timings;
  std::chrono::time_point<std::chrono::system_clock> start;
  std::chrono::time_point<std::chrono::system_clock> end;

  location = std::make_shared<Location>();
  auto region = std::make_shared<Region>();
  region->resize(WIDTH, HEIGHT, CellType::UNKNOWN);
  location->place(region, 0, 0);
  location->getCells();

  if (bgThread.joinable()) {
    bgThread.join();
  }
  bgThread = std::thread([=]() {
    dlog("make exterior");
    makeExterior(region);
    dlog("make exterior finished");
    // location->printRegion();
    // auto _cells = location->getCells();
    // fmt::print("====\nlocation: {}x{}\n", _cells->front().size(),
    // _cells->size()); auto _cell = location->getCell(0,0,0); if (!_cell) {
    //   fmt::print("test cell: failed\n====\n");
    // } else {
    //   auto cell = *_cell;
    //   fmt::print("test cell: {}.{}.{}, {}\n====\n", cell->x, cell->y,
    //   cell->z, cell->type.name);
    // }
    // end = std::chrono::system_clock::now();
    // timings["gen ground"] = end - start;
    dlog("end");
  });

  return location;

  dlog("region created");
  // if (spec.type == RegionType::DUNGEON) {
  //   start = std::chrono::system_clock::now();
  //   placeRooms(region);
  //   end = std::chrono::system_clock::now();
  //   timings["placeRooms"] = end - start;

  //   start = std::chrono::system_clock::now();
  //   makePassages(region);
  //   end = std::chrono::system_clock::now();
  //   timings["makePassages"] = end - start;

  //   if (region->hasFeature(RegionFeature::CAVE_PASSAGE)) {
  //     start = std::chrono::system_clock::now();
  //     makeCavePassage(region);
  //     end = std::chrono::system_clock::now();
  //     timings["makeCavePassage"] = end - start;
  //   }
  // } else if (spec.type == RegionType::CAVERN) {
  //   start = std::chrono::system_clock::now();
  //   placeCaves(region);
  //   end = std::chrono::system_clock::now();
  //   timings["placeCaves"] = end - start;
  // } else if (spec.type == RegionType::EXTERIOR) {
  start = std::chrono::system_clock::now();
  // placeCaves(region);
  //
  // region->cells = mapUtils::fill(HEIGHT, WIDTH, CellType::UNKNOWN);
  // auto room = Room::makeBlob(location, 100, 90, 100, 90,
  // CellType::GROUND, CellType::UNKNOWN, true);
  // mapUtils::paste(room->cells, location, 0, 0);
  dlog("make exterior");
  makeExterior(region);
  dlog("make exterior finished");
  auto _cells = location->getCells();
  fmt::print("====\nlocation: {}x{}\n", _cells->front().size(), _cells->size());
  auto _cell = location->getCell(0, 0, 0);
  if (!_cell) {
    fmt::print("test cell: failed\n====\n");
  } else {
    auto cell = *_cell;
    fmt::print("test cell: {}.{}.{}, {}\n====\n", cell->x, cell->y, cell->z,
               cell->type.name);
  }
  end = std::chrono::system_clock::now();
  timings["gen ground"] = end - start;
  dlog("end");
  return location;
  // }

  /*
start = std::chrono::system_clock::now();
fixOverlapped(region);
end = std::chrono::system_clock::now();
timings["fixOverlapped"] = end - start;

if (region->hasFeature(RegionFeature::RIVER)) {
  start = std::chrono::system_clock::now();
  makeRiver(region);
  end = std::chrono::system_clock::now();
  timings["makeRiver"] = end - start;
}

// if (region->hasFeature(RegionFeature::VOID)) {
//   start = std::chrono::system_clock::now();
//   placeVoid(region);
//   end = std::chrono::system_clock::now();
//   timings["placeVoid"] = end - start;

//   start = std::chrono::system_clock::now();
//   fixOverlapped(region);
//   end = std::chrono::system_clock::now();
//   timings["fixOverlapped-void"] = end - start;
// }

if (region->hasFeature(RegionFeature::LAKE)) {
  start = std::chrono::system_clock::now();
  // placeLake(region);
  end = std::chrono::system_clock::now();
  timings["placeLake"] = end - start;
  start = std::chrono::system_clock::now();
  fixOverlapped(region);
  end = std::chrono::system_clock::now();
  timings["fixOverlapped-lake"] = end - start;
}

// if (region->hasFeature(RegionFeature::ALTAR)) {
//   start = std::chrono::system_clock::now();
//   placeAltar(region);
//   end = std::chrono::system_clock::now();
//   timings["placeAltar"] = end - start;
// }

// if (region->hasFeature(RegionFeature::BONES_FIELD)) {
//   placeTemplateInRoom(location, RoomTemplates::BONES_FIELD);
// }
// if (region->hasFeature(RegionFeature::ICE)) {
//   placeStartTemplateInRoom(location, RoomTemplates::ICE);
// }
// if (region->hasFeature(RegionFeature::CORRUPT)) {
//   placeStartTemplateInRoom(location, RoomTemplates::CORRUPT);
// }
// if (region->hasFeature(RegionFeature::HEAL)) {
//   placeTemplateInRoom(location, RoomTemplates::HEAL_STAND_ROOM);
// }
// if (region->hasFeature(RegionFeature::MANA)) {
//   placeTemplateInRoom(location, RoomTemplates::MANA_STAND_ROOM);
// }

start = std::chrono::system_clock::now();

// if (spec.type != RegionType::EXTERIOR) {
//   auto success = placeStairs(region);

//   if (!success) {
//     log.info(lu::green("MAPGEN"), "regen location");
//     dlog("regen location");
//     return getLocation(spec);
//   }
//   end = std::chrono::system_clock::now();
//   timings["placeStairs"] = end - start;

//   start = std::chrono::system_clock::now();
//   placeWalls(region);
//   end = std::chrono::system_clock::now();
//   timings["placeWalls"] = end - start;
// }

// if (spec.type == RegionType::DUNGEON) {
//   start = std::chrono::system_clock::now();
//   placeDoors(region);
//   end = std::chrono::system_clock::now();
//   timings["placeDoors"] = end - start;
// }

start = std::chrono::system_clock::now();
placeEnemies(location, spec.threat);
end = std::chrono::system_clock::now();
timings["placeEnemies"] = end - start;

start = std::chrono::system_clock::now();
placeLoot(location, spec.threat);
end = std::chrono::system_clock::now();
timings["placeLoot"] = end - start;

if (region->hasFeature(RegionFeature::TORCHES)) {
  start = std::chrono::system_clock::now();
  placeTorches(region);
  end = std::chrono::system_clock::now();
  timings["placeTorches"] = end - start;
}

if (region->hasFeature(RegionFeature::STATUE)) {
  start = std::chrono::system_clock::now();
  placeStatue(region);
  end = std::chrono::system_clock::now();
  timings["placeStatue"] = end - start;
}

// if (region->hasFeature(RegionFeature::TREASURE_SMALL)) {
//   placeTemplate(location, RoomTemplates::TREASURE_ROOM);
// }

start = std::chrono::system_clock::now();
auto _cells = region->getCells();
for (auto r : *_cells) {
  for (auto c : r) {
    if (c->passThrough && R::R() < data->probability["BLOOD"]) {
      c->features.push_back(CellFeature::BLOOD);
    } else if (c->room != nullptr && c->room->type == RegionType::HALL &&
               c->type == region->type.floor &&
               R::R() < data->probability["CRATE"]) {
      location->addTerrain("CRATE", c);
      //
crate->triggers.push_back(std::make_shared<AttackTrigger>([=](std::shared_ptr<Creature>
      // actor){
      //   return Triggers::CRATE_TRIGGER(actor, crate, location);
      // }));
      //
crate->triggers.push_back(std::make_shared<InteractTrigger>([=](std::shared_ptr<Creature>
      // actor){
      //   return Triggers::CRATE_MOVE_TRIGGER(actor, crate, location);
      // }));
    } else if (c->passThrough && R::R() < data->probability["BONES"]) {
      location->addItem("BONES", 1, c);
    }
    for (auto cf : region->type.cellFeatures) {
      c->features.push_back(cf);
    }
    // if (c->room != nullptr && (c->room->type == RegionType::CAVERN) &&
    //     c->passThrough && R::R() < data->probability["POND"]) {
    //   c->type = CellType::WATER;
    // }
  }
}
end = std::chrono::system_clock::now();
timings["cellFeatures"] = end - start;

std::vector<std::string> timeMarks;
for (auto [mark, ms] : timings) {
  timeMarks.push_back(fmt::format("{}: {}", mark, ms.count()));
}

location->place(region, 0, 0);

auto t1 = std::chrono::system_clock::now();
using milliseconds = std::chrono::duration<double, std::milli>;
milliseconds ms = t1 - t0;
return location;
*/
}
