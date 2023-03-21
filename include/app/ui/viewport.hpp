#ifndef __VIEWPORT_H_
#define __VIEWPORT_H_
#include <app/ui/tile.hpp>
#include <exception>
#include <mutex>

struct TileSet {
  std::vector<std::string> maps;
  std::pair<int, int> size;
  int gap;
  std::map<std::string, TileSpec> sprites;
  std::map<std::string, std::vector<std::string>> spriteVariations;
  std::map<std::string, std::vector<std::string>> colorVariations;
  std::map<std::string, std::map<std::string, std::vector<float>>> colorWandering;
};

class Viewport {
  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();

public:
  std::mutex mutex;
  ~Viewport() { tilesTextures.clear(); }
  sf::IntRect getTileRect(int x, int y) {
    return sf::IntRect((tileSet.size.first + tileSet.gap) * x,
                       (tileSet.size.second + tileSet.gap) * y,
                       tileSet.size.first, tileSet.size.second);
  }

  void loadTileset(fs::path path) {
    std::ifstream cfile(path / "colors.json");
    cfile >> colors;

    std::ifstream file(path / "tiles.json");
    json tilesSpec;
    file >> tilesSpec;
    tileSet = TileSet{tilesSpec["TILEMAPS"],   tilesSpec["SIZE"],
                      tilesSpec["GAP"],        tilesSpec["SPRITES"],
                      tilesSpec["VARIATIONS"], colors["VARIATIONS"], colors["WANDERING"]};

    tilesTextures.clear();
    for (auto t_path : tileSet.maps) {
      sf::Texture t;
      t.loadFromFile(path / t_path);
      tilesTextures.push_back(t);
    }
  }
  Viewport() {}
  TileSet tileSet;
  json colors;
  // std::shared_ptr<Tile> UT;

public:
  float scale = 1.f;
  int width = 100;
  int height = 50;

  int view_x = 0;
  int view_y = 0;
  int view_z = 0;

  std::vector<sf::Texture> tilesTextures;
  std::vector<std::shared_ptr<Region>> regions;

  void setSize(std::pair<int, int> size) {
    width = size.first;
    height = size.second;
  }

  std::pair<std::optional<std::shared_ptr<Cell>>, int> getCell(int x, int y,
                                                               int z) {
    std::optional<std::shared_ptr<Cell>> cell = std::nullopt;
    for (auto region : regions) {
      if (!region->cells.empty() && region->active && region->z == z &&
          x >= region->position.first &&
          x < (int)(region->position.first + region->cells[0].size()) &&
          y >= region->position.second &&
          y < (int)(region->position.second + region->cells.size())) {
        cell = region->cells[y - region->position.second]
                            [x - region->position.first];
      }
    }
    if (z > 0 && !cell) {
      return getCell(x, y, z - 1);
    }
    return std::make_pair(cell, z);
  }

  std::pair<std::optional<std::shared_ptr<Region>>, int> getRegion(int x, int y,
                                                                   int z) {
    std::optional<std::shared_ptr<Region>> reg = std::nullopt;
    for (auto region : regions) {
      if (!region->cells.empty() && region->active && region->z == z &&
          x >= region->position.first &&
          x < (int)(region->position.first + region->cells[0].size()) &&
          y >= region->position.second &&
          y < (int)(region->position.second + region->cells.size())) {
        return std::make_pair(region, z);
      }
    }
    if (z > 0 && !reg) {
      return getRegion(x, y, z - 1);
    }
    return std::make_pair(reg, z);
  }

  sf::Color getColor(std::string color) {
    auto c = Color::fromHexString(color);
    return sf::Color(c.r, c.g, c.b, c.a);
  }

  sf::Color getColor(std::string cat, std::string key);
  std::string getColorString(std::string cat, std::string key);

  std::shared_ptr<sf::Sprite> makeSprite(std::string cat, std::string key) {
    auto spec = tileSet.sprites["UNKNOWN"];
    if (tileSet.spriteVariations.find(key) != tileSet.spriteVariations.end()) {
      key = *Random::get(tileSet.spriteVariations[key]);
    }
    if (tileSet.sprites.find(key) != tileSet.sprites.end()) {
      spec = tileSet.sprites[key];
    } else {
      fmt::print("Missed sprite spec: {}: {}\n", cat, key);
    }
    auto s = std::make_shared<sf::Sprite>();
    s->setTexture(tilesTextures[spec[0]]);
    s->setTextureRect(getTileRect(spec[1], spec[2]));
    // if (spec[3] > 0) {
    // s->setOrigin(tileSet.size.first / 2, tileSet.size.second / 2);
    // s->setRotation(spec[3] * 90);
    // std::shared_ptr<sf::Sprite> s =
    // std::make_shared<sf::Sprite>(*s->getTexture()); s->setOrigin(0, 0);
    // }
    if (cat != "") {
      s->setColor(getColor(cat, key));
    }
    return s;
  }

  std::optional<std::shared_ptr<Tile>> getTile(int x, int y, int z);

  std::pair<int, int> getCoords(std::shared_ptr<Cell> cell) {
    return std::make_pair<int, int>(cell->anchor.first + cell->x,
                                    cell->anchor.second + cell->y);
  }

  TileSpec getWallSpec(std::shared_ptr<Cell> cell);
};

#endif // __VIEWPORT_H_
