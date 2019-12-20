#ifndef __VIEWPORT_H_
#define __VIEWPORT_H_
#include <app/ui/tile.hpp>

struct TileSet {
  std::vector<std::string> maps;
  std::pair<int, int> size;
  int gap;
  std::map<std::string, std::array<int, 3>> sprites;
};

class Viewport {
  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();

public:
    ~Viewport() {
        tilesTextures.clear();
        tilesCache.clear();
    }
  sf::IntRect getTileRect(int x, int y) {
    return sf::IntRect((tileSet.size.first + tileSet.gap) * x,
                       (tileSet.size.second + tileSet.gap) * y,
                       tileSet.size.first, tileSet.size.second);
  }

  void loadTileset(fs::path path) {
    std::ifstream file(path / "tiles.json");
    json tilesSpec;
    file >> tilesSpec;
    tileSet = TileSet{
      tilesSpec["TILEMAPS"],
      tilesSpec["SIZE"],
      tilesSpec["GAP"],
      tilesSpec["SPRITES"],
    };

    std::ifstream cfile(path / "colors.json");
    cfile >> colors;

    tilesTextures.clear();
    for (auto t_path : tileSet.maps) {
      sf::Texture t;
      t.loadFromFile(path / t_path);
      tilesTextures.push_back(t);
    }

  }
  Viewport() {

    // UT = std::make_shared<Tile>();
    // auto sprite = std::make_shared<sf::Sprite>();
    // auto src = getTileRect(0, 0);
    // auto fgColor = sf::Color(33,33,23);
    // sprite->setTexture(tilesTextures[0]);
    // sprite->setTextureRect(src);
    // sprite->setColor(fgColor);
    // UT->sprites.push_back(sprite);
  }
  TileSet tileSet;
  json colors;
  std::map<std::string, std::shared_ptr<Tile>> tilesCache;
  // std::shared_ptr<Tile> UT;

public:

  int width = 100;
  int height = 50;
  std::pair<int, int> position;
  int z = 0;
  std::vector<sf::Texture> tilesTextures;
  std::vector<std::shared_ptr<Region>> regions;

  void setSize(std::pair<int, int> size) {
    width = size.first;
    height = size.second;
  }

  std::pair<std::optional<std::shared_ptr<Cell>>, int> getCell(int x, int y, int z) {
    std::optional<std::shared_ptr<Cell>> cell = std::nullopt;
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
    if (z > 0 && !cell) {
      return getCell(x,y,z-1);
    }
    return std::make_pair(cell, z);
  }

  std::pair<std::optional<std::shared_ptr<Region>>, int> getRegion(int x, int y, int z) {
    std::optional<std::shared_ptr<Region>> reg = std::nullopt;
    for (auto region : regions) {
      if (region->active && region->z == z &&
          x >= region->position.first &&
          x < (int)(region->position.first + region->cells[0].size()) &&
          y >= region->position.second &&
          y < (int)(region->position.second + region->cells.size())) {
        return std::make_pair(region, z);
      }
    }
    if (z > 0 && !reg) {
      return getRegion(x,y,z-1);
    }
    return std::make_pair(reg, z);
  }

  sf::Color getColor(std::string color) {
    auto c = Color::fromHexString(color);
    return sf::Color(c.r, c.g, c.b, c.a);
  }

std::shared_ptr<sf::Sprite> makeSprite(std::string cat, std::string key) {
    auto spec = tileSet.sprites["UNKNOWN"];
    if (tileSet.sprites.find(key) != tileSet.sprites.end()) {
      spec = tileSet.sprites[key];
    } else  {
      fmt::print("Missed sprite spec: {}: {}\n", cat, key);
    }
    auto s = std::make_shared<sf::Sprite>();
    s->setTexture(tilesTextures[spec[0]]);
    s->setTextureRect(getTileRect(spec[1], spec[2]));
    if (colors[cat].contains(key)) {
      s->setColor(getColor(colors[cat][key]));
    } else {
      // fmt::print("Missed color: {}: {}\n", cat, key);
    }
    return s;
}


