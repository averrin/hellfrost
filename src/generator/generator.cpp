#include <chrono>
#include <fmt/format.h>
#include <iostream>
#include <set>

// #include "EventBus/EventBus.hpp"
// #include "lss/game/trigger.hpp"
#include "lss/game/content/enemies.hpp"
#include "lss/game/door.hpp"
// #include "lss/game/enemy.hpp"
#include "lss/game/item.hpp"
#include "lss/game/content/items.hpp"
// #include "lss/game/player.hpp"
#include "lss/game/terrain.hpp"
#include "lss/generator/generator.hpp"
#include "lss/generator/room.hpp"
#include "lss/generator/spawnTable.hpp"
#include "lss/generator/lootTable.hpp"
#include "lss/generator/mapUtils.hpp"
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

auto getRandomCell(std::shared_ptr<Location> location) {
  // auto room = location->rooms[rand() % location->rooms.size()];
  // return room->cells[rand() % room->cells.size()];
  return location->cells[rand() % HEIGHT][rand() % WIDTH];
}

std::optional<std::shared_ptr<Cell>>
getRandomCell(std::shared_ptr<Location> location, CellSpec type) {
  if (location->rooms.size() == 0) {
    return std::nullopt;
  }

  auto room = location->rooms[rand() % location->rooms.size()];
  auto rc = room->getRandomCell(type);
  auto n = location->rooms.size();
  while (!rc && n > 0) {
    room = location->rooms[rand() % location->rooms.size()];
    auto rc = room->getRandomCell(type);
    n--;
  }
  return rc;
}

void placeInRoom(std::shared_ptr<Location> location, std::shared_ptr<Room> room) {
  auto floor = location->type.floor;
  auto ltcr = getRandomCell(location, floor);
  while (!ltcr || (*ltcr)->x + room->width >= WIDTH ||
         (*ltcr)->y + room->height >= HEIGHT) {
    ltcr = getRandomCell(location, floor);
  }
  auto ltc = *ltcr;
  mapUtils::paste(room->cells, location, ltc->x, ltc->y);

  location->rooms.push_back(room);
}

void placeStartInRoom(std::shared_ptr<Location> location, std::shared_ptr<Room> room) {
  auto floor = location->type.floor;
  auto ltcr = *getRandomCell(location, floor);
  while (ltcr->x + room->width >= WIDTH ||
         ltcr->y + room->height >= HEIGHT) {
    auto _ltcr = getRandomCell(location, floor);
    if (_ltcr) ltcr = *_ltcr;
  }
  mapUtils::paste(room->cells, location, ltcr->x, ltcr->y);

  location->rooms.push_back(room);
}

void placeRoom(std::shared_ptr<Location> location, std::shared_ptr<Room> room) {
  auto ltcr = getRandomCell(location);
  while (ltcr->x + room->width >= WIDTH ||
         ltcr->y + room->height >= HEIGHT) {
    ltcr = getRandomCell(location);
  }
  mapUtils::paste(room->cells, location, ltcr->x, ltcr->y);

  location->rooms.push_back(room);
}

