#ifndef __CELL_H_
#define __CELL_H_
#include <hellfrost/game/cell_feature.hpp>
#include <hellfrost/game/cell_spec.hpp>

namespace hellfrost {
class RawCell {
public:
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