  std::optional<std::shared_ptr<Tile>> getTile(int x, int y, int z) {
    x += position.first;
    y += position.second;

    auto [c, rz] = getCell(x, y, z);
    if (!c /*|| (*c)->type == CellType::UNKNOWN*/) {
      return std::nullopt;
    }
    auto cell = *c;

    auto coords = fmt::format("{}.{}.{}", x, y, rz);
    auto t = tilesCache.find(coords);
    // if (t != tilesCache.end() && !t->damaged) return t->second;
    if (t != tilesCache.end()) return t->second;
    auto [region, _z] = getRegion(x, y, rz);
    if (cell->type == CellType::UNKNOWN && region && (*region)->location->type.type != LocationType::EXTERIOR) {
      return std::nullopt;
    }

    auto tile = std::make_shared<Tile>();
    auto fgColor = sf::Color(220,220,220);
    auto sprite = std::make_shared<sf::Sprite>();
    tile->sprites.push_back(sprite);
    auto sprite_spec = tileSet.sprites["UNKNOWN"];
    if (cell->type == CellType::UPSTAIRS) {
      sprite_spec = tileSet.sprites["UPSTAIRS"];
    } else if (cell->type == CellType::DOWNSTAIRS) {
      sprite_spec = tileSet.sprites["DOWNSTAIRS"];
    } else if (cell->type == CellType::WATER || (cell->type == CellType::UNKNOWN && region && (*region)->location->type.type == LocationType::EXTERIOR)) {
      sprite_spec = tileSet.sprites["WATER"];
      fgColor = getColor(colors["ENV"]["WATER"]);
    } else if (cell->type == CellType::VOID) {
      sprite_spec = tileSet.sprites["VOID"];
      fgColor = getColor(colors["ENV"]["VOID"]);
      tile->bgColor = getColor(colors["ENV"]["VOID"]);
      tile->hasBackground = true;
      tile->bgLayer = 0;
    } else if (cell->passThrough) {

      if (cell->type == CellType::GROUND) {
        sprite_spec = tileSet.sprites["GRASS"];
        auto brown = Color::fromHexString(colors["PALETTE"]["BROWN"]);
        brown.g += gen->R(0, 60);
        fgColor = sf::Color(brown.r, brown.g, brown.b, brown.a);
      } else if (cell->type == CellType::FLOOR) {
        sprite_spec = tileSet.sprites["FLOOR"];
        fgColor = getColor(colors["ENV"]["FLOOR"]);
      }

      // if (region) {
      //   auto doors = utils::castObjects<Door>((*region)->location->getObjects(cell));
      //   if (doors.size() != 0) {
      //     if (doors.front()->hidden) {
      //       sprite_spec = getWallSpec(cell);
      //       fgColor = getColor(colors["ENV"]["WALL"]);
      //       if (cell->hasFeature(CellFeature::CAVE)) {
      //         fgColor = getColor(colors["ENV"]["WALL_CAVE"]);
      //       }
      //     } else {
      //       sprite_spec = tileSet.sprites["DOOR"];
      //       fgColor = getColor(colors["ENV"]["DOOR"]);
      //     }
      //   } else {
      //     auto terrain = utils::castObjects<Terrain>((*region)->location->getObjects(cell));
      //     if (terrain.size() != 0) {
      //         auto key = terrain.front()->type.name;
      //         std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      //         std::replace(key.begin(), key.end(), ' ', '_');
      //         tile->sprites.push_back(makeSprite("TERRAIN", key));
      //     } else {
      //       auto enemies = utils::castObjects<Enemy>((*region)->location->getObjects(cell));
      //       if (enemies.size() != 0) {
      //           auto key = enemies.front()->type.name;
      //           std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      //           std::replace(key.begin(), key.end(), ' ', '_');
      //           // fmt::print("{}\n", key);
      //           tile->sprites.push_back(makeSprite("ENEMY", key));
      //       } else {
      //         auto items = utils::castObjects<Item>((*region)->location->getObjects(cell));
      //         if (items.size() != 0) {
      //             auto key = items.front()->type.name;
      //             std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      //             std::replace(key.begin(), key.end(), ' ', '_');
      //             // fmt::print("{}\n", key);
      //             tile->sprites.push_back(makeSprite("ITEMS", key));
      //         }
      //       }
      //     }
      //   }
      // }
    } else if (cell->type == CellType::WALL) {
      sprite_spec = getWallSpec(cell);
      fgColor = getColor(colors["ENV"]["WALL"]);
      if (cell->hasFeature(CellFeature::CAVE)) {
        fgColor = getColor(colors["ENV"]["WALL_CAVE"]);
      }
    } else if (cell->type == CellType::ROOF) {
      sprite_spec = {0, 1, 17};
    }

    sprite->setTexture(tilesTextures[sprite_spec[0]]);


    auto src = getTileRect(sprite_spec[1], sprite_spec[2]);
    sprite->setTextureRect(src);
    sprite->setColor(fgColor);

    // tile->bgColor = sf::Color(255, 255, 255, rand()%256);

    // if (cell->type != CellType::UNKNOWN) {
      for (auto f : cell->features) {
        if (f == CellFeature::BLOOD) {
            tile->bgColor = getColor(colors["ENV"]["BLOOD"]);
            tile->hasBackground = true;
            for (auto s : tile->sprites) {
              tile->bgLayer++;
            }
            tile->bgLayer--;
        }
        if (f == CellFeature::FROST) {
            tile->bgColor = getColor(colors["ENV"]["FROST"]);
            tile->hasBackground = true;
        }
        if (f == CellFeature::CORRUPT) {
            tile->bgColor = getColor(colors["ENV"]["CORRUPT"]);
            tile->hasBackground = true;
        }
      }
    // }

    tile->pos = sf::Vector2f((cell->anchor.first + cell->x) * (tileSet.size.first),
          (cell->anchor.second + cell->y) * (tileSet.size.second));
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

std::array<int, 3> getWallSpec(std::shared_ptr<Cell> cell) {
    auto coords = getCoords(cell);
    auto [_l, _z1] = getCell(coords.first-1, coords.second, z);
    auto [_r, _z2] = getCell(coords.first+1, coords.second, z);
    auto [_t, _z3] = getCell(coords.first, coords.second-1, z);
    auto [_b, _z4] = getCell(coords.first, coords.second+1, z);

    auto spec = tileSet.sprites["WALL_H"];
    if (_r && _l && ((*_r)->type == CellType::WALL && (*_l)->type != CellType::WALL)) {
        spec = tileSet.sprites["WALL_HRS"];
    }
    else
    if (_r && _l && ((*_r)->type != CellType::WALL && (*_l)->type == CellType::WALL)) {
        spec = tileSet.sprites["WALL_HLS"];
    }
    if (_t && _b && ((*_t)->type == CellType::WALL && (*_b)->type == CellType::WALL)) {
        spec = tileSet.sprites["WALL_V"];
    }
    else
    if (_t && _b && ((*_t)->type == CellType::WALL && (*_b)->type != CellType::WALL)) {
        spec = tileSet.sprites["WALL_VBS"];
    }
    else
    if (_t && _b && ((*_t)->type != CellType::WALL && (*_b)->type == CellType::WALL)) {
        spec = tileSet.sprites["WALL_VTS"];
    }

    if (_t && _b && (*_t)->type == CellType::WALL && (*_b)->type == CellType::WALL &&
      _l && _r && (*_l)->type == CellType::WALL && (*_r)->type == CellType::WALL) {
          spec = tileSet.sprites["WALL_CROSS"];
          return spec;
    }
    if (_t && _b && (*_t)->type != CellType::WALL && (*_b)->type != CellType::WALL &&
      _l && _r && (*_l)->type != CellType::WALL && (*_r)->type != CellType::WALL) {
          spec = tileSet.sprites["WALL_C"];
          return spec;
    }

    if (_l && _r && (*_l)->type == CellType::WALL && (*_r)->type == CellType::WALL) {
        spec = tileSet.sprites["WALL_H"];
        if (_b && (*_b)->type == CellType::WALL) {
          spec = tileSet.sprites["WALL_HB"];
        }
        if (_t && (*_t)->type == CellType::WALL) {
          spec = tileSet.sprites["WALL_HT"];
        }
        return spec;
    }
    if (_t && _b && (*_t)->type == CellType::WALL && (*_b)->type == CellType::WALL) {
        spec = tileSet.sprites["WALL_V"];
        if (_l && (*_l)->type == CellType::WALL) {
          spec = tileSet.sprites["WALL_VL"];
        }
        if (_r && (*_r)->type == CellType::WALL) {
          spec = tileSet.sprites["WALL_VR"];
        }
        return spec;
    }

    if (_l && _b && (*_l)->type == CellType::WALL && (*_b)->type == CellType::WALL) {
        spec = tileSet.sprites["WALL_LB"];
    }
    else
    if (_r && _b && (*_r)->type == CellType::WALL && (*_b)->type == CellType::WALL) {
        spec = tileSet.sprites["WALL_RB"];
    }
    else
    if (_r && _t && (*_r)->type == CellType::WALL && (*_t)->type == CellType::WALL) {
        spec = tileSet.sprites["WALL_RT"];
    }
    else
    if (_l && _t && (*_l)->type == CellType::WALL && (*_t)->type == CellType::WALL) {
        spec = tileSet.sprites["WALL_LT"];
    }
    return spec;
}
};

#endif // __VIEWPORT_H_
