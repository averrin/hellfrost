#ifndef __ROOM_H_
#define __ROOM_H_
#include "lss/deps.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/randomTools.hpp"
#include "lss/generator/mapUtils.hpp"
#include <lss/gameData.hpp>
#include <sol/sol.hpp>
// #include "lss/game/item.hpp"
// #include "lss/game/trigger.hpp"
// #include "lss/game/content/enemies.hpp"

enum class RoomType {
  UNKNOWN,
  HALL,
  PASSAGE,
  CAVERN,
  CAVE,
};

enum class RoomFeature { CAVE, DUNGEON, STATUE, ALTAR };

class Room {
public:
  Room(RoomType t, Cells c) : type(t) {
    height = c.size();
    width = c.front().size();
    for (auto r : c) {
      for (auto cell : r) {
        // if (cell->type == CellType::UNKNOWN)
        //   continue;
        cells.push_back(cell);
      }
    }
  }
  std::vector<std::shared_ptr<Cell>> cells;
  RoomType type = RoomType::UNKNOWN;
  int threat = 0;
  std::vector<RoomFeature> features;
  std::vector<entt::entity> entities;

  void addEntity(entt::entity e) { entities.push_back(e); }

  int height = 0;
  int width = 0;

  int x = 0;
  int y = 0;

  std::shared_ptr<Cell> getCell(int x, int y) { return cells[x + y * width]; }
  std::string color;

  void rotateEntities(std::shared_ptr<entt::registry> registry) {

    for (auto e : entities) {
      auto &p = registry->get<hf::position>(e);
      auto n = 0;
      for (auto c : cells) {
        if (c->x == p.x && c->y == p.y) {
          auto cx = n % width;
          auto cy = n / width;
          auto nx = height - (cy + 1);
          auto ny = cx;
          registry->assign_or_replace<hf::position>(e, nx, ny, p.z);
          break;
        }
        n++;
      }
    }
  }
  void rotate() {
    std::vector<std::shared_ptr<Cell>> result(cells.size());
    auto n = 0;
    for (auto c : cells) {
      auto cx = n % width;
      auto cy = n / width;
      auto nx = height - (cy + 1);
      auto ny = cx;
      auto nn = nx + height * ny;
      result[nn] = c;
      c->_x = nx;
      c->_y = ny;
      c->x = nx + x;
      c->y = ny + y;
      n++;
    }
    cells = result;
    auto w = width;
    auto h = height;
    width = h;
    height = w;
  }

  std::optional<std::shared_ptr<Cell>> getRandomCell(CellSpec type) {
    std::vector<std::shared_ptr<Cell>> result(cells.size());
    auto it = std::copy_if(cells.begin(), cells.end(), result.begin(),
                           [type](auto cell) { return cell->type == type; });
    result.resize(std::distance(result.begin(), it));
    if (result.size() == 0) {
      // std::cout << rang::fg::red << "FATAL: " << "cannot get random cell with
      // this type" << rang::style::reset << std::endl;
      return std::nullopt;
    }

    auto cell = result[rand() % result.size()];
    while (cell->type != type) {
      cell = result[rand() % result.size()];
    }
    return cell;
  }

  static std::shared_ptr<Room> makeRoom(int hMax = 11, int hMin = 3,
                                        int wMax = 15, int wMin = 3,
                                        CellSpec type = CellType::FLOOR) {
    auto rh = R::Z(hMin, hMax);
    auto rw = R::Z(wMin, wMax);

    auto cells = mapUtils::fill(rh, rw, type);
    auto room = std::make_shared<Room>(RoomType::HALL, cells);
    for (auto c : room->cells) {
      c->room = room;
    }
    room->color = (*Random::get(webColorNames)).first;
    return room;
  }

  static void paste(std::shared_ptr<Room> inner, std::shared_ptr<Room> outer,
                    int x, int y) {
    for (auto cell : inner->cells) {
      auto r = y + cell->y;
      auto c = x + cell->x;
      if (cell->type != CellType::UNKNOWN) {
        if (cell->type != CellType::EMPTY) {
          outer->cells.at(r * outer->width + c) = cell;
        } else {
          outer->cells.at(r * outer->width + c)->features = cell->features;
          cell = outer->cells.at(r * outer->width + c);
        }
      }
      cell->x = c;
      cell->y = r;
    }
    inner->x = outer->x + x;
    inner->y = outer->y + y;
  }