void placeWalls(std::shared_ptr<Location> location) {
  dlog("place walls");
  for (auto r : location->cells) {
    for (auto cell : r) {
      if (cell->type == CellType::UNKNOWN) {
        auto n = location->getNeighbors(cell);
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
  for (auto cell : location->cells.front()) {
    if (cell->type != CellType::UNKNOWN) {
      cell->type = CellType::WALL;
      cell->seeThrough = false;
      cell->passThrough = false;
    }
  }
  for (auto cell : location->cells.back()) {
    if (cell->type != CellType::UNKNOWN) {
      cell->type = CellType::WALL;
      cell->seeThrough = false;
      cell->passThrough = false;
    }
  }
}

std::shared_ptr<Room> makePassage(std::shared_ptr<Cell> start, Direction dir,
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
    //TODO: fix
    rh = length;
  }
  auto cells = mapUtils::fill(rh, rw, CellType::FLOOR);
  auto room = std::make_shared<Room>(RoomType::PASSAGE, cells);

  for (auto r : cells) {
    for (auto c : r) {
      c->room = room;
    }
  }
  return room;
}

void makePassageBetweenRooms(std::shared_ptr<Location> location,
                             std::shared_ptr<Room> room1,
                             std::shared_ptr<Room> room2) {
  auto sc = room1->cells[rand() % room1->cells.size()];
  auto ec = room2->cells[rand() % room2->cells.size()];
  while (sc == ec) {
    ec = room2->cells[rand() % room2->cells.size()];
  }

  if (std::find(room1->features.begin(), room1->features.end(),
                RoomFeature::DUNGEON) == room1->features.end())
    room1->features.push_back(RoomFeature::DUNGEON);
  if (std::find(room2->features.begin(), room2->features.end(),
                RoomFeature::DUNGEON) == room2->features.end())
    room2->features.push_back(RoomFeature::DUNGEON);

  if (sc->y > ec->y) {
    std::swap(sc, ec);
  }

  auto passage = makePassage(sc, S, ec->y - sc->y + 1);
  mapUtils::paste(passage->cells, location, sc->x, sc->y);
  passage->x = sc->x;
  passage->y = sc->y;
  passage->features.push_back(RoomFeature::DUNGEON);
  location->rooms.push_back(passage);
  sc = passage->cells.back();
  if (sc != ec) {
    if (sc->x > ec->x) {
      std::swap(sc, ec);
    }
    passage = makePassage(sc, E, ec->x - sc->x + 1);
    mapUtils::paste(passage->cells, location, sc->x, sc->y);
    passage->x = sc->x;
    passage->y = sc->y;
    passage->features.push_back(RoomFeature::DUNGEON);
    location->rooms.push_back(passage);
  }
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
        dCells[oy][ox] = std::make_shared<Cell>(ox, oy, type, f);
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
        dCells[oy][ox] = std::make_shared<Cell>(ox, oy, type, f);
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
        dCells[oy][ox] = std::make_shared<Cell>(ox, oy, type, f);
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
        dCells[oy][ox] = std::make_shared<Cell>(ox, oy, type, f);
      }
      nx += 1;
      ny += offset * w;
      break;
    }
    if (ny < 0 || nx < 0 || ny > HEIGHT - 1 || nx > WIDTH - 1) {
      break;
    };
    cell = dCells[ny][nx];
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

//TODO: better room merge
void fixOverlapped(std::shared_ptr<Location> location) {
  dlog("fix overlapped");
  std::map<std::shared_ptr<Room>, std::vector<std::shared_ptr<Cell>>> cache;
  for (auto r : location->rooms) {
    cache[r] = {};
  }
  for (auto r = 0; r < HEIGHT; r++) {
    for (auto c = 0; c < WIDTH; c++) {
      if (location->cells[r][c]->type == CellType::UNKNOWN) {
        continue;
      }
      std::vector<std::shared_ptr<Room>> rooms(location->rooms.size());
      auto it = std::copy_if(location->rooms.begin(), location->rooms.end(),
                             rooms.begin(), [&](auto room) {
                               auto result =
                                   room->x <= c && room->x + room->width > c &&
                                   room->y <= r && room->y + room->height > r;
                               return result;
                             });
      rooms.resize(std::distance(rooms.begin(), it));
      if (rooms.size() == 1) {
        cache[rooms.front()].push_back(location->cells[r][c]);
        location->cells[r][c]->room = rooms.front();
      } else if (rooms.size() != 0) {

        std::vector<std::shared_ptr<Room>> halls(rooms.size());
        auto it =
            std::copy_if(rooms.begin(), rooms.end(), halls.begin(),
                         [](auto r) { return r->type != RoomType::PASSAGE; });
        halls.resize(std::distance(halls.begin(), it));
        if (halls.size() == 0) {
          cache[rooms.front()].push_back(location->cells[r][c]);
          location->cells[r][c]->room = rooms.front();
        } else if (halls.size() == 1) {
          cache[halls.front()].push_back(location->cells[r][c]);
          location->cells[r][c]->room = halls.front();
        } else {
          // TODO: select hall by neighbors
          cache[halls.front()].push_back(location->cells[r][c]);
          location->cells[r][c]->room = halls.front();
        }
      }
    }
  }
  auto rooms = location->rooms;
  for (auto room : rooms) {
    // FIXME: remove orphaned objects
    room->cells = cache[room];
    if (room->cells.size() == 0) {
      location->rooms.erase(
          std::remove(location->rooms.begin(), location->rooms.end(), room));
    }
  }
}

void Generator::placeDoors(std::shared_ptr<Location> location) {
  auto data = entt::service_locator<GameData>::get().lock();
  dlog("place doors");
  auto d = 0;
  for (auto r : location->cells) {
    for (auto c : r) {
      if (c->type == CellType::FLOOR) {
        if ((location->cells[c->y - 1][c->x]->type == CellType::WALL &&
             location->cells[c->y + 1][c->x]->type == CellType::WALL) ||
            (location->cells[c->y][c->x - 1]->type == CellType::WALL &&
             location->cells[c->y][c->x + 1]->type == CellType::WALL)) {
          auto n = location->getNeighbors(c);
          if (std::count_if(n.begin(), n.end(), [&](std::shared_ptr<Cell> nc) {
                return nc->type == CellType::WALL;
              }) < 6) {
            auto o = location->getObjects(c);
            if (R::R() > data->probability["DOOR"] || c->hasFeature(CellFeature::CAVE) ||
                o.size() > 0)
              continue;
            auto nd = false;
            for (auto nc : n) {
              auto no = location->getObjects(nc);
              if (std::find_if(no.begin(), no.end(),
                               [](std::shared_ptr<Object> ob) {
                                 return std::dynamic_pointer_cast<Door>(ob);
                               }) != no.end()) {
                nd = true;
                break;
              }
            }
            if (nd)
              continue;
            auto door = std::make_shared<Door>();
            if (R::R() < data->probability["SECRET_DOOR"]) {
              door->hidden = true;
            }
            door->setCurrentCell(c);
            c->seeThrough = false;
            location->addObject<Door>(door);
            d++;
          }
        }
      }
    }
  }
}

void placeLoot(std::shared_ptr<Location> location, int threat) {
  dlog("place loot");
  auto floor = location->type.floor;
  auto table = LootBoxes::ZERO;
  if (location->type.type == int(LocationType::DUNGEON)) {
    if (threat >= LootTable::DUNGEON.size())
      threat = LootTable::DUNGEON.size() - 1;
    table = LootTable::DUNGEON.at(threat);
  } else if (location->type.type == LocationType::CAVERN) {
    // TODO: use cavern table
    if (threat >= int(LootTable::DUNGEON.size()))
      threat = LootTable::DUNGEON.size() - 1;
    table = LootTable::DUNGEON.at(threat);
  }

  // TODO: gen count and get loot from 100% lootbox
  for (auto room : location->rooms) {
    auto rc = room->getRandomCell(floor);
    std::shared_ptr<Cell> c;
    if (rc)
      c = *rc;
    else
      continue;
    auto n = 0;
    // TODO: fix crash in getObjects
    while (location->getObjects(c).size() != 0 && n < 20) {
      rc = room->getRandomCell(floor);
      if (rc)
        c = *rc;
      else
        continue;
      n++;
    }
    if (n == 20) {
      continue;
    }

    auto loot = table.open();
    for (auto item : loot) {
      auto new_item = item->roll();
      // fmt::print("Place {} at {}.{}\n", new_item->getTitle(true), c->x,
      // c->y);
      new_item->setCurrentCell(c);
      location->addObject<Item>(new_item);
    }
  }
}

void placeEnemies(std::shared_ptr<Location> location, int threat) {
  dlog("place enemies");
  std::map<EnemySpec, float> table;
  auto pather = new micropather::MicroPather(location.get());
  micropather::MPVector<void *> path;
  float totalCost = 0;

  if (location->type.type == LocationType::DUNGEON) {
    if (threat >= int(SpawnTable::DUNGEON.size()))
      threat = SpawnTable::DUNGEON.size() - 1;
    table = SpawnTable::DUNGEON.at(threat);
  } else if (location->type.type == LocationType::CAVERN) {
    // TODO: use cavern table
    if (threat >= int(SpawnTable::DUNGEON.size()))
      threat = SpawnTable::DUNGEON.size() - 1;
    table = SpawnTable::DUNGEON.at(threat);
  }
  for (auto room : location->rooms) {
    if (room->type == RoomType::PASSAGE)
      continue;
    std::vector<std::shared_ptr<Cell>> cells(room->cells.size());
    auto it =
        std::copy_if(room->cells.begin(), room->cells.end(), cells.begin(),
                     [&location](auto cell) { return cell->type == location->type.floor; });
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

      pather->Reset();
      int result =
          pather->Solve(c.get(), location->enterCell.get(), &path, &totalCost);
      if (result != micropather::MicroPather::SOLVED)
        break;

      auto enemy = mapUtils::makeEnemy(location, c, type);
      location->addObject<Enemy>(enemy);
    }
  }
}

