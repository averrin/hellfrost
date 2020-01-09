#ifndef __TILESET_H_
#define __TILESET_H_
#include <hellfrost/deps.hpp>

namespace hellfrost {
struct TileSet {
  std::vector<std::string> maps;
  std::pair<int, int> size;
  int gap;
  std::map<std::string, std::map<std::string, std::array<int, 3>>> sprites;
  std::map<std::string, std::map<std::string, std::string>> colors;

  friend class cereal::access;
  template <class Archive> void serialize(Archive &ar) {
    ar(CEREAL_NVP(maps), CEREAL_NVP(size), CEREAL_NVP(gap), CEREAL_NVP(sprites), CEREAL_NVP(colors));
  };
};
}; // namespace hellfrost

#endif // __TILESET_H_
