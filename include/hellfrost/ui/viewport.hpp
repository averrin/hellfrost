#ifndef __VIEWPORT_H_
#define __VIEWPORT_H_
#include <mutex>

#include <librandom/random.hpp>

#include <hellfrost/deps.hpp>

#include <hellfrost/game/cell.hpp>
#include <hellfrost/game/location.hpp>
#include <hellfrost/ui/tile.hpp>

namespace hellfrost {
class Viewport {
  LibLog::Logger log = LibLog::Logger(fmt::color::gold, "VIEW");
  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();
  std::mutex mutex;

public:
  Viewport() { log.setParent(&LibLog::Logger::getInstance()); }
  void update(std::shared_ptr<Location> l) {
    view_x = 0;
    view_y = 0;
    view_z = 0;
    location = l;
  }

public:
  float scale = 1.f;
  int width = 100;
  int height = 50;

  int view_x = 0;
  int view_y = 0;
  int view_z = 0;

  std::shared_ptr<Location> location;

  std::pair<Cell, int> getCell(int _x, int _y, int _z) {
    auto l = fmt::format("get cell: {}.{}.{}", _x, _y, _z);
    auto cell = location->getCell(_x, _y, _z);
    if (_z > 0 && !cell) {
      return getCell(_x, _y, _z - 1);
    }
    return std::make_pair(cell, _z);
  }
};
} // namespace hellfrost
#endif // __VIEWPORT_H_
