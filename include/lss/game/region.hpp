#ifndef __REGION_H_
#define __REGION_H_

#include <lss/game/cell.hpp>
#include <lss/generator/mapUtils.hpp>

enum class RegionType {
  EXTERIOR,
  HALL,
  PASSAGE,
  CAVERN,
  CAVE,
} enum class RegionFeature {
  CAVE,
  DUNGEON
};

class Region {
public:
  Cells cells;
  std::pair<int, int> position;
  bool active = false;

  int height = 0;
  int width = 0;

  int x = 0;
  int y = 0;
  int z = 0;

  RegionType type = RegionType::HALL;
  int threat = 0;
  std::vector<RegionFeature> features{};
  std::vector<std::shared_ptr<Region>> regions;

  Cells getCells() {
    auto result = cells;
    int rbx = 0;
    int rby = 0;
    for (auto r : regions) {
      rbx = max(rbx, r.x+r.width);
      rby = max(rby, r.y+r.height);
    }
    result = resize(result, rbx, rby);
    width = rbx;
    height = rby;
    for (auto r : regions) {
      paste(r->cells, r.x, r.y, result);
    }
    return result;
  }
    auto getRandomCell() {
      return getRandomCell(getCells());
    }

    auto getRandomCell(Cells src) {
      return src[rand()%height][rand()%width];
    }

  Cells resize(Cells src, int w, int h) {
    auto dst = Cells{};
    for (int y = 0; y < h; y++) {
      if (dst.size() <= y)
        dst.push_back(std::vector<std::shared_ptr<Cell>>{});
      for (int x = 0; x < w; x++) {
        if (dst[y].size() <= x)
          dst[y].push_back(std::make_shared<Cell>(CellType::EMPTY));
        else
          dst[y][x] = src[y][x];
      }
    }
    return dst;
  }

  Cells paste(Cells src, int x, int y, Cells dst) {
    int bh = dst.size();
    int bw = dst.front().size();
    for (auto r = y; r < int(src->cells.size() + y) && r < bh; r++) {
      for (auto c = x; c < int(src.front().size() + x) && c < bw; c++) {
        auto cell = src[r - y][c - x];
        if (cell->type == CellType::EMPTY)
          continue;
        dst[r][c] = cell;
        dst[r][c]->x = c;
        dst[r][c]->y = r;
      }
    }
    return dst;
  }

  void fill(int h, int w, CellSpec type) {
    cells.clear();
    for (auto r = 0; r < h; r++) {
      std::vector<std::shared_ptr<Cell>> row;
      for (auto c = 0; c < w; c++) {
        auto cell = std::make_shared<Cell>(c, r, type);
        cell->visibilityState = VisibilityState::UNKNOWN;
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
          mapUtils::updateCell(c, type);
        }
      } else {
        auto n = 0;
        for (auto c : row) {
          if (n == 0 || n == row.size() - 1) {
            mapUtils::updateCell(c, type);
          }
          n++;
        }
      }
      i++;
    }
  }
};

#endif // __REGION_H_
