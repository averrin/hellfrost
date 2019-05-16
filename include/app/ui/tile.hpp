#ifndef __TILE_H_
#define __TILE_H_

#include <utility>
#include <vector>
#include <map>
#include <memory>
#include <SFML/Graphics.hpp>
#include <librandom/random.hpp>

#include <game/cell.hpp>

class Tile {
public:
  std::pair<int, int> spriteIndex;
  sf::Sprite sprite;
  std::shared_ptr<Cell> cell;
};

class Viewport {
  const std::pair<int, int> SPRITE_MAP_SIZE = std::make_pair(32, 32);
  const int SPRITE_GAP_SIZE = 1;

  sf::IntRect getTileRect(int x, int y) {
    return sf::IntRect((SPRITE_TILE_SIZE.first + SPRITE_GAP_SIZE) * y,
                       (SPRITE_TILE_SIZE.second + SPRITE_GAP_SIZE) * x,
                       SPRITE_TILE_SIZE.first, SPRITE_TILE_SIZE.second);
  }


  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();

public:
  const std::pair<int, int> SPRITE_TILE_SIZE = std::make_pair(16, 16);
  std::map<std::string, std::shared_ptr<Tile>> tilesCache;

public:
  int width = 60;
  int height = 50;
  std::pair<int, int> position;
  int z = 0;
  sf::Texture tilesTexture;
  std::vector<std::shared_ptr<Region>> regions;
  std::shared_ptr<Cell> getCell(int x, int y, int z) {
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
    return cell;
  }

  std::shared_ptr<Tile> getTile(int x, int y, int z) {
    x += position.first;
    y += position.second;

    auto coords = fmt::format("{}.{}.{}", x, y, z);
    auto t = tilesCache.find(coords);
    if (t != tilesCache.end()) return t->second;

    auto cell = getCell(x, y, z);
    if (cell == nullptr) {
      cell = std::make_shared<Cell>(x, y, CellType::UNKNOWN);
    }

    sf::Sprite sprite;
    sprite.setTexture(tilesTexture);
    std::pair<int, int> sprite_spec;
    if (cell->type == CellType::GROUND) {
      auto v = gen->R(5, 7);
      sprite_spec = std::make_pair(0, v);
    } else if (cell->type == CellType::FLOOR) {
      auto v = gen->R(16, 17);
      sprite_spec = std::make_pair(0, v);
    } else if (cell->type == CellType::WALL) {
      sprite_spec = getWallSpec(cell);
    } else if (cell->type == CellType::ROOF) {
      sprite_spec = std::make_pair<int, int>(17, 1);
    } else if (cell->type == CellType::UNKNOWN) {
      sprite_spec = std::make_pair<int, int>(0, 0);
    }
    auto src = getTileRect(sprite_spec.first, sprite_spec.second);
    sprite.setTextureRect(src);
    sprite.setPosition(
        (cell->anchor.first + cell->x) * (SPRITE_TILE_SIZE.first),
        (cell->anchor.second + cell->y) * (SPRITE_TILE_SIZE.second));
    auto tile = std::make_shared<Tile>();
    tile->cell = cell;
    tile->sprite = sprite;
    tile->spriteIndex = sprite_spec;
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
    // auto n = getNeighbors(cell);
    auto coords = getCoords(cell);
    auto l = getCell(coords.first-1, coords.second, z);
    auto r = getCell(coords.first+1, coords.second, z);
    auto t = getCell(coords.first, coords.second-1, z);
    auto b = getCell(coords.first, coords.second+1, z);
    // if (c == nullptr || c->type != CellType::WALL) {
    //     return std::make_pair<int, int>(13, 16);
    // }
    if (l != nullptr && r != nullptr && l->type == CellType::WALL && r->type == CellType::WALL) {
        return std::make_pair<int, int>(13, 17);
    }
    if (t != nullptr && b != nullptr && t->type == CellType::WALL && b->type == CellType::WALL) {
        return std::make_pair<int, int>(14, 16);
    }

    if (l != nullptr && b != nullptr && l->type == CellType::WALL && b->type == CellType::WALL) {
        return std::make_pair<int, int>(13, 18);
    }
    if (r != nullptr && b != nullptr && r->type == CellType::WALL && b->type == CellType::WALL) {
        return std::make_pair<int, int>(13, 16);
    }
}

  // std::vector<std::shared_ptr<Cell>> getNeighbors(std::shared_ptr<Cell> cell) {
  //   std::vector<std::shared_ptr<Cell>> nbrs;
  //   std::shared_ptr<Cell> c;
  //   c = getCell(cell->x-1, cell->y-1, z);
  //   if (c != nullptr) nbrs.push_back(c);
  //   c = getCell(cell->x, cell->y-1, z);
  //   if (c != nullptr) nbrs.push_back(c);
  //   c = getCell(cell->x+1, cell->y-1, z);
  //   if (c != nullptr) nbrs.push_back(c);
  //   c = getCell(cell->x+1, cell->y, z);
  //   if (c != nullptr) nbrs.push_back(c);
  //   c = getCell(cell->x+1, cell->y+1, z);
  //   if (c != nullptr) nbrs.push_back(c);
  //   c = getCell(cell->x, cell->y+1, z);
  //   if (c != nullptr) nbrs.push_back(c);
  //   c = getCell(cell->x-1, cell->y+1, z);
  //   if (c != nullptr) nbrs.push_back(c);
  //   c = getCell(cell->x-1, cell->y, z);
  //   if (c != nullptr) nbrs.push_back(c);
  //   return nbrs;
  // }
};

#endif // __TILE_H_
