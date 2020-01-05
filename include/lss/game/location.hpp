#ifndef __LOCATION_H_
#define __LOCATION_H_
#include <cmath>
#include <iostream>
#include <map>
#include <optional>
#include <mutex>

#include <micropather/micropather.h>

#include <lss/components.hpp>

#include <lss/game/cell.hpp>
#include <lss/game/fov.hpp>
#include <lss/game/object.hpp>
#include <lss/game/region.hpp>

class Player;
class AiManager;
class Location : public Region,
                 public micropather::Graph
{
public:
  std::shared_ptr<entt::registry> registry = std::make_shared<entt::registry>();
  LibLog::Logger &log = LibLog::Logger::getInstance();
  Location() : Region() {}
  ~Location();
  Objects objects{};
  int depth = 0;
  std::mutex create_mutex;

  entt::entity addTerrain(std::string typeKey, std::shared_ptr<Cell> cell);
  entt::entity addItem(std::string prefabId, std::shared_ptr<Cell> cell);
  entt::entity addItem(std::string prefabId, int count, std::shared_ptr<Cell> cell);

  template <typename T>
  void addObject(std::shared_ptr<T> o, std::shared_ptr<T> cc) {
    o->setCurrentCell(cc);
    addObject<T>(o);
  }

  template <typename T> void addObject(std::shared_ptr<T> o) {
    if (o->currentCell == nullptr) {
      throw std::runtime_error("you cannot add object without currentCell");
      return;
    }
    objects.push_back(o);
  }

  void removeObject(std::shared_ptr<Object> o) {
    objects.erase(std::remove(objects.begin(), objects.end(), o));
    o->removeCell();
  }

  void updateView(std::shared_ptr<Player>);
  void updateLight(std::shared_ptr<Player>);
  void reveal();
  void enter(std::shared_ptr<Player>, std::shared_ptr<Cell>);
  void leave(std::shared_ptr<Player>);
  Objects getObjects(std::shared_ptr<Cell>);

  // std::vector<std::shared_ptr<Cell>> getNeighbors(Cell *cell) {
  //   std::vector<std::shared_ptr<Cell>> nbrs;
  //   if (cell->x > 0) {
  //     if (cell->y > 0) {
  //       nbrs.push_back(cells[cell->y - 1][cell->x - 1]);
  //       nbrs.push_back(cells[cell->y - 1][cell->x]);
  //       nbrs.push_back(cells[cell->y][cell->x - 1]);

  //       if (cell->x < int(cells.front().size() - 1)) {
  //         nbrs.push_back(cells[cell->y - 1][cell->x + 1]);
  //         nbrs.push_back(cells[cell->y][cell->x + 1]);
  //       }
  //     }
  //     if (cell->y < int(cells.size() - 1)) {
  //       if (cell->x < int(cells.front().size() - 1)) {
  //         nbrs.push_back(cells[cell->y + 1][cell->x + 1]);
  //         nbrs.push_back(cells[cell->y + 1][cell->x - 1]);
  //       }
  //       nbrs.push_back(cells[cell->y + 1][cell->x]);
  //     }
  //   }
  //   return nbrs;
  // }

  std::vector<std::shared_ptr<Cell>> getVisible(std::shared_ptr<Cell> start,
                                                float distance);

  void invalidateVisibilityCache(std::shared_ptr<Cell> cell);

  std::map<std::pair<std::shared_ptr<Cell>, float>,
           std::vector<std::shared_ptr<Cell>>>
      visibilityCache;

  void invalidate(std::string reason);
  void invalidate() { needUpdateLight = true; }

  std::vector<std::shared_ptr<Cell>> getLine(std::shared_ptr<Cell> c1,
                                             std::shared_ptr<Cell> c2);

  std::optional<Direction> getDirFromCell(std::shared_ptr<Cell> start,
                                          std::shared_ptr<Cell> end) {
    std::optional<Direction> dir = std::nullopt;
    if (start == nullptr || end == nullptr) {
      return dir;
    }
    if (start->x == end->x && start->y < end->y) {
      dir = Direction::N;
    } else if (start->x == end->x && start->y > end->y) {
      dir = Direction::S;
    } else if (start->x < end->x && start->y == end->y) {
      dir = Direction::W;
    } else if (start->x > end->x && start->y == end->y) {
      dir = Direction::E;
    } else if (start->x < end->x && start->y < end->y) {
      dir = Direction::NW;
    } else if (start->x > end->x && start->y < end->y) {
      dir = Direction::NE;
    } else if (start->x < end->x && start->y > end->y) {
      dir = Direction::SW;
    } else if (start->x > end->x && start->y > end->y) {
      dir = Direction::SE;
    }
    return dir;
  }

  float getDistance(std::shared_ptr<Cell> c, std::shared_ptr<Cell> cc) {
    return sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
  }
  bool needUpdateLight = true;
  int apAccomulator = 0;

private:

  float LeastCostEstimate(void *stateStart, void *stateEnd) override;
  void AdjacentCost(void *state,
                    MP_VECTOR<micropather::StateCost> *adjacent) override;
  void PrintStateInfo(void *state) override;
};

#endif // __LOCATION_H_
