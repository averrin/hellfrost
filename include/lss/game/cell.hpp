#ifndef __CELL_H_
#define __CELL_H_
#include <algorithm>
#include <cmath>
#include <lss/deps.hpp>
#include <lss/game/content/traits.hpp>
#include <lss/utils.hpp>
#include <memory>
#include <set>
#include <vector>

const float TORCH_DISTANCE = 4.5f;

class Room;
class Object;
class Trigger;
struct CellSpec {
  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(name, seeThrough, passThrough);
  };
  template <class Archive> void load(Archive &ar) {
    ar(name, seeThrough, passThrough);
  };
  std::string name;

  bool passThrough = false;
  bool seeThrough = false;

  std::vector<Trait> canPassTraits;

  friend bool operator==(CellSpec &t1, const CellSpec &t2) {
    return t1.name == t2.name;
  }
  friend bool operator!=(CellSpec &t1, const CellSpec &t2) {
    return t1.name != t2.name;
  }
  friend bool operator<(const CellSpec &t1, const CellSpec &t2) {
    return t1.name < t2.name;
  }
};

namespace CellType {
const CellSpec EMPTY = CellSpec{"CELL_EMPTY", true, true};
const CellSpec UNKNOWN = CellSpec{"CELL_UNKNOWN", false, false};
const CellSpec FLOOR = CellSpec{"CELL_FLOOR", true, true};
const CellSpec GROUND = CellSpec{"CELL_GROUND", true, true};
const CellSpec ROAD = CellSpec{"CELL_ROAD", true, true};
const CellSpec WALL = CellSpec{"CELL_WALL", false, false};
const CellSpec ROOF = CellSpec{"CELL_ROOF", true, false};
const CellSpec DOWNSTAIRS = CellSpec{"CELL_DOWNSTAIRS", true, true};
const CellSpec UPSTAIRS = CellSpec{"CELL_UPSTAIRS", true, false};
const CellSpec WATER =
    CellSpec{"CELL_WATER", false, true, {Traits::FLY, Traits::CAN_SWIM}};
const CellSpec VOID = CellSpec{"CELL_VOID", false, true, {Traits::FLY}};
}; // namespace CellType

enum class VisibilityState { UNKNOWN, SEEN, VISIBLE };
// enum class CellFeature { BLOOD, CAVE, FROST, MARK1, MARK2, ACID, CORRUPT, DUNGEON, POI, WIPE };

class Cell {
  ll::Logger &log = ll::Logger::getInstance();

public:
  static std::vector<CellSpec> types;
  Cell(CellSpec t) : type(t) {}
  Cell(int __x, int __y, CellSpec t)
      : x(__x), y(__y), type(t), passThrough(type.passThrough),
        seeThrough(type.seeThrough), _x(__x), _y(__y), z(0), _z(0) {}
  Cell(int __x, int __y, CellSpec t, std::vector<std::string> tg)
      : x(__x), y(__y), type(t), passThrough(type.passThrough),
        seeThrough(type.seeThrough), _x(__x), _y(__y), z(0), _z(0) {
    tags.tags = tg;
  }
  CellSpec type;
  VisibilityState visibilityState = VisibilityState::UNKNOWN;
  bool illuminated = false;
  void setIlluminated(bool v) {
    if (illuminated != v) {
      illuminated = v;
      invalidate();
    }
  }
  Tags tags = Tags{};

  std::vector<std::shared_ptr<Trigger>> triggers =
      std::vector<std::shared_ptr<Trigger>>{};
  std::set<std::shared_ptr<Object>> lightSources;
  std::shared_ptr<Object> nearestLightEmitter;
  std::shared_ptr<Room> room;

  int x = 0;
  int y = 0;
  int z = 0;
  int _x = 0;
  int _y = 0;
  int _z = 0;
  int getId() { return x * 1000000 + y * 1000 + z; }
  std::string getSId() {
    return fmt::format("{}{}.{}.{}", type.name.front(), x, y,
                       z);
  }
  std::pair<int, int> anchor;

  bool passThrough = false;
  bool seeThrough = false;

  bool damaged = true;
  void invalidate(std::string reason);
  void invalidate() { damaged = true; }
  void setVisibilityState(VisibilityState vs) {
    if (visibilityState != vs) {
      visibilityState = vs;
      invalidate();
    }
  }

  bool canPass(std::vector<Trait> traits) {
    if (passThrough)
      return true;
    std::sort(traits.begin(), traits.end());
    std::sort(type.canPassTraits.begin(), type.canPassTraits.end());

    std::vector<Trait> v_intersection;

    std::set_intersection(traits.begin(), traits.end(),
                          type.canPassTraits.begin(), type.canPassTraits.end(),
                          std::back_inserter(v_intersection));
    return v_intersection.size() != 0;
  }

  static const int DEFAULT_LIGHT = 60;
  static const int MINIMUM_LIGHT = 5;
  int illumination = DEFAULT_LIGHT;

  void setIllumination(int v) {
    if (illumination != v) {
      illumination = v;
      invalidate();
    }
  }
};

typedef std::vector<std::shared_ptr<Cell>> CellRow;
typedef std::vector<CellRow> Cells;

#endif // __CELL_H_
