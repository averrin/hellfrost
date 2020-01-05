#ifndef __TILE_H_
#define __TILE_H_

#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <memory>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <librandom/random.hpp>
#include <libcolor/libcolor.hpp>

using Color = LibColor::Color;

namespace hellfrost {
class Tile {
public:
  std::vector<std::shared_ptr<sf::Sprite>> sprites{};
  bool damaged = false;
  sf::Color bgColor = sf::Color(33, 33, 23);
  bool hasBackground = false;
  sf::Vector3f pos{};
  int id() { return pos.x * 1000 * 100 + pos.y * 100 + pos.z; }
};
}

#endif // __TILE_H_
