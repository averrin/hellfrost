#include <chrono>
#include <fmt/format.h>
#include <iostream>
#include <set>

// #include "EventBus/EventBus.hpp"
// #include "lss/game/trigger.hpp"
#include "lss/game/content/enemies.hpp"
#include "lss/game/door.hpp"
// #include "lss/game/enemy.hpp"
#include "lss/game/content/items.hpp"
#include "lss/game/item.hpp"
// #include "lss/game/player.hpp"
#include "lss/game/terrain.hpp"
#include "lss/generator/generator.hpp"
#include "lss/generator/lootTable.hpp"
#include "lss/generator/mapUtils.hpp"
#include "lss/generator/room.hpp"
#include "lss/generator/spawnTable.hpp"
#include "lss/utils.hpp"
// #include <liblog/liblog.hpp>

#ifndef GEN_DEBUG
#define GEN_DEBUG true
#endif

void dlog(std::string msg) {
  if (GEN_DEBUG)
    fmt::print("GENERATOR: {}\n", msg);
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

void placeInRoom(std::shared_ptr<Location> location,
                 std::shared_ptr<Room> room) {
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

void placeStartInRoom(std::shared_ptr<Location> location,
                      std::shared_ptr<Room> room) {
  auto floor = location->type.floor;
  auto ltcr = *getRandomCell(location, floor);
  while (ltcr->x + room->width >= WIDTH || ltcr->y + room->height >= HEIGHT) {
    auto _ltcr = getRandomCell(location, floor);
    if (_ltcr)
      ltcr = *_ltcr;
  }
  mapUtils::paste(room->cells, location, ltcr->x, ltcr->y);

  location->rooms.push_back(room);
}

void placeRoom(std::shared_ptr<Location> location, std::shared_ptr<Room> room) {
  auto ltcr = getRandomCell(location);
  while (ltcr->x + room->width >= WIDTH || ltcr->y + room->height >= HEIGHT) {
    ltcr = getRandomCell(location);
  }
  mapUtils::paste(room->cells, location, ltcr->x, ltcr->y);

  location->rooms.push_back(room);
}

void Generator::placeWalls(std::shared_ptr<Location> location) {
  if (location->rooms.size() == 0)
    return;
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
          cell->tags = (*nc)->tags;
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
    // TODO: fix
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

void Generator::makePassageBetweenCells(std::shared_ptr<Location> location,
                                        std::shared_ptr<Cell> sc,
                                        std::shared_ptr<Cell> ec) {
  if (sc->y > ec->y) {
    std::swap(sc, ec);
  }

  std::array<Direction, 2> dirs = {S, E};
  auto t = false;
  if (Random::get<bool>()) {
    dirs = {N, W};
  }

  auto l = ec->y - sc->y + 1;
  auto passage = makePassage(sc, dirs[0], l);
  mapUtils::paste(passage->cells, location, sc->x, sc->y);
  passage->x = sc->x;
  passage->y = sc->y;
  // passage->tags.add("DUNGEON");
  location->rooms.push_back(passage);
  sc = passage->cells.back();
  if (sc != ec) {
    if (sc->x > ec->x) {
      std::swap(sc, ec);
    }
    l = ec->x - sc->x + 1;
    passage = makePassage(sc, dirs[1], l);
    mapUtils::paste(passage->cells, location, sc->x, sc->y);
    passage->x = sc->x;
    passage->y = sc->y;
    // passage->tags.add("DUNGEON");
    location->rooms.push_back(passage);
  }
}

void Generator::makePassageBetweenRooms(std::shared_ptr<Location> location,
                                        std::shared_ptr<Room> room1,
                                        std::shared_ptr<Room> room2) {
  auto sc = *Random::get(room1->cells);
  auto ec = *Random::get(room2->cells);
  while (sc == ec) {
    ec = *Random::get(room2->cells);
  }
  makePassageBetweenCells(location, sc, ec);
}

std::pair<std::shared_ptr<Cell>, Cells>
dig(std::shared_ptr<Cell> start, Direction dir, int length, int minWidth,
    int jWidth, int wind = 0, CellSpec type = CellType::GROUND) {
  auto cell = start;
  std::vector<std::string> f = {"CAVE"};
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

// TODO: better room merge
void Generator::fixOverlapped(std::shared_ptr<Location> location) {
  if (location->rooms.size() == 0)
    return;
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
  auto data = entt::locator<GameData>::value();
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
            // TODO: migrate to entitiees
            //  auto o = location->getObjects(c);
            if (R::R() > data.probability["DOOR"] || c->tags.has("CAVE"))
              continue;
            // auto nd = false;
            // for (auto nc : n) {
            //   auto no = location->getObjects(nc);
            //   if (std::find_if(no.begin(), no.end(),
            //                    [](std::shared_ptr<Object> ob) {
            //                      return std::dynamic_pointer_cast<Door>(ob);
            //                    }) != no.end()) {
            //     nd = true;
            //     break;
            //   }
            // }
            // if (nd)
            //   continue;
            // auto door = location->placeDoor(c);
            // if (R::R() < data->probability["SECRET_DOOR"]) {
            //   door->hidden = true;
            // }
            location->addTerrain("DOOR", c);
            d++;
          }
        }
      }
    }
  }
}

