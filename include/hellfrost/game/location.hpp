#ifndef __LOCATION_H_
#define __LOCATION_H_
#include <memory>

#include <hellfrost/game/cell.hpp>

namespace hellfrost {
class Location {
  bool damaged = true;

public:
  Cell getCell(int x, int y, int z);
  void invalidate() { damaged = true; }

  int width = 0;
  int height = 0;
};
} // namespace hellfrost
#endif // __LOCATION_H_
