#ifndef __MAPUTILS_H_
#define __MAPUTILS_H_

#include "lss/game/enemy.hpp"
// #include "lss/game/location.hpp"

class mapUtils {
public:
  static Cells fill(int h, int w, CellSpec type) {
    Cells cells;
    for (auto r = 0; r < h; r++) {
      std::vector<std::shared_ptr<Cell>> row;
      for (auto c = 0; c < w; c++) {
        auto cell = std::make_shared<Cell>(c, r, type);
        cell->visibilityState = VisibilityState::UNKNOWN;

        row.push_back(cell);
      }

      cells.push_back(row);
    }
    return cells;
  }

    static std::shared_ptr<Enemy> makeEnemy(
      // std::shared_ptr<Location> location,
                                          std::shared_ptr<Cell> c,
                                          EnemySpec type) {
    if (c == nullptr) {
      throw std::runtime_error("nullptr cell for enemy");
    }
    auto enemy = std::make_shared<Enemy>(type);
    enemy->setCurrentCell(c);
    // enemy->currentLocation = location;
    return enemy;
  }

  static void makeFloor(std::shared_ptr<Cell> cell) {
    cell->type = CellType::FLOOR;
    cell->seeThrough = true;
    cell->passThrough = true;
  }

  static void makeFloor(std::shared_ptr<Cell> cell,
                        std::vector<CellFeature> features) {
    cell->features = features;
    makeFloor(cell);
  }

  static void updateCell(std::shared_ptr<Cell> cell, CellSpec type,
                         std::vector<CellFeature> features) {
    cell->type = type;
    cell->seeThrough = type.seeThrough;
    cell->passThrough = type.passThrough;
    cell->features = features;
  }

  static void updateCell(std::shared_ptr<Cell> cell, CellSpec type) {
    updateCell(cell, type, cell->features);
  }
};
#endif // __MAPUTILS_H_