void Generator::placeLoot(std::shared_ptr<Location> location, int threat) {
  dlog("place loot");
  auto floor = location->type.floor;
  auto table = LootBoxes::ZERO;
  // if (location->type.type == int(LocationType::DUNGEON)) {
  //   if (threat >= LootTable::DUNGEON.size())
  //     threat = LootTable::DUNGEON.size() - 1;
  table = LootTable::DUNGEON.at(threat);
  // } else if (location->type.type == LocationType::CAVERN) {
  //   // TODO: use cavern table
  //   if (threat >= int(LootTable::DUNGEON.size()))
  //     threat = LootTable::DUNGEON.size() - 1;
  //   table = LootTable::DUNGEON.at(threat);
  // }

  // TODO: gen count and get loot from 100% lootbox
  for (auto room : location->rooms) {
    auto rc = room->getRandomCell(floor);
    std::shared_ptr<Cell> c;
    if (rc)
      c = *rc;
    else
      continue;
    auto n = 0;
    // TODO: counting
    // while (location->getObjects(c).size() != 0 && n < 20) {
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
      // location->addObject<Item>(new_item);
      // auto n = 0;
      auto key = fmt::format("{}", new_item->name);
      std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      std::replace(key.begin(), key.end(), ' ', '_');
      location->log.info(key);
      location->addEntity(key, c);
    }
  }
}

void Generator::placeEnemies(std::shared_ptr<Location> location, int threat) {
  dlog("place enemies");
  auto data = entt::locator<GameData>::value();
  auto &registry = entt::locator<entt::registry>::value();
  // if(data.prototypes == nullptr) {
  if (!registry.view<entt::tag<"proto"_hs>>().size() ) {
      location->log.error("prototypes not loaded");
      return;
  }
  std::map<EnemySpec, float> table;
  auto pather = new micropather::MicroPather(location.get());
  micropather::MPVector<void *> path;
  float totalCost = 0;

  // if (location->type.type == LocationType::DUNGEON) {
  //   if (threat >= int(SpawnTable::DUNGEON.size()))
  // threat = SpawnTable::DUNGEON.size() - 1;
  table = SpawnTable::DUNGEON.at(threat);
  // } else if (location->type.type == LocationType::CAVERN) {
  //   // TODO: use cavern table
  //   if (threat >= int(SpawnTable::DUNGEON.size()))
  //     threat = SpawnTable::DUNGEON.size() - 1;
  //   table = SpawnTable::DUNGEON.at(threat);
  // }
  for (auto room : location->rooms) {
    if (room->type == RoomType::PASSAGE)
      continue;
    std::vector<std::shared_ptr<Cell>> cells(room->cells.size());
    auto it = std::copy_if(
        room->cells.begin(), room->cells.end(), cells.begin(),
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
      if (location->enterCell) {
        int result = pather->Solve(c.get(), location->enterCell.get(), &path,
                                   &totalCost);
        if (result != micropather::MicroPather::SOLVED)
          break;
      }

      // auto enemy = mapUtils::makeEnemy(location, c, type);
      // location->addObject<Enemy>(enemy);
      auto key = type.name;
      std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      std::replace(key.begin(), key.end(), ' ', '_');
      location->log.info(key);
      auto e = location->addEntity(key, c);
      if(!location->registry.valid(e) || !location->registry.all_of<hf::meta>(e)) return;
      location->creatures[e] = std::make_shared<Creature>();
      location->creatures[e]->name = location->registry.get<hf::meta>(e).id;
      location->creatures[e]->entity = e;
      location->creatures[e]->currentCell = c;
    }
  }
}

