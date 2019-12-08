#ifndef __REGION_H_
#define __REGION_H_

#include <lss/game/cell.hpp>
#include <lss/game/location.hpp>

class Region {
public:
  Cells cells;
  std::pair<int, int> position;
  bool active = false;
  int z = 0;
  std::shared_ptr<Location> location;

  void fill(int h, int w, CellSpec type) {
    cells.clear();
    for (auto r = 0; r < h; r++) {
      std::vector<std::shared_ptr<Cell>> row;
      for (auto c = 0; c < w; c++) {
        auto cell = std::make_shared<Cell>(c, r, type);
        cell->visibilityState = VisibilityState::UNKNOWN;
        if (type == CellType::FLOOR) {
          cell->seeThrough = true;
          cell->passThrough = true;
        }

        cell->anchor = position;
        row.push_back(cell);
      }

      cells.push_back(row);
    }
  }

  void walls(CellSpec type) {
    auto i = 0;
    for (auto row : cells) {
      if (i == 0 || i == cells.size() - 1) {
        for (auto c : row) {
          c->type = type;
        }
      } else {
        auto n = 0;
        for (auto c : row) {
          if (n == 0 || n == row.size() - 1) {
            c->type = type;
          }
          n++;
        }
      }
      i++;
    }
  }
};


#endif // __REGION_H_
