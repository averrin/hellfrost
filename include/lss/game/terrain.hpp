#ifndef __TERRAIN_H_
#define __TERRAIN_H_
#include "lss/game/object.hpp"

class Creature;
struct TerrainSpec {
  friend class cereal::access;
  template<class Archive>
  void save(Archive & ar) const {
      ar( name, seeThrough, passThrough, apLeft, destructable, light, sign );
  };
  template<class Archive>
  void load(Archive & ar) {
      ar( name, seeThrough, passThrough, apLeft, destructable, light, sign );
  };
  std::string name;
  bool seeThrough;
  bool passThrough;

  int apLeft = -1;
  bool destructable = true;

  LightSpec light;
  std::string sign;

  friend bool operator==(TerrainSpec &t1, const TerrainSpec &t2) {
    return t1.name == t2.name;
  }
  friend bool operator<(TerrainSpec &t1, const TerrainSpec &t2) {
    return t1.name < t2.name;
  }
  friend bool operator<(const TerrainSpec &lhs, const TerrainSpec &rhs) {
    return lhs.name < rhs.name;
  }
};

class Terrain : public Object {
public:
  Terrain(TerrainSpec t) : Object(), type(t) {
    seeThrough = t.seeThrough;
    passThrough = t.passThrough;
    zIndex = 2;
    light = t.light;
    apLeft = t.apLeft;
    destructable = t.destructable;
  }
  Terrain(TerrainSpec t, std::vector<std::shared_ptr<Trigger>> ts) : Object(ts), type(t) {
    seeThrough = t.seeThrough;
    passThrough = t.passThrough;
    zIndex = 2;
    light = t.light;
    apLeft = t.apLeft;
    destructable = t.destructable;
  }
  Terrain(TerrainSpec t, int z) : Object(), type(t) {
    seeThrough = t.seeThrough;
    passThrough = t.passThrough;
    zIndex = z;
    light = t.light;
    apLeft = t.apLeft;
    destructable = t.destructable;
  }
  TerrainSpec type;

  // bool interact(std::shared_ptr<Creature> actor);

  std::optional<LightSpec> getGlow() override {
    if (light.distance <= 0) {
      return std::nullopt;
    }
    return std::make_optional<LightSpec>(light);
  };
};

class UsableTerrain: public Terrain {
  public:
    UsableTerrain(TerrainSpec t) : Terrain(t) {}
};

#endif // __TERRAIN_H_