void Generator::makeRiver(std::shared_ptr<Location> location) {
  dlog("place river");
  auto start = location->cells[0][location->cells.front().size() / 3 +
                                  rand() % location->cells.front().size() / 3];
  auto end = location->cells.size() - 2;
  auto dir = Random::get({S, E});
  if (dir == E) {
    start = location->cells[location->cells.size() / 3 +
                            rand() % location->cells.size() / 3][0];
    auto end = location->cells.front().size() - 2;
    location->tags.add("RIVER_H");
  } else {
    location->tags.add("RIVER_V");
  }
  auto river = dig(start, dir, end, 2, 2, 1, CellType::WATER);
  mapUtils::paste(river.second, location, 0, 0);
}

std::vector<std::shared_ptr<Cell>>
Generator::scatter(std::shared_ptr<Location> location, std::string tid, int min,
                   int max, bool stickWalls = false) {
  std::vector<std::shared_ptr<Cell>> res;
  auto floor = location->type.floor;
  auto tc = Random::get(min, max);
  int n;
  for (n = 0; n < tc;) {
    auto room = *Random::get(location->rooms);
    // TODO: ensure cell uniq
    auto cell = *Random::get(room->cells);
    if (cell->type != floor || location->getEntities(cell).size() > 0)
      continue;
    if (stickWalls) {
      auto ngs = location->getNeighbors(cell);
      if (std::find_if(ngs.begin(), ngs.end(), [](std::shared_ptr<Cell> nc) {
            return nc->type == CellType::WALL;
          }) == ngs.end())
        continue;
    }
    if (tid != "") {
      location->addEntity(tid, cell);
    }
    res.push_back(cell);
    n++;
  }
  location->log.debug("Scatter: {} {}", tid, n);
  return res;
}

void Generator::placeTorches(std::shared_ptr<Location> location) {
  dlog("place torches");
  auto tid = "TORCH_STAND";
  Generator::scatter(location, tid, 3, 10);

  if (location->exitCell) {
    auto nbrs = location->getNeighbors(location->exitCell);
    location->addTerrain(tid, *Random::get(nbrs));
  }

  if (location->enterCell) {
    auto nbrs = location->getNeighbors(location->enterCell);
    location->addTerrain(tid, *Random::get(nbrs));
  }

  // fmt::print("Torches: {}\n", n);
}

std::vector<std::shared_ptr<Cell>>
Generator::getPath(std::shared_ptr<Location> location,
                   std::shared_ptr<Cell> start, std::shared_ptr<Cell> end) {
  auto pather = new micropather::MicroPather(location.get());
  micropather::MPVector<void *> path;
  std::vector<std::shared_ptr<Cell>> res;
  float totalCost = 0;
  pather->Reset();
  int result = pather->Solve(start.get(), end.get(), &path, &totalCost);

  for (auto i = 0; i < path.size(); i++) {
    auto c = (Cell *)(path[i]);
    auto ptr = location->cells[c->y][c->x];
    if (ptr != nullptr) {
      res.push_back(ptr);
    }
  }

  delete pather;
  return res;
}

void Generator::placeHero(std::shared_ptr<Location> location) {
  dlog("place hero");
  auto n = location->getNeighbors(location->enterCell);
  auto c = *Random::get(n);
  location->player = std::make_shared<Player>();
  location->player->entity = location->addEntity("HERO", c);
  location->creatures[location->player->entity] = location->player;
  location->player->currentCell = c;
  dlog("hero was placed");
}

