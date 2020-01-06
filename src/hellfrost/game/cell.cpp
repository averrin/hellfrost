#include <hellfrost/game/cell.hpp>

namespace hellfrost {
std::vector<CellSpec> Cell::types = std::vector<CellSpec>{
      CellType::EMPTY,    CellType::UNKNOWN, CellType::FLOOR,
      CellType::GROUND,   CellType::WALL,    CellType::DOWNSTAIRS,
      CellType::UPSTAIRS, CellType::WATER};

};
