#ifndef __TILE_H_
#define __TILE_H_

#include <utility>
#include <vector>
#include <map>
#include <memory>
#include <SFML/Graphics.hpp>
#include <librandom/random.hpp>

#include <game/cell.hpp>
#include <app/ui/tilemap.hpp>

class Tile {
public:
  // std::pair<int, int> spriteIndex;
  std::vector<std::shared_ptr<sf::Sprite>> sprites;
  std::shared_ptr<Cell> cell;
  bool damaged = false;
  sf::Color bgColor = sf::Color(33, 33, 23);
  sf::Vector2f pos;
};

class Viewport {
  const std::pair<int, int> SPRITE_MAP_SIZE = std::make_pair(16, 16);
  const int SPRITE_GAP_SIZE = 0;

  sf::IntRect getTileRect(int x, int y) {
    return sf::IntRect((SPRITE_TILE_SIZE.first + SPRITE_GAP_SIZE) * y,
                       (SPRITE_TILE_SIZE.second + SPRITE_GAP_SIZE) * x,
                       SPRITE_TILE_SIZE.first, SPRITE_TILE_SIZE.second);
  }


  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();

public:
  const std::pair<int, int> SPRITE_TILE_SIZE = std::make_pair(20, 20);
  std::map<std::string, std::shared_ptr<Tile>> tilesCache;

public:
  int width = 60;
  int height = 50;
  std::pair<int, int> position;
  int z = 0;
  sf::Texture tilesTexture;
  std::vector<std::shared_ptr<Region>> regions;
  std::pair<std::shared_ptr<Cell>, int> getCell(int x, int y, int z) {
    std::shared_ptr<Cell> cell(nullptr);
    for (auto region : regions) {
      if (region->active && region->z == z &&
          x >= region->position.first &&
          x < (int)(region->position.first + region->cells[0].size()) &&
          y >= region->position.second &&
          y < (int)(region->position.second + region->cells.size())) {
        cell = region->cells[y - region->position.second]
                            [x - region->position.first];
      }
    }
    if (z > 0 && cell == nullptr) {
      return getCell(x,y,z-1);
    }
    return std::make_pair(cell, z);
  }

  std::shared_ptr<Tile> getTile(int x, int y, int z) {
    x += position.first;
    y += position.second;

    auto [cell, rz] = getCell(x, y, z);
    if (cell == nullptr) {
      cell = std::make_shared<Cell>(x, y, CellType::UNKNOWN);
    }

    auto coords = fmt::format("{}.{}.{}", x, y, rz);
    auto t = tilesCache.find(coords);
    if (t != tilesCache.end()) return t->second;


    auto tile = std::make_shared<Tile>();
    auto fgColor = sf::Color(220,220,220);
    auto sprite = std::make_shared<sf::Sprite>();
    sprite->setTexture(tilesTexture);
    tile->sprites.push_back(sprite);

    std::pair<int, int> sprite_spec;
    if (cell->type == CellType::GROUND) {
      sprite_spec = TileMap::GRASS;
      fgColor = sf::Color(120,150,60);
    } else if (cell->type == CellType::FLOOR) {
      sprite_spec = TileMap::FLOOR;
      fgColor = sf::Color(95,85,65);

      if (gen->R(0, 100) < 10) {
        auto s = std::make_shared<sf::Sprite>();
        s->setTexture(tilesTexture);
        s->setTextureRect(getTileRect(0, 1));
        s->setColor(sf::Color(rand()%256, rand()%256, rand()%256));
        tile->sprites.push_back(s);
        tile->bgColor = sf::Color(rand()%256, rand()%256, rand()%256, rand()%256);
      }
    } else if (cell->type == CellType::WALL) {
      sprite_spec = getWallSpec(cell);
      // sprite_spec = gen->R(TileMap::WALL);
      // fgColor = sf::Color(190,185,170);
      fgColor = sf::Color(125,115,105);
    } else if (cell->type == CellType::ROOF) {
      sprite_spec = std::make_pair<int, int>(17, 1);
    } else if (cell->type == CellType::UNKNOWN) {
      sprite_spec = TileMap::UNKNOWN;
    }

    auto src = getTileRect(sprite_spec.first, sprite_spec.second);
    sprite->setTextureRect(src);
    sprite->setColor(fgColor);

    // tile->bgColor = sf::Color(255, 255, 255, rand()%256);


    auto scale = 0.8f;
    tile->pos = sf::Vector2f((cell->anchor.first + cell->x) * (SPRITE_TILE_SIZE.first),
          (cell->anchor.second + cell->y) * (SPRITE_TILE_SIZE.second));
    for (auto s : tile->sprites) {
      s->setPosition(tile->pos);
    }
    tile->cell = cell;
    // tile->spriteIndex = sprite_spec;
    tilesCache[coords] = tile;
    return tile;
  }

std::pair<int, int> getCoords(std::shared_ptr<Cell> cell) {
    return std::make_pair<int, int>(
      cell->anchor.first + cell->x,
      cell->anchor.second + cell->y
    );
}

std::pair<int, int> getWallSpec(std::shared_ptr<Cell> cell) {
    auto coords = getCoords(cell);
    auto l = getCell(coords.first-1, coords.second, z);
    auto r = getCell(coords.first+1, coords.second, z);
    auto t = getCell(coords.first, coords.second-1, z);
    auto b = getCell(coords.first, coords.second+1, z);

    auto spec = TileMap::WALL_H;
    if (t != nullptr && b != nullptr && (t->type == CellType::WALL || b->type == CellType::WALL)) {
        spec = TileMap::WALL_V;
    }

    if (t != nullptr && b != nullptr && t->type == CellType::WALL && b->type == CellType::WALL &&
      l != nullptr && r != nullptr && l->type == CellType::WALL && r->type == CellType::WALL) {
          spec = TileMap::WALL_CROSS;
          return spec;
    }

    if (l != nullptr && r != nullptr && l->type == CellType::WALL && r->type == CellType::WALL) {
        spec = TileMap::WALL_H;
        if (b != nullptr && b->type == CellType::WALL) {
          spec = TileMap::WALL_HB;
        }
        if (t != nullptr && t->type == CellType::WALL) {
          spec = TileMap::WALL_HT;
        }
        return spec;
    }
    if (t != nullptr && b != nullptr && t->type == CellType::WALL && b->type == CellType::WALL) {
        spec = TileMap::WALL_V;
        if (l != nullptr && l->type == CellType::WALL) {
          spec = TileMap::WALL_VL;
        }
        if (r != nullptr && r->type == CellType::WALL) {
          spec = TileMap::WALL_VR;
        }
        return spec;
    }

    if (l != nullptr && b != nullptr && l->type == CellType::WALL && b->type == CellType::WALL) {
        spec = TileMap::WALL_LB;
    }
    else
    if (r != nullptr && b != nullptr && r->type == CellType::WALL && b->type == CellType::WALL) {
        spec = TileMap::WALL_RB;
    }
    else
    if (r != nullptr && t != nullptr && r->type == CellType::WALL && t->type == CellType::WALL) {
        spec = TileMap::WALL_RT;
    }
    else
    if (l != nullptr && t != nullptr && l->type == CellType::WALL && t->type == CellType::WALL) {
        spec = TileMap::WALL_LT;
    }
    return spec;
}
};

#endif // __TILE_H_
