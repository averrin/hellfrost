#ifndef __TERRAIN_H_
#define __TERRAIN_H_
#include <lss/game/object.hpp>

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

#endif // __TERRAIN_H_