void makeRiver(std::shared_ptr<Location> location) {
  dlog("place river");
  auto river =
      dig(location->cells[0][location->cells.front().size() / 3 +
                             rand() % location->cells.front().size() / 3],
          S, location->cells.size() - 2, 2, 2, 1, CellType::WATER);
  mapUtils::paste(river.second, location, 0, 0);
}

// TODO: respect void on prev location
// std::shared_ptr<Location>
// Generator::getRandomLocation(std::shared_ptr<Player> hero, int depth,
//                              std::shared_ptr<Cell> enter) {
//   if (depth == -1 && hero->currentLocation == nullptr) {
//     depth = 0;
//   }
//   if (depth == -1) {
//     depth = hero->currentLocation->depth + 1;
//   }
//   auto spec = LocationSpec{"Dungeon"};
//   if (R::R() < data->probability["CAVERN"]&& depth >= 3) {
//     spec.type = LocationType::CAVERN;
//     spec.cellFeatures.push_back(CellFeature::CAVE);
//     spec.name = "Cavern";
//   }
//   if (spec.type == LocationType::DUNGEON && R::R() < data->probability["CAVE_PASSAGE"]) {
//     spec.features.push_back(LocationFeature::CAVE_PASSAGE);
//   }
//   if (spec.type == LocationType::CAVERN && R::R() < data->probability["RIVER"]) {
//     spec.features.push_back(LocationFeature::RIVER);
//   }
//   if (R::R() < data->probability["TORCHES"]) {
//     spec.features.push_back(LocationFeature::TORCHES);
//   }
//   if (R::R() < data->probability["STATUE"]) {
//     spec.features.push_back(LocationFeature::STATUE);
//   }
//   if (spec.type == LocationType::DUNGEON && R::R() < data->probability["ALTAR"]) {
//     spec.features.push_back(LocationFeature::ALTAR);
//   }
//   if (spec.type == LocationType::DUNGEON && R::R() < pdata->robabilities["TREASURE_SMALL"]) {
//     spec.features.push_back(LocationFeature::TREASURE_SMALL);
//   }
//   if (R::R() < data->probability["VOID"]) {
//     spec.features.push_back(LocationFeature::VOID);
//   }
//   if (R::R() < data->probability["ICE"]) {
//     spec.features.push_back(LocationFeature::ICE);
//   }
//   if (R::R() < data->probability["BONES_FIELD"]) {
//     spec.features.push_back(LocationFeature::BONES_FIELD);
//   }
//   if (R::R() < data->probability["CORRUPT"]) {
//     spec.features.push_back(LocationFeature::CORRUPT);
//   }
//   if (R::R() < data->probability["HEAL"]) {
//     spec.features.push_back(LocationFeature::HEAL);
//   }
//   if (R::R() < data->probability["MANA"]) {
//     spec.features.push_back(LocationFeature::MANA);
//   }
//   if (spec.type == LocationType::CAVERN && R::R() < data->probability["LAKE"]) {
//     spec.features.push_back(LocationFeature::LAKE);
//   }
//   spec.threat = depth;
//   spec.enterCell = enter;
//   auto l = getLocation(spec);
//   l->depth = depth;
//   return l;
// }

