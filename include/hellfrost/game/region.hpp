#ifndef __REGION_H_
#define __REGION_H_
#include <hellfrost/deps.hpp>
#include <hellfrost/game/cell.hpp>
#include <hellfrost/generator/mapUtils.hpp> //move to game

namespace hellfrost {
class Region {
  std::map<int, std::shared_ptr<Cells>> cache{};
  std::mutex mutex;
  LibLog::Logger log = LibLog::Logger(fmt::color::blue, "REGION");

public:
  bool damaged = true;
  std::shared_ptr<Cells> cells = std::make_shared<Cells>();
  Region() : cells(std::make_shared<Cells>()) {}
  Region(std::shared_ptr<Cells> c)
      : cells(c), width(c->front().size()), height(c->size()) {
    update();
    // getCells();
  }
  void invalidate(bool recursive = false) {
    const std::lock_guard<std::mutex> lock(mutex);
    damaged = true;
    cache.clear();
    if (recursive) {
      for (auto& r : regions) {
        r->invalidate(true);
      }
    }
  }

  void update();
  Cell getCell(int x, int y, int z);
  // std::shared_ptr<Cells> getCells();
  std::shared_ptr<Cells> getCells(int _z);
  bool isInside(int _x, int _y, int _z);

  void resize(int w, int h, CellSpec type = CellType::EMPTY);
  Cells resize(Cells src, int w, int h, CellSpec type = CellType::EMPTY);

  Cells paste(Cells src, int x, int y, Cells dst);
  void place(std::shared_ptr<Region> region, int _x, int _y, int _z);

  bool active = true;

  std::string id;
  std::string name = "";
  int width = 0;
  int height = 0;

  int x = 0;
  int y = 0;
  int z = 0;

  std::vector<std::shared_ptr<Region>> regions{};

  static std::shared_ptr<Region> random(int wMin = 3, int wMax = 11,
                                        int hMin = 3, int hMax = 15,
                                        CellSpec type = CellType::FLOOR) {
    auto gen = entt::service_locator<R::Generator>::get().lock();
    auto rh = gen->R(hMin, hMax);
    auto rw = gen->R(wMin, wMax);

    auto cells = mapUtils::fill(rh, rw, type);
    auto room = std::make_shared<Region>(std::make_shared<Cells>(cells));
    return room;
  }

  // void printRegion() {
  //   getCells();
  //   fmt::print("{}x{} ({}x{})\n", width, height, cells->front().size(),
  //              cells->size());
  //   fmt::print("r: {}, cache: {}x{}\n", regions.size(), cache->front().size(),
  //              cache->size());
  //   fmt::print(dump());
  // }
  // static void printRegion(std::shared_ptr<Region> r) {
  //   fmt::print(dump(r->getCells()));
  // }

  std::string dump(int _z) { return dump(getCells(_z)); }
  static std::string dump(std::shared_ptr<Cells> _cells) {
    std::string out = "";
    for (auto r : *_cells) {
      for (auto _c : r) {
          if(!_c) {
              out += "X";
              continue;
          }
          auto c = *_c;

        if (c->type == CellType::WALL) {
          out += "#";
        } else if (c->type == CellType::GROUND) {
          out += ".";
        } else if (c->type == CellType::FLOOR) {
          out += ".";
        } else if (c->type == CellType::EMPTY) {
          out += "e";
        } else if (c->type == CellType::UNKNOWN) {
          out += " ";
        } else if (c->type == CellType::WATER) {
          out += "=";
        } else {
          out += "?";
        }
      }
      out += "\n";
    }
    return out;
  }
};
}; // namespace hellfrost

#endif // __REGION_H_
