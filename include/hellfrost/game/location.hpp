#ifndef __LOCATION_H_
#define __LOCATION_H_
#include <memory>
#include <optional>

namespace hellfrost {
class Cell;
class Location {
public:
  std::optional<std::shared_ptr<Cell>> getCell(int x, int y, int z);
  void invalidate();

  int width = 0;
  int height = 0;
};
} // namespace hellfrost
#endif // __LOCATION_H_
