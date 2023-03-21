#include "lss/generator/room.hpp"

// 000000000
// 011111110
// 011111110
// 000000000

std::shared_ptr<Room> Room::_makeBlob(std::shared_ptr<Room> room, CellSpec type, CellSpec outerType) {
  std::map<std::shared_ptr<Cell>, int> vns;
  for (auto c : room->cells) {
    auto n = room->getNeighbors(c);
    auto vn =
        std::count_if(n.begin(), n.end(), [type](std::shared_ptr<Cell> nc) {
          return nc->type == type;
        });
    vns[c] = vn;
  }

  for (auto c : room->cells) {
    auto vn = vns[c];
    if (c->type == type) {
      if (vn < 4) {
        // fmt::print("{} / {}.{} {}\n", c->getSId(), c->_x, c->_y, vn);
        c->type = outerType;
        c->passThrough = outerType.passThrough;
        c->seeThrough = outerType.seeThrough;
      } else if (c->type != type) {
        if (vn >= 6) {
          c->type = type;
          c->passThrough = type.passThrough;
          c->seeThrough = type.seeThrough;
        }
      }
    }
  }
   return room;
}
std::shared_ptr<Room> Room::makeBlob(std::shared_ptr<Location> location,
                                     int hMax, int hMin, int wMax, int wMin,
                                     CellSpec type, CellSpec outerType,
                                     bool smooth, int rounds) {
  auto room = Room::makeRoom(hMax, hMin, wMax, wMin, outerType);
  auto outerRoom = Room::makeRoom(room->height + 2, room->height + 2,
                                  room->width + 2, room->width + 2, outerType);

  for (auto c : room->cells) {
    if (R::R() > 0.35) {
      c->type = type;
      c->passThrough = type.passThrough;
      c->seeThrough = type.seeThrough;
    }
  }

  // Room::printRoom(location, room);
  // Room::paste(room, outerRoom, 1, 1);
  // Room::printRoom(location, outerRoom);

  for (auto n = 0; n < rounds; n++) {
    room = Room::_makeBlob(room, type, outerType);
    // Room::printRoom(location, room);
  }


  if (smooth) {
    for (auto n = 0; n < 5; n++) {
      for (auto c : room->cells) {
        if (c->type == outerType) {
          auto n = room->getNeighbors(c);
          auto fn =
              std::count_if(n.begin(), n.end(), [=](std::shared_ptr<Cell> nc) {
                return nc->type == type;
              });
          if (fn == 0) {
            c->type = CellType::UNKNOWN;
          } else if (fn >= 6) {
            c->type = type;
          }
        }
      }
    }
  }

  // room->print();
  return room;
}