// std::shared_ptr<Terrain> makeTorch() {
//     auto data = entt::service_locator<GameData>::get().lock();
//     auto torch = std::make_shared<Terrain>(data->terrainSpecs["TORCH_STAND"]);
//     // torch->triggers.push_back(std::make_shared<PickTrigger>([=](std::shared_ptr<Creature> actor){
//     //   auto hero = std::dynamic_pointer_cast<Player>(actor);
//     //   return Triggers::TORCH_STAND_TRIGGER(hero, torch);
//     // }));
//     return torch;
// }

void placeTorches(std::shared_ptr<Location> location) {
  dlog("place torches");
  auto floor = location->type.floor;
  auto tc = rand() % 7 + 3;
  int n;
  for (n = 0; n < tc;) {
    auto room = location->rooms[rand() % location->rooms.size()];
    auto cell = room->cells[rand() % room->cells.size()];
    if (cell->type != floor || location->getObjects(cell).size() > 0)
      continue;
    auto ngs = location->getNeighbors(cell);
    if (std::find_if(ngs.begin(), ngs.end(), [](std::shared_ptr<Cell> nc) {
          return nc->type == CellType::WALL;
        }) == ngs.end())
      continue;
    location->addTerrain("TORCH_STAND", cell);
    n++;
  }
  auto nbrs = location->getNeighbors(location->exitCell);
  location->addTerrain("TORCH_STAND", nbrs[rand() % nbrs.size()]);

  nbrs = location->getNeighbors(location->enterCell);
  location->addTerrain("TORCH_STAND", nbrs[rand() % nbrs.size()]);

  // fmt::print("Torches: {}\n", n);
}

bool placeStairs(std::shared_ptr<Location> location) {
  dlog("place stairs");

  auto floor = location->type.floor;
  auto entr = getRandomCell(location, floor);
  if (location->type.enterCell != nullptr) {
    auto e = location->type.enterCell;
    entr =
        std::make_optional<std::shared_ptr<Cell>>(location->cells[e->y][e->x]);
  }
  dlog("entr found 1");
  auto exir = getRandomCell(location, floor);
  dlog("exir found 1");
  while (!entr) {
    entr = getRandomCell(location, floor);
  }
  dlog("entr found");
  while (!exir) {
    exir = getRandomCell(location, floor);
  }
  dlog("exir found");
  location->enterCell = *entr;
  location->exitCell = *exir;
  // location->dump();

  auto pather = new micropather::MicroPather(location.get());
  micropather::MPVector<void *> path;
  float totalCost = 0;
  pather->Reset();
  int result = pather->Solve(location->enterCell.get(),
                             location->exitCell.get(), &path, &totalCost);

  auto i = 0;
  while (result != micropather::MicroPather::SOLVED ||
         (totalCost < 20 && i < 10)) {
    // entr = mapUtils::getRandomCell(location, CellType::FLOOR);
    exir = getRandomCell(location, floor);
    // while (!entr) {
    // entr = mapUtils::getRandomCell(location, CellType::FLOOR);
    // }
    while (!exir) {
      exir = getRandomCell(location, floor);
    }
    location->enterCell = *entr;
    location->exitCell = *exir;
    pather->Reset();
    result = pather->Solve(location->enterCell.get(), location->exitCell.get(),
                           &path, &totalCost);
    i++;
    if (i == 30) {
      location->log.warn(lu::green("MAPGEN"), "cannot place exit");
      delete pather;
      return false;
    }
  }
  // fmt::print("{}.{} -> {}.{} = {}\n", location->enterCell->x,
  //            location->enterCell->y, location->exitCell->x,
  //            location->exitCell->y, totalCost);

  delete pather;

  auto _objects = location->objects;
  for (auto o: _objects) {
      if (o->currentCell != nullptr && (o->currentCell == location->exitCell ||
          o->currentCell == location->enterCell)) {
        location->removeObject(o);
      }
  }

  location->exitCell->type = CellType::DOWNSTAIRS;
  auto n = location->getNeighbors(location->exitCell);
  // FIXME: crash
  std::for_each(n.begin(), n.end(), [location](auto c) {
    mapUtils::makeFloor(c);
    c->room = location->exitCell->room;
    if (c->room) {
      c->room->cells.push_back(c);
    }
  });
  location->enterCell->type = CellType::UPSTAIRS;
  n = location->getNeighbors(location->enterCell);
  std::for_each(n.begin(), n.end(), [location](auto c) {
    mapUtils::makeFloor(c);
    c->room = location->enterCell->room;
    if (c->room) {
      c->room->cells.push_back(c);
    }
  });
  return true;
}

