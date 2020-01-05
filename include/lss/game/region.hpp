#ifndef __REGION_H_
#define __REGION_H_
#include <mutex>

#include <lss/game/cell.hpp>
#include <lss/generator/mapUtils.hpp>

enum class RegionType {
  EXTERIOR,
  HALL,
  PASSAGE,
  CAVERN,
  CAVE,
  DUNGEON,
};

enum class RegionFeature {
  CAVE_PASSAGE,
  RIVER,
  TORCHES,
  STATUE,
  ALTAR,
  LAKE,
  VOID,
  ICE,
  HEAL,
  MANA,
  TREASURE_SMALL,
  CORRUPT,
  BONES_FIELD,
  CAVE,
  DUNGEON,
};

struct RegionSpec {
  std::string name;
  int threat = 0;
  std::vector<RegionFeature> features;
  std::vector<CellFeature> cellFeatures;
  // std::shared_ptr<Cell> enterCell;
  CellSpec floor = CellType::FLOOR;
  RegionType type;
};

class Region {
  std::shared_ptr<Cells> cells;
  bool damaged = true;
  std::shared_ptr<Cells> cache;

  std::mutex resize_mutex;

public:
  Region() : cells(std::make_shared<Cells>()) {}
  Region(std::shared_ptr<Cells> c)
      : cells(c), width(c->front().size()), height(c->size()) {
    update();
  }
  void invalidate() { damaged = true; }

  RegionSpec type;
  bool active = true;

  std::string id;
  std::string name;

  int height = 0;
  int width = 0;

  int x = 0;
  int y = 0;
  int z = 0;

  int threat = 0;
  std::vector<RegionFeature> features{};
  bool hasFeature(RegionFeature f) {
    return std::find(features.begin(), features.end(), f) != features.end();
  }

  void printRegion() {
    getCells();
    fmt::print("{}x{} ({}x{})\n", width, height, cells->front().size(),
               cells->size());
    fmt::print("r: {}, cache: {}x{}\n", regions.size(), cache->front().size(),
               cache->size());
    fmt::print(dump());
  }
  void printRegion(std::shared_ptr<Region> r) {
    fmt::print(dump(r->getCells()));
  }

    void update();

  std::string dump() { return dump(getCells()); }
  std::string dump(std::shared_ptr<Cells> _cells) {
    std::string out = "";
    for (auto r : *_cells) {
      for (auto c : r) {
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

  std::vector<std::shared_ptr<Region>> regions;

  std::shared_ptr<Cell> getRandomCell() { return getRandomCell(getCells()); }
  std::shared_ptr<Cell> getRandomCell(std::shared_ptr<Cells> src) {
    return src->at(rand() % height)[rand() % width];
  }

  void place(std::shared_ptr<Region> region, int _x, int _y);

  bool placeInside(std::shared_ptr<Region> region, bool smart = false);

  std::optional<std::shared_ptr<Cell>> getCell(int _x, int _y, int _z);

  std::shared_ptr<Cells> getCells();
  bool isInside(int _x, int _y, int _z);

  void updateCell(int _x, int _y, int _z, CellSpec type,
                  std::vector<CellFeature> features);

  std::shared_ptr<Cells> getCells(int _z);
  std::optional<std::shared_ptr<Cell>> getRandomCell(CellSpec type);

  void resize(int w, int h, CellSpec type = CellType::EMPTY);
  Cells resize(Cells src, int w, int h, CellSpec type = CellType::EMPTY);

  Cells paste(Cells src, int x, int y, Cells dst);

  void fill(int h, int w, CellSpec type);

  static std::shared_ptr<Region>
  createRandomRegion(int hMax = 11, int hMin = 3, int wMax = 15, int wMin = 3,
                     CellSpec type = CellType::FLOOR) {
    auto rh = R::Z(hMin, hMax);
    auto rw = R::Z(wMin, wMax);

    auto cells = mapUtils::fill(rh, rw, type);
    auto room = std::make_shared<Region>(std::make_shared<Cells>(cells));
    return room;
  }

  void flatten();

  int countNeighbors(std::shared_ptr<Cell> cell,
                     std::function<bool(std::shared_ptr<Cell>)> test);

  std::vector<std::shared_ptr<Cell>> getNeighbors(std::shared_ptr<Cell> cell);

  std::optional<std::shared_ptr<Cell>> getCell(std::shared_ptr<Cell> cc,
                                               Direction d);

  void walls(CellSpec type);
};

#endif // __REGION_H_
