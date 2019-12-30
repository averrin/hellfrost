#ifndef __TILE_H_
#define __TILE_H_

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <fstream>
#include <libcolor/libcolor.hpp>
#include <librandom/random.hpp>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <lss/game/cell.hpp>
#include <lss/game/door.hpp>
#include <lss/game/enemy.hpp>
#include <lss/game/item.hpp>
#include <lss/game/region.hpp>
#include <lss/game/terrain.hpp>
#include <lss/utils.hpp>

using Color = LibColor::Color;

class Tile {
public:
  std::vector<std::shared_ptr<sf::Sprite>> sprites{};
  std::shared_ptr<Cell> cell;
  bool damaged = false;
  sf::Color bgColor = sf::Color(33, 33, 23);
  bool hasBackground = false;
  sf::Vector3f pos{};
  int id() { return pos.x * 1000 * 100 + pos.y * 100 + pos.z; }
};

#endif // __TILE_H_
