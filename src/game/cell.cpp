#include "lss/game/cell.hpp"

std::vector<CellSpec> Cell::types = std::vector<CellSpec>{
      CellType::EMPTY,    CellType::UNKNOWN, CellType::FLOOR,
      CellType::GROUND,   CellType::WALL,    CellType::DOWNSTAIRS,
      CellType::UPSTAIRS, CellType::WATER};

void Cell::invalidate(std::string reason) {
  log.info(lu::yellow("CELL"),
           fmt::format("cell ({}.{}) invalidate [{}]",
                       lu::green(fmt::format("{}", x)),
                       lu::green(fmt::format("{}", y)),
                       lu::magenta(reason)));
  invalidate();
}