  std::vector<std::shared_ptr<Cell>> getNeighbors(std::shared_ptr<Cell> cell) {
    std::vector<std::shared_ptr<Cell>> nbrs;
    if (cell->_x > 0 && cell->_y > 0) {
      nbrs.push_back(cells[(cell->_y - 1) * width + (cell->_x - 1)]);
    }
    if (cell->_y > 0) {
      nbrs.push_back(cells[(cell->_y - 1) * width + (cell->_x)]);
    }
    if (cell->_y > 0 && cell->_x < int(width - 1)) {
      nbrs.push_back(cells[(cell->_y - 1) * width + (cell->_x + 1)]);
    }
    if (cell->_x > 0) {
      nbrs.push_back(cells[(cell->_y) * width + (cell->_x - 1)]);
    }
    if (cell->_x < int(width - 1)) {
      nbrs.push_back(cells[(cell->_y) * width + (cell->_x + 1)]);
    }
    if (cell->_x > 0 && cell->_y < int(height - 1)) {
      nbrs.push_back(cells[(cell->_y + 1) * width + (cell->_x - 1)]);
    }
    if (cell->_y < int(height - 1)) {
      nbrs.push_back(cells[(cell->_y + 1) * width + (cell->_x)]);
    }
    if (cell->_y < int(height - 1) && cell->_x < int(width - 1)) {
      nbrs.push_back(cells[(cell->_y + 1) * width + (cell->_x + 1)]);
    }

    // nbrs.push_back(getCellD(cell, -1, -1));
    // nbrs.push_back(getCellD(cell, -1, 0));
    // nbrs.push_back(getCellD(cell, 0, -1));
    // nbrs.push_back(getCellD(cell, -1, 1));
    // nbrs.push_back(getCellD(cell, 0, 1));
    // nbrs.push_back(getCellD(cell, 1, 1));
    // nbrs.push_back(getCellD(cell, 1, -1));
    // nbrs.push_back(getCellD(cell, 1, 0));
    // nbrs.erase(
    //     std::remove_if(nbrs.begin(), nbrs.end(),
    //                    [](std::shared_ptr<Cell> c) { return c == nullptr; }),
    //     nbrs.end());

    return nbrs;
  }

  template <typename T>
  void addObject(std::shared_ptr<Location> location, std::shared_ptr<T> o,
                 int x, int y) {
    entities.push_back(o);
    auto cell = getCell(x, y);
    location->addObject(o, cell);
  }

  static std::shared_ptr<Room> _makeBlob(std::shared_ptr<Room> room,
                                         CellSpec type, CellSpec outerType);
  static std::shared_ptr<Room> makeBlob(std::shared_ptr<Location> location,
                                        int hMax = 11, int hMin = 3,
                                        int wMax = 15, int wMin = 3,
                                        CellSpec type = CellType::FLOOR,
                                        CellSpec outerType = CellType::UNKNOWN,
                                        bool smooth = true, int rounds = 5);

  std::string getInfo() {
    auto room = this;
    return fmt::format("{}x{} = {} @ {}.{}", room->width, room->height,
                       room->cells.size(), room->x, room->y);
  }

  static void printRoom(std::shared_ptr<Location> location,
                        std::shared_ptr<Room> room) {
    auto n = 0;

    fmt::print("Room stats: {}\n", room->getInfo());
    for (auto c : room->cells) {
      if (n % room->width == 0 && n > 0) {
        fmt::print("\n");
      }
      fmt::print("[{}.{}]", c->_x, c->_y);
      n++;
    }
    fmt::print("\n");
    n = 0;
    for (auto c : room->cells) {
      if (n % room->width == 0 && n > 0) {
        fmt::print("\n");
      }
      auto o = location->getObjects(c);
      if (o.size() > 0) {
        fmt::print("o");
        n++;
        continue;
      }
      auto e = location->getEntities(c);
      if (e.size() > 0) {
        fmt::print("e");
        n++;
        continue;
      }
      if (c->type == CellType::WALL) {
        fmt::print("#");
      } else if (c->type == CellType::FLOOR) {
        fmt::print(".");
      } else if (c->type == CellType::UNKNOWN) {
        fmt::print(" ");
      } else if (c->type == CellType::VOID) {
        fmt::print("⌄");
      } else if (c->type == CellType::WATER) {
        fmt::print("=");
      } else {
        fmt::print("?");
      }
      n++;
    }
    fmt::print("\n");
  }
};

typedef std::function<std::shared_ptr<Room>(std::shared_ptr<Location>)>
    RoomGenerator;
class RoomTemplate {
  RoomGenerator generator;
  sol::function luaGenerator;
  bool isLua = false;

public:
  int stage = 5;
  RoomTemplate(RoomGenerator g, int s = 5) : generator(g), stage(s) {}
  RoomTemplate(sol::function g, int s = 5) : luaGenerator(g), stage(s) { isLua = true; }
  std::shared_ptr<Room> generate(std::shared_ptr<Location> location) {
    if (isLua) {
      return generator(location);
    }
    return generator(location);
  }
};

namespace RoomTemplates {

const auto TREASURE_ROOM = std::make_shared<RoomTemplate>(
    /*
            ###
            #(#
            ###
    */
    [](std::shared_ptr<Location> location) {
      auto innerRoom = Room::makeRoom(3, 1, 3, 1, CellType::FLOOR);
      auto room = Room::makeRoom(innerRoom->height + 2, innerRoom->height + 2,
                                 innerRoom->width + 2, innerRoom->width + 2,
                                 CellType::WALL);

      Room::paste(innerRoom, room, 1, 1);
      // room->print();

      auto cell = room->getRandomCell(CellType::FLOOR);

      auto box = LootBoxes::DUNGEON_3;
      for (auto item : box.open(true)) {
        item->setCurrentCell(*cell);
        location->addObject<Item>(item);
      }

      // for (auto c: innerRoom->cells) {
      //   mapUtils::updateCell(c, CellType::FLOOR, {CellFeature::BLOOD});
      // }

      return room;
    });

}; // namespace RoomTemplates

#endif // __ROOM_H_