bool Generator::placeStairs(std::shared_ptr<Location> location) {
  dlog("place stairs");

  auto floor = location->type.floor;
  auto entr = getRandomCell(location, floor);
  if (location->type.enterCell != nullptr) {
    auto e = location->type.enterCell;
    entr =
        std::make_optional<std::shared_ptr<Cell>>(location->cells[e->y][e->x]);
  }
  dlog("entt found 1");
  auto exir = getRandomCell(location, floor);
  dlog("exit found 1");
  while (!entr) {
    entr = getRandomCell(location, floor);
  }
  dlog("entr found");
  while (!exir) {
    exir = getRandomCell(location, floor);
  }
  dlog("exit found");
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
  location->log.debug("Path: {} / {}", totalCost, i);
  while (result != micropather::MicroPather::SOLVED ||
         (totalCost < 30 && i < 10) || path.size() < 20) {
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
    location->log.debug("Path: {} / {}", totalCost, i);
    if (i == 30) {
      location->log.warn("cannot place exit");
      delete pather;
      return false;
    }
  }

  for (auto i = 0; i < path.size(); i++) {
    auto c = (Cell *)(path[i]);
    auto ptr = location->cells[c->y][c->x];
    if (ptr != nullptr) {
      location->path.push_back(ptr);
    }
  }

  delete pather;

  // TODO: migrate to entities
  //  auto _objects = location->objects;
  //  for (auto o : _objects) {
  //    if (o->currentCell != nullptr && (o->currentCell == location->exitCell
  //    ||
  //                                      o->currentCell ==
  //                                      location->enterCell)) {
  //      location->removeObject(o);
  //    }
  //  }

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

void Generator::makePassages(std::shared_ptr<Location> location,
                             std::vector<std::shared_ptr<Room>> d_rooms) {
  dlog("place passages");
  auto pc = rand() % 10;
  auto n = 0;
  std::set<std::shared_ptr<Room>> rooms;
  std::vector<std::shared_ptr<Room>> halls = d_rooms;

  while (n < pc) {
    auto room1 = *Random::get(d_rooms);
    auto room2 = *Random::get(d_rooms);
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
      auto room2 = *Random::get(d_rooms);
      while (room2 == r) {
        room2 = *Random::get(d_rooms);
      }
      makePassageBetweenRooms(location, r, room2);
    }
    for (auto c : r->cells) {
      c->room = r;
    }
  }
}

void Generator::makeExterior(std::shared_ptr<Location> location) {
  // auto data = entt::locator<GameData>::get().lock();
  // dlog("place caves");
  // auto rc = rand() % 20 + 35;

  // location->cells = mapUtils::fill(HEIGHT * 2, WIDTH * 2, CellType::UNKNOWN);
  // for (auto n = 0; n < rc; n++) {
  //   auto room = Room::makeRoom(34, 12, 34, 12, CellType::WALL);

  //   for (auto c : room->cells) {
  //     if (R::R() > 0.45) {
  //       c->type = CellType::GROUND;
  //       c->passThrough = true;
  //       c->seeThrough = true;
  //     }
  //   }
  //   auto ry = rand() % (location->cells.size() - room->height - 8) + 8;
  //   auto rx = rand() % (location->cells.front().size() - room->width - 8) +
  //   8; mapUtils::paste(room->cells, location, rx, ry); room->x = rx; room->y
  //   = ry; room->threat = rand() % 4; room->tags.add("CAVE");
  //   location->rooms.push_back(room);

  //   for (auto c : room->cells) {
  //     c->room = room;
  //   }
  // }
  // Generator::fixOverlapped(location);

  // makePassages(location);

  // for (auto n = 0; n < rand() % 4 + 2; n++) {
  //   auto _cells = location->cells;
  //   std::shuffle(_cells.begin(), _cells.end(),
  //                std::default_random_engine(rand()));
  //   for (auto r : _cells) {
  //     std::shuffle(r.begin(), r.end(), std::default_random_engine(rand()));
  //     for (auto c : r) {
  //       if (c->type == CellType::UNKNOWN) {
  //         auto n = location->getNeighbors(c);
  //         auto fn =
  //             std::count_if(n.begin(), n.end(), [=](std::shared_ptr<Cell> nc)
  //             {
  //               return nc->type != CellType::UNKNOWN;
  //             });
  //         if ((fn >= 4 && rand() % 10 < 8) || fn == 3) {
  //           // c->type = CellType::GROUND;
  //           mapUtils::updateCell(location->cells[c->y][c->x],
  //           CellType::GROUND,
  //                                c->tags.tags);
  //         }
  //       } else {
  //         auto n = location->getNeighbors(c);
  //         auto fn =
  //             std::count_if(n.begin(), n.end(), [=](std::shared_ptr<Cell> nc)
  //             {
  //               return nc->type == CellType::UNKNOWN;
  //             });
  //         if (fn >= 4) {
  //           c->type = CellType::UNKNOWN;
  //           // mapUtils::updateCell(location->cells[y][x], CellType::UNKNOWN,
  //           // c->features);
  //         }
  //       }
  //     }
  //   }
  // }

  // for (auto r : location->cells) {
  //   for (auto c : r) {
  //     if (c->type != CellType::UNKNOWN) {
  //       mapUtils::updateCell(location->cells[c->y][c->x], CellType::GROUND,
  //                            c->tags.tags);
  //       if (R::R() < data->probability["EXT_BUSH"]) {
  //         location->addTerrain("BUSH", c);
  //         //
  //         s->triggers.push_back(std::make_shared<EnterTrigger>([=](std::shared_ptr<Creature>
  //         // actor){
  //         //   return Triggers::BUSH_TRIGGER(c, location);
  //         // }));
  //       } else if (R::R() < data->probability["EXT_TREE"]) {
  //         location->addTerrain("TREE", c);
  //       } else if (R::R() < data->probability["EXT_GRASS"]) {
  //         location->addEntity("HERB_GREEN", c);
  //       }
  //     }
  //   }
  // }
}