void makePassages(std::shared_ptr<Location> location) {
  dlog("place passages");
  auto pc = rand() % 10;
  auto n = 0;
  std::set<std::shared_ptr<Room>> rooms;
  std::vector<std::shared_ptr<Room>> halls = location->rooms;

  while (n < pc) {
    auto room1 = location->rooms[rand() % location->rooms.size()];
    auto room2 = location->rooms[rand() % location->rooms.size()];
    if (room1 == room2)
      continue;
    if (std::find(halls.begin(), halls.end(), room1) != halls.end())
      rooms.insert(room1);
    if (std::find(halls.begin(), halls.end(), room2) != halls.end())
      rooms.insert(room2);
    n++;
    makePassageBetweenRooms(location, room1, room2);
  }
  for (auto r : halls) {
    if (std::find(rooms.begin(), rooms.end(), r) == rooms.end()) {
      auto room2 = location->rooms[rand() % location->rooms.size()];
      while (room2 == r) {
        room2 = location->rooms[rand() % location->rooms.size()];
      }
      makePassageBetweenRooms(location, r, room2);
    }
    for (auto c : r->cells) {
      c->room = r;
    }
  }
}

void Generator::makeExterior(std::shared_ptr<Location> location) {
  auto data = entt::service_locator<GameData>::get().lock();
  dlog("place caves");
  auto rc = rand() % 20 + 35;

  location->cells = mapUtils::fill(HEIGHT*2, WIDTH*2, CellType::UNKNOWN);
  for (auto n = 0; n < rc; n++) {
    auto room = Room::makeRoom(34, 12, 34, 12, CellType::WALL);

    for (auto c : room->cells) {
      if (R::R() > 0.45) {
        c->type = CellType::GROUND;
        c->passThrough = true;
      }
    }
    auto ry = rand() % (location->cells.size() - room->height - 8) + 8;
    auto rx = rand() % (location->cells.front().size() - room->width - 8) + 8;
    mapUtils::paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    room->threat = rand() % 4;
    room->features.push_back(RoomFeature::CAVE);
    location->rooms.push_back(room);

    for (auto c : room->cells) {
      c->room = room;
    }
  }
  fixOverlapped(location);

makePassages(location);

  for (auto n = 0; n < rand() % 4 + 2; n++) {
    auto _cells = location->cells;
    std::shuffle(_cells.begin(), _cells.end(), std::default_random_engine(rand()));
    for (auto r : _cells) {
      std::shuffle(r.begin(), r.end(), std::default_random_engine(rand()));
      for (auto c : r) {
        if (c->type == CellType::UNKNOWN) {
          auto n = location->getNeighbors(c);
          auto fn =
              std::count_if(n.begin(), n.end(), [=](std::shared_ptr<Cell> nc) {
                return nc->type != CellType::UNKNOWN;
              });
          if ((fn >= 4 && rand() % 10 < 8) || fn == 3) {
            // c->type = CellType::GROUND;
            mapUtils::updateCell(location->cells[c->y][c->x], CellType::GROUND, c->features);
          }
        } else {
          auto n = location->getNeighbors(c);
          auto fn =
              std::count_if(n.begin(), n.end(), [=](std::shared_ptr<Cell> nc) {
                return nc->type == CellType::UNKNOWN;
              });
              if (fn >= 4) {
                c->type = CellType::UNKNOWN;
                // mapUtils::updateCell(location->cells[y][x], CellType::UNKNOWN, c->features);
              }

        }
      }
    }
  }

  for (auto r : location->cells) {
    for (auto c : r) {
      if (c->type != CellType::UNKNOWN) {
        mapUtils::updateCell(location->cells[c->y][c->x], CellType::GROUND, c->features);
        if (R::R() < data->probability["EXT_BUSH"]) {
          location->addTerrain("BUSH", c);
          // s->triggers.push_back(std::make_shared<EnterTrigger>([=](std::shared_ptr<Creature> actor){
          //   return Triggers::BUSH_TRIGGER(c, location);
          // }));
        } else if (R::R() < data->probability["EXT_TREE"]) {
          location->addTerrain("TREE", c);
        } else if (R::R() < data->probability["EXT_GRASS"]) {
          auto grass = Prototype::GRASS->clone();
          grass->setCurrentCell(c);
          location->addObject<Item>(grass);
          // location->addTerrain("GRASS", c);
          // auto grass = Prototype::GRASS->clone();
        }
      }
    }
  }

}

