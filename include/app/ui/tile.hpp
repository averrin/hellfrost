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

#include <lss/game/cell.hpp>
#include <lss/game/region.hpp>
#include <lss/game/terrain.hpp>
#include <lss/game/door.hpp>
#include <lss/game/item.hpp>
#include <lss/game/enemy.hpp>
#include <lss/utils.hpp>

using Color = LibColor::Color;

class Tile {
public:
  // std::pair<int, int> spriteIndex;
  std::vector<std::shared_ptr<sf::Sprite>> sprites;
  std::shared_ptr<Cell> cell;
  bool damaged = false;
  sf::Color bgColor = sf::Color(33, 33, 23);
  bool hasBackground = false;
  sf::Vector2f pos;
  int bgLayer = 1;
};

#endif // __TILE_H_
