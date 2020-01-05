#ifndef __CELL_H_
#define __CELL_H_
#include <hellfrost/game/cell_feature.hpp>
#include <hellfrost/game/cell_spec.hpp>

namespace hellfrost {
class Cell {
public:
  CellSpec type = CellType::EMPTY;

  int x = 0;
  int y = 0;
  int z = 0;

  bool passThrough = false;
  bool seeThrough = false;
  std::vector<CellFeature> features;

  static std::vector<CellSpec> types;
};
} // namespace hellfrost
#endif // __CELL_H_