std::vector<std::shared_ptr<Room>>
Generator::placeRooms(std::shared_ptr<Location> location, int min, int max,
                      int w, int h, int x, int y) {
  dlog("place rooms");
  auto rc = Random::get<int>(min, max);
  std::vector<std::shared_ptr<Room>> rooms;

  // TODO: ensure place in location borders
  // if (location->type.enterCell != nullptr) {
  //   auto room = Room::makeRoom(5, 3, 5, 3);
  //   auto e = location->type.enterCell;
  //   auto rx = e->x - room->width / 2;
  //   auto ry = e->y - room->height / 2;
  //   mapUtils::paste(room->cells, location, rx, ry);
  //   room->x = rx;
  //   room->y = ry;
  //   room->threat = rand() % 4;
  //   location->rooms.push_back(room);
  // }

  for (auto n = 0; n < rc; n++) {
    auto room = Room::makeRoom();
    auto ry = y + Random::get<int>(2, h - room->height - 2);
    auto rx = x + Random::get<int>(2, w - room->width - 2);
    mapUtils::paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    room->threat = Random::get(0, 4);
    location->rooms.push_back(room);
    rooms.push_back(room);
  }
  return rooms;
}

std::vector<std::shared_ptr<Room>>
Generator::placeCaves(std::shared_ptr<Location> location, int min, int max,
                      int w, int h, int x, int y) {
  auto data = entt::locator<GameData>::value();
  dlog("place caves");
  auto rc = Random::get<int>(min, max);
  std::vector<std::shared_ptr<Room>> rooms;

  for (auto n = 0; n < rc; n++) {
    auto room = Room::makeRoom(h / 2, 12, w / 2, 12, location->type.border);

    for (auto c : room->cells) {
      if (R::R() > data.probability["CAVERN_WALL"]) {
        mapUtils::updateCell(c, location->type.floor);
      }
    }
    auto ry = y + Random::get<int>(2, h - room->height - 2);
    auto rx = x + Random::get<int>(2, w - room->width - 2);
    mapUtils::paste(room->cells, location, rx, ry);
    room->x = rx;
    room->y = ry;
    room->threat = rand() % 4;
    room->tags.add("CAVE");
    location->rooms.push_back(room);
    rooms.push_back(room);

    for (auto c : room->cells) {
      c->tags.add("CAVE");
      c->room = room;
    }
  }
  Generator::fixOverlapped(location);

  for (auto i = 0; i < 5; i++) {
    for (auto room : location->rooms) {
      room->type = RoomType::CAVERN;
      for (auto c : room->cells) {
        auto n = location->getNeighbors(c);
        auto fn =
            std::count_if(n.begin(), n.end(), [&](std::shared_ptr<Cell> nc) {
              return nc->type == location->type.floor;
            });

        if (c->type == location->type.floor) {
          if (fn < 4) {
            mapUtils::updateCell(c, location->type.border);
          }
        } else if (c->type == location->type.border) {
          if (fn >= 6) {
            mapUtils::updateCell(c, location->type.floor);
          }
        }
      }
    }
  }
  // Generator::cleanWalls(location);
  return rooms;
}

