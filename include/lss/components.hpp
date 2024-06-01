#ifndef __COMPONENTS_H_
#define __COMPONENTS_H_
#include "lss/game/cell.hpp"
#include <lss/deps.hpp>
#include <lss/game/itemSpec.hpp>
#include <lss/game/light.hpp>
#include <string>
#include <vector>

namespace hellfrost {
struct ingame {
  friend class cereal::access;
  template <class Archive> void serialize(Archive &ar) { ar(); };
};

struct meta {
  std::string name = "";
  std::string description = "";
  std::string id = "";

  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(name, description, id);
  };
  template <class Archive> void load(Archive &ar) {
    ar(name, description, id);
  };
};
struct ineditor {
  std::string name = "";
  std::vector<std::string> folders;
  std::string icon = "";
  bool selected = false;

  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(name, folders, icon, selected);
  };
  template <class Archive> void load(Archive &ar) {
    ar(name, folders, icon, selected);
  };
};

struct position {
  int x = 0;
  int y = 0;
  int z = 0;
  bool movable = true;

  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(x, y, z, movable);
  };
  template <class Archive> void load(Archive &ar) { ar(x, y, z, movable); };
};

struct visible {
  std::string type = "";
  std::string sign = "";
  bool hidden = false;
  bool seeThrough = true;
  bool passThrough = true;

  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(type, sign, hidden, seeThrough, passThrough);
  };
  template <class Archive> void load(Archive &ar) {
    ar(type, sign, hidden, seeThrough, passThrough);
  };
};

struct pickable {
  ItemCategory category;
  bool identified = false;
  int count;
  std::string unidName;

  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(category, identified, count, unidName);
  };
  template <class Archive> void load(Archive &ar) {
    ar(category, identified, count, unidName);
  };
};
struct wearable {
  WearableType wearableType = WearableType::INVALID;
  int durability = -1;

  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(wearableType, durability);
  };
  template <class Archive> void load(Archive &ar) {
    ar(wearableType, durability);
  };
};

struct glow {
  float distance;
  LightType type;
  bool stable = false; // depricated
  int bright = 80;
  int flick = 5;
  bool passive = false;
  int pulse = 0;

  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(distance, type, bright, flick, passive, pulse);
  };
  template <class Archive> void load(Archive &ar) {
    ar(distance, type, bright, flick, passive, pulse);
  };
};

struct cell {
  std::shared_ptr<Cell> cell = nullptr;
};

struct room {
  std::shared_ptr<Room> room;
};

struct children {
  std::vector<entt::entity> children;
};
struct size {
  int width = 0;
  int height = 0;
};
struct wall {
};
struct tags {
  std::vector<std::string> tags;
  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(tags);
  };
  template <class Archive> void load(Archive &ar) {
    ar(tags);
  };
};

struct renderable {
  std::string spriteKey = "UNKNOWN";
  std::string fgColor = "#fff";
  bool hasBg = false;
  std::string bgColor = "#ffffff00";
  bool hasBorder = false;
  std::string borderColor = "#ffffff00";
  bool hidden = false;
  int zIndex = 0;
  std::string fgLayer = "entities";
  std::string bgLayer = "entitiesBg";
  std::string brdLayer = "entitiesBrd";

  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(spriteKey, fgColor, hasBg, bgColor, hasBorder, borderColor, hidden,
       zIndex, fgLayer, bgLayer, brdLayer);
  };
  template <class Archive> void load(Archive &ar) {
    ar(spriteKey, fgColor, hasBg, bgColor, hasBorder, borderColor, hidden,
     zIndex, fgLayer ,bgLayer, brdLayer);
  };
};

struct overwrite {
  std::string spriteKey = "UNKNOWN";
  std::string fgColor = "#fff";
  bool hasBg = false;
  std::string bgColor = "#ffffff00";
  bool hasBorder = false;
  std::string borderColor = "#ffffff00";
  bool hidden = false;
  int zIndex = 0;
};

struct creature {
};
struct obstacle {
  bool passThrough = false;
  bool seeThrough = false;
  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(passThrough, seeThrough);
  };
  template <class Archive> void load(Archive &ar) {
    ar(passThrough, seeThrough);
  };
};
struct player {};
struct vision {
  float distance = 0;
  friend class cereal::access;
  template <class Archive> void save(Archive &ar) const {
    ar(distance);
  };
  template <class Archive> void load(Archive &ar) {
    ar(distance);
  };
};

// TODO
struct usable {};
struct consumable {};
struct destructable {};
}; // namespace hellfrost

#endif // __COMPONENTS_H_