void placeRooms(std::shared_ptr<Location> location) {
  dlog("place rooms");
  auto rc = rand() % 12 + 7;

  location->cells = mapUtils::fill(HEIGHT, WIDTH, CellType::UNKNOWN);
  // TODO: ensure place in location borders
  if (location->type.enterCell != nullptr) {
    auto room = Room::makeRoom(5, 3, 5, 3);
    auto e = location->type.enterCell;
    auto rx = e->x - room->width / 2;
    auto ry = e->y - room->height / 2;
    mapUtils::paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    room->threat = rand() % 4;
    location->rooms.push_back(room);
  }

  for (auto n = 0; n < rc; n++) {
    auto room = Room::makeRoom();
    auto ry = rand() % (location->cells.size() - room->height - 2) + 2;
    auto rx = rand() % (location->cells.front().size() - room->width - 2) + 2;
    mapUtils::paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    room->threat = rand() % 4;
    location->rooms.push_back(room);
  }
}

void Generator::placeCaves(std::shared_ptr<Location> location) {
  auto data = entt::service_locator<GameData>::get().lock();
  dlog("place caves");
  auto rc = rand() % 12 + 7;

  location->cells = mapUtils::fill(HEIGHT, WIDTH, CellType::UNKNOWN);
  for (auto n = 0; n < rc; n++) {
    auto room = Room::makeRoom(44, 12, 44, 12, CellType::WALL);

    for (auto c : room->cells) {
      if (R::R() > data->probability["CAVERN_WALL"]) {
        c->type = CellType::GROUND;
        c->passThrough = true;
      }
    }
    auto ry = rand() % (location->cells.size() - room->height - 2) + 2;
    auto rx = rand() % (location->cells.front().size() - room->width - 2) + 2;
    mapUtils::paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    room->threat = rand() % 4;
    room->features.push_back(RoomFeature::CAVE);
    location->rooms.push_back(room);

    for (auto c : room->cells) {
      c->room = room;
    }
  }
  fixOverlapped(location);

  for (auto i = 0; i < 5; i++) {
    for (auto room : location->rooms) {
      room->type = RoomType::CAVERN;
      for (auto c : room->cells) {
        auto n = location->getNeighbors(c);
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

  for (auto room : location->rooms) {
    for (auto c : room->cells) {
      if (c->type == CellType::UNKNOWN)
        continue;
      auto n = location->getNeighbors(c);
      auto fn = std::count_if(n.begin(), n.end(), [](std::shared_ptr<Cell> nc) {
        return nc->type == CellType::GROUND;
      });
      if (fn == 0) {
        c->type = CellType::UNKNOWN;
      } else if (fn != 8) {
        if (c->type == CellType::GROUND && R::R() < data->probability["CAVE_ROCK"]) {
          auto rock = Prototype::ROCK->clone();
          rock->setCurrentCell(c);
          location->addObject<Item>(rock);
          continue;
        }
      } else if (c->type == CellType::GROUND && R::R() < data->probability["CAVE_GRASS"]) {
        if (R::R() < data->probability["CAVE_BUSH"]) {/*||
            std::find_if(n.begin(), n.end(), [&](auto nc) {
              auto t = utils::castObjects<Terrain>(location->getObjects(nc));
              return t.size() == 1 && t.front()->type == data->terrainSpecs["BUSH"];
            }) != n.end()*/
          location->addTerrain("BUSH", c);
          // s->triggers.push_back(std::make_shared<EnterTrigger>([=](std::shared_ptr<Creature> actor){
          //   return Triggers::BUSH_TRIGGER(c, location);
          // }));
        } else {
          auto grass = Prototype::GRASS->clone();
          grass->setCurrentCell(c);
          location->addObject<Item>(grass);
        }
        continue;
      }
    }
  }
}

void Generator::makeCavePassage(std::shared_ptr<Location> location) {
  auto data = entt::service_locator<GameData>::get().lock();
  dlog("place cave passage");
  std::vector<Direction> ds{N, E, S, W};
  auto room = location->rooms[rand() % location->rooms.size()];
  auto cell = room->cells[rand() % room->cells.size()];
  for (auto n = 0; n < rand() % 6 + 2; n++) {
    auto res = randomDig(cell, ds[rand() % ds.size()], rand() % 30);
    cell = res.first;
    auto newRoom = std::make_shared<Room>(RoomType::CAVE, res.second);
    room->features.push_back(RoomFeature::CAVE);
    mapUtils::paste(newRoom->cells, location, 0, 0);
    newRoom->x = 0;
    newRoom->y = 0;
    newRoom->threat = rand() % 4;
    location->rooms.push_back(newRoom);
    for (auto c : newRoom->cells) {
      if (c->type == CellType::GROUND && R::R() < data->probability["CAVE_ROCK"]) {
        auto rock = Prototype::ROCK->clone();
        rock->setCurrentCell(c);
        location->addObject<Item>(rock);
      } else if (c->type == CellType::GROUND && R::R() < data->probability["CAVE_GRASS"]) {
        if (R::R() < data->probability["CAVE_BUSH"]) {
          location->addTerrain("BUSH", c);
        } else {
          auto grass = Prototype::GRASS->clone();
          grass->setCurrentCell(c);
          location->addObject<Item>(grass);
        }
        continue;
      }
    }
  }
}

void placeTemplateInRoom(std::shared_ptr<Location> location, std::shared_ptr<RoomTemplate> rtp) {
  auto room = rtp->generate(location);
  // Room::printRoom(location, room);

  //TODO: shuffle suitable rooms and select
  auto target = location->rooms[rand() % location->rooms.size()];
  auto n = 0;
  while (target->width - 2 < room->width || target->height -2 < room->height || target->x == 0) {
    target = location->rooms[rand() % location->rooms.size()];
    if (n > 30) {
      location->log.warn(lu::green("MAPGEN"), "cannot place template");
      return;
    }
    n++;
  }

  mapUtils::paste(room->cells, location, target->x+1, target->y+1);

  location->rooms.push_back(room);
}

void placeTemplate(std::shared_ptr<Location> location, std::shared_ptr<RoomTemplate> rtp) {
  auto room = rtp->generate(location);
  placeRoom(location, room);
}

void placeStartTemplateInRoom(std::shared_ptr<Location> location, std::shared_ptr<RoomTemplate> rtp) {
  auto room = rtp->generate(location);
  placeStartInRoom(location, room);
}

void placeStatue(std::shared_ptr<Location> location) {
  placeTemplateInRoom(location, RoomTemplates::STATUE_ROOM);
}

void placeAltar(std::shared_ptr<Location> location) {
  placeTemplateInRoom(location, RoomTemplates::ALTAR_ROOM);
}

std::shared_ptr<Room> placeVoid(std::shared_ptr<Location> location) {
  // return placeBlob(location, CellType::VOID);
  auto room = Room::makeBlob(location, 12, 3, 12, 3, CellType::VOID, CellType::UNKNOWN, true);
  placeInRoom(location, room);
  return room;
}

std::shared_ptr<Room> placeLake(std::shared_ptr<Location> location) {
  // return placeBlob(location, CellType::WATER);
  auto room = Room::makeBlob(location, 12, 3, 12, 3, CellType::WATER, CellType::UNKNOWN, true);
  placeInRoom(location, room);
  return room;
}

std::shared_ptr<Location> Generator::getLocation(LocationSpec spec) {
  using milliseconds = std::chrono::duration<double, std::milli>;
  auto data = entt::service_locator<GameData>::get().lock();

  auto t0 = std::chrono::system_clock::now();
  std::map<std::string, milliseconds> timings;
  std::chrono::time_point<std::chrono::system_clock> start;
  std::chrono::time_point<std::chrono::system_clock> end;

  auto location = std::make_shared<Location>(spec);
  location->depth = spec.threat;

  if (spec.type == LocationType::DUNGEON) {
    start = std::chrono::system_clock::now();
    placeRooms(location);
    end = std::chrono::system_clock::now();
    timings["placeRooms"] = end - start;

    start = std::chrono::system_clock::now();
    makePassages(location);
    end = std::chrono::system_clock::now();
    timings["makePassages"] = end - start;

    if (location->hasFeature(LocationFeature::CAVE_PASSAGE)) {
      start = std::chrono::system_clock::now();
      makeCavePassage(location);
      end = std::chrono::system_clock::now();
      timings["makeCavePassage"] = end - start;
    }
  } else if (spec.type == LocationType::CAVERN) {
    start = std::chrono::system_clock::now();
    placeCaves(location);
    end = std::chrono::system_clock::now();
    timings["placeCaves"] = end - start;
  } else if (spec.type == LocationType::EXTERIOR) {
    start = std::chrono::system_clock::now();
    // placeCaves(location);
    //
    // location->cells = mapUtils::fill(HEIGHT, WIDTH, CellType::UNKNOWN);
    // auto room = Room::makeBlob(location, 100, 90, 100, 90, CellType::GROUND, CellType::UNKNOWN, true);
    // mapUtils::paste(room->cells, location, 0, 0);
    makeExterior(location);
    end = std::chrono::system_clock::now();
    timings["gen ground"] = end - start;
    return location;
  }

  start = std::chrono::system_clock::now();
  fixOverlapped(location);
  end = std::chrono::system_clock::now();
  timings["fixOverlapped"] = end - start;

  if (location->hasFeature(LocationFeature::RIVER)) {
    start = std::chrono::system_clock::now();
    makeRiver(location);
    end = std::chrono::system_clock::now();
    timings["makeRiver"] = end - start;
  }

  if (location->hasFeature(LocationFeature::VOID)) {
    start = std::chrono::system_clock::now();
    placeVoid(location);
    end = std::chrono::system_clock::now();
    timings["placeVoid"] = end - start;

    start = std::chrono::system_clock::now();
    fixOverlapped(location);
    end = std::chrono::system_clock::now();
    timings["fixOverlapped-void"] = end - start;
  }

  if (location->hasFeature(LocationFeature::LAKE)) {
    start = std::chrono::system_clock::now();
    placeLake(location);
    end = std::chrono::system_clock::now();
    timings["placeLake"] = end - start;
    start = std::chrono::system_clock::now();
    fixOverlapped(location);
    end = std::chrono::system_clock::now();
    timings["fixOverlapped-lake"] = end - start;
  }

  if (location->hasFeature(LocationFeature::ALTAR)) {
    start = std::chrono::system_clock::now();
    placeAltar(location);
    end = std::chrono::system_clock::now();
    timings["placeAltar"] = end - start;
  }

  if (location->hasFeature(LocationFeature::BONES_FIELD)) {
    placeTemplateInRoom(location, RoomTemplates::BONES_FIELD);
  }
  if (location->hasFeature(LocationFeature::ICE)) {
    placeStartTemplateInRoom(location, RoomTemplates::ICE);
  }
  if (location->hasFeature(LocationFeature::CORRUPT)) {
    placeStartTemplateInRoom(location, RoomTemplates::CORRUPT);
  }
  if (location->hasFeature(LocationFeature::HEAL)) {
    placeTemplateInRoom(location, RoomTemplates::HEAL_STAND_ROOM);
  }
  if (location->hasFeature(LocationFeature::MANA)) {
    placeTemplateInRoom(location, RoomTemplates::MANA_STAND_ROOM);
  }

  start = std::chrono::system_clock::now();

  if (spec.type != LocationType::EXTERIOR) {
    auto success = placeStairs(location);

    if (!success) {
      log.info(lu::green("MAPGEN"), "regen location");
      dlog("regen location");
      return getLocation(spec);
    }
    end = std::chrono::system_clock::now();
    timings["placeStairs"] = end - start;

    start = std::chrono::system_clock::now();
    placeWalls(location);
    end = std::chrono::system_clock::now();
    timings["placeWalls"] = end - start;
  }

  if (spec.type == LocationType::DUNGEON) {
    start = std::chrono::system_clock::now();
    placeDoors(location);
    end = std::chrono::system_clock::now();
    timings["placeDoors"] = end - start;
  }

  start = std::chrono::system_clock::now();
  placeEnemies(location, spec.threat);
  end = std::chrono::system_clock::now();
  timings["placeEnemies"] = end - start;

  start = std::chrono::system_clock::now();
  placeLoot(location, spec.threat);
  end = std::chrono::system_clock::now();
  timings["placeLoot"] = end - start;

  if (location->hasFeature(LocationFeature::TORCHES)) {
    start = std::chrono::system_clock::now();
    placeTorches(location);
    end = std::chrono::system_clock::now();
    timings["placeTorches"] = end - start;
  }

  if (location->hasFeature(LocationFeature::STATUE)) {
    start = std::chrono::system_clock::now();
    placeStatue(location);
    end = std::chrono::system_clock::now();
    timings["placeStatue"] = end - start;
  }

  if (location->hasFeature(LocationFeature::TREASURE_SMALL)) {
    placeTemplate(location, RoomTemplates::TREASURE_ROOM);
  }

  start = std::chrono::system_clock::now();
  for (auto r : location->cells) {
    for (auto c : r) {
      if (c->passThrough && R::R() < data->probability["BLOOD"]) {
        c->features.push_back(CellFeature::BLOOD);
      } else if (c->room != nullptr && c->room->type == RoomType::HALL && c->type == location->type.floor && R::R() < data->probability["CRATE"]) {
        location->addTerrain("CRATE", c);
        // crate->triggers.push_back(std::make_shared<AttackTrigger>([=](std::shared_ptr<Creature> actor){
        //   return Triggers::CRATE_TRIGGER(actor, crate, location);
        // }));
        // crate->triggers.push_back(std::make_shared<InteractTrigger>([=](std::shared_ptr<Creature> actor){
        //   return Triggers::CRATE_MOVE_TRIGGER(actor, crate, location);
        // }));
      } else if (c->passThrough && R::R() < data->probability["BONES"]) {
        auto bones = std::make_shared<Item>("BONES", 1);
        bones->setCurrentCell(c);
        location->addObject<Item>(bones);
      }
      for (auto cf : location->type.cellFeatures) {
        c->features.push_back(cf);
      }
      if (c->room != nullptr && (c->room->type == RoomType::CAVERN) &&
          c->passThrough && R::R() < data->probability["POND"]) {
        c->type = CellType::WATER;
      }
    }
  }
  end = std::chrono::system_clock::now();
  timings["cellFeatures"] = end - start;

  std::vector<std::string> timeMarks;
  for (auto [mark, ms] : timings) {
    timeMarks.push_back(fmt::format("{}: {}", mark, ms.count()));
  }

  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  return location;
}
