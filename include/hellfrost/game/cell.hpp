#ifndef __CELL_H_
#define __CELL_H_
#include <hellfrost/game/cell_feature.hpp>
#include <hellfrost/game/cell_spec.hpp>

namespace hellfrost {
class RawCell {
public:
  RawCell(int _x, int _y, int _z, CellSpec t) : x(_x), y(_y), z(_z), type(t) {}
  CellSpec type = CellType::UNKNOWN;

  int x = 0;
  int y = 0;
  int z = 0;

  bool passThrough = false;
  bool seeThrough = false;
  std::vector<CellFeature> features;
};

typedef std::optional<std::shared_ptr<RawCell>> Cell;
typedef std::vector<std::vector<Cell>> Cells;

} // namespace hellfrost
#endif // __CELL_H_