void Generator::cleanWalls(std::shared_ptr<Location> location) {
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
      }
    }
  }
}

void Generator::makeCavePassage(std::shared_ptr<Location> location) {
  auto data = entt::locator<GameData>::value();
  dlog("place cave passage");
  std::vector<Direction> ds{N, E, S, W};
  auto room = location->rooms[rand() % location->rooms.size()];
  auto cell = room->cells[rand() % room->cells.size()];
  for (auto n = 0; n < rand() % 6 + 2; n++) {
    auto res = randomDig(cell, ds[rand() % ds.size()], rand() % 30);
    cell = res.first;
    auto newRoom = std::make_shared<Room>(RoomType::CAVE, res.second);
    room->tags.add("CAVE");
    mapUtils::paste(newRoom->cells, location, 0, 0);
    newRoom->x = 0;
    newRoom->y = 0;
    newRoom->threat = rand() % 4;
    location->rooms.push_back(newRoom);
    for (auto c : newRoom->cells) {
      if (c->type == CellType::GROUND &&
          R::R() < data.probability["CAVE_ROCK"]) {
        location->addEntity("ROCK", c);
      } else if (c->type == CellType::GROUND &&
                 R::R() < data.probability["CAVE_GRASS"]) {
        if (R::R() < data.probability["CAVE_BUSH"]) {
          location->addTerrain("BUSH", c);
        } else {
          location->addEntity("HERB_GREEN", c);
        }
        continue;
      }
    }
  }
}

void Generator::placeTemplateInRoom(std::shared_ptr<Location> location,
                                    std::shared_ptr<RoomTemplate> rtp) {
  auto room = rtp->generate(location);
  if (!room)
    return;
  Room::printRoom(location, room);

  // TODO: shuffle suitable rooms and select
  std::vector<std::shared_ptr<Room>> rooms;
  for (auto target : location->rooms) {
    if (target->width - 2 < room->width || target->height - 2 < room->height ||
        target->x == 0)
      continue;
    rooms.push_back(target);
  }
  if (rooms.size() == 0) {
    // TODO: display template name
    location->log.warn("cannot place template");
    return;
  }
  auto target = *Random::get(rooms);
  auto dx = target->width - room->width - 1;
  auto dy = target->height - room->height - 1;

  location->addRoom(room, target->x + Random::get(0, dx) + 1,
                    target->y + Random::get(0, dy) + 1);
}

void placeTemplate(std::shared_ptr<Location> location,
                   std::shared_ptr<RoomTemplate> rtp) {
  auto room = rtp->generate(location);
  placeRoom(location, room);
}

void placeStartTemplateInRoom(std::shared_ptr<Location> location,
                              std::shared_ptr<RoomTemplate> rtp) {
  auto room = rtp->generate(location);
  placeStartInRoom(location, room);
}

std::shared_ptr<Room> placeLake(std::shared_ptr<Location> location) {
  // return placeBlob(location, CellType::WATER);
  auto room = Room::makeBlob(location, 12, 3, 12, 3, CellType::WATER,
                             CellType::UNKNOWN, true);
  placeInRoom(location, room);
  return room;
}

std::shared_ptr<Location> Generator::getLocation(LocationSpec spec) {
  using milliseconds = std::chrono::duration<double, std::milli>;
  //auto data = entt::locator<GameData>::get().lock();

  auto t0 = std::chrono::system_clock::now();
  std::map<std::string, milliseconds> timings;
  std::chrono::time_point<std::chrono::system_clock> start;
  std::chrono::time_point<std::chrono::system_clock> end;

  auto location = std::make_shared<Location>(spec);
  location->depth = spec.threat;

  location->cells = mapUtils::fill(location->type.height, location->type.width,
                                   CellType::UNKNOWN);

  // execTemplates(location, 0);

  // if (spec.type == LocationType::DUNGEON) {
  //   start = std::chrono::system_clock::now();
  //   placeRooms(location);
  //   end = std::chrono::system_clock::now();
  //   timings["placeRooms"] = end - start;

  //   start = std::chrono::system_clock::now();
  //   makePassages(location);
  //   end = std::chrono::system_clock::now();
  //   timings["makePassages"] = end - start;

  //   if (location->tags.has("CAVE_PASSAGE")) {
  //     start = std::chrono::system_clock::now();
  //     makeCavePassage(location);
  //     end = std::chrono::system_clock::now();
  //     timings["makeCavePassage"] = end - start;
  //   }
  // } else if (spec.type == LocationType::CAVERN) {
  //   start = std::chrono::system_clock::now();
  //   placeCaves(location);
  //   end = std::chrono::system_clock::now();
  //   timings["placeCaves"] = end - start;
  // } else if (spec.type == LocationType::EXTERIOR) {
  //   start = std::chrono::system_clock::now();
  //   // placeCaves(location);
  //   //
  //   // location->cells = mapUtils::fill(HEIGHT, WIDTH, CellType::UNKNOWN);
  //   // auto room = Room::makeBlob(location, 100, 90, 100, 90,
  //   CellType::GROUND,
  //   // CellType::UNKNOWN, true); mapUtils::paste(room->cells, location, 0,
  //   0); makeExterior(location); end = std::chrono::system_clock::now();
  //   timings["gen ground"] = end - start;
  //   return location;
  // } else if (spec.type == LocationType::ZERO) {
  //   // location->cells = mapUtils::fill(HEIGHT, WIDTH, CellType::UNKNOWN);
  //   if (spec.genFunc) {
  //     spec.genFunc(location);
  //   }
  //   return location;
  // }
  //
  if (spec.genFunc) {
    spec.genFunc(location);
  }

  // execTemplates(location, 1);

  // start = std::chrono::system_clock::now();
  // Generator::fixOverlapped(location);
  // end = std::chrono::system_clock::now();
  // timings["fixOverlapped"] = end - start;

  // start = std::chrono::system_clock::now();

  // execTemplates(location, 2);

  // if (spec.type != LocationType::EXTERIOR) {
  //   auto success = placeStairs(location);

  //   if (!success) {
  //     log.info("regen location");
  //     dlog("regen location");
  //     return getLocation(spec);
  //   }
  // }

  // execTemplates(location, 3);
  // execTemplates(location, 4);
  // execTemplates(location, 5);
  // placeWalls(location);
  // execTemplates(location, 6);

  // execTemplates(location, 7);
  for (auto r : location->cells) {
    for (auto c : r) {
      for (auto cf : location->type.cellTags.tags) {
        c->tags.add(cf);
      }
    }
  }

  std::vector<std::string> timeMarks;
  for (auto [mark, ms] : timings) {
    timeMarks.push_back(fmt::format("{}: {}", mark, ms.count()));
  }

  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  return location;
}

void Generator::execTemplates(std::shared_ptr<Location> location,
                              std::string key, int min, int max) {
  std::vector<bool> p;
  for (auto [k, v] : location->type.templateMap[key]) {
    p.push_back(Random::get<bool>(v));
  }
  auto n = 0;
  for (auto [k, v] : location->type.templateMap[key]) {
    // location->log.debug("Executing template: {} @ {}", key, k);
    if (key == "DUNGEON" && k == "LOOT") {
      location->log.debug("Executing template: {} @ {}", key, k);
    }
    if (p[n]) {
      if (Generator::templates.find(k) != templates.end()) {
        auto tpl = templates[k];
        if (tpl->stage >= min && tpl->stage < max) {
          Generator::placeTemplateInRoom(location, tpl);
          location->tags.add(k);
        }
      } else if (features.find(k) != features.end()) {
        auto tpl = features[k];
        if (tpl->stage >= min && tpl->stage < max) {
          tpl->generate(location);
          location->tags.add(k);
        }
      }
    }
    n++;
  }
  // }
};
