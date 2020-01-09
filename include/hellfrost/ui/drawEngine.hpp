#ifndef __DRAWENGINE_H_
#define __DRAWENGINE_H_
#include <mutex>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <hellfrost/deps.hpp>
#include <hellfrost/game/components.hpp>
#include <hellfrost/game/gameManager.hpp>

#include <hellfrost/ui/layers.hpp>
#include <hellfrost/ui/tile.hpp>
#include <hellfrost/ui/tileSet.hpp>
#include <hellfrost/ui/viewport.hpp>


namespace hellfrost {
struct renderable {
  std::string spriteCat = "ENV";
  std::string spriteKey = "UNKNOWN";
  std::string fgColor = "#fff";
  bool hasBg = false;
  std::string bgColor = "#ffffff00";
  bool hasBorder = false;
  std::string borderColor = "#ffffff00";
  bool hidden = false;
  int zIndex = 0;
};

class DrawEngine {
  std::shared_ptr<sf::RenderWindow> window;
  std::shared_ptr<sf::RenderTexture> canvas =
      std::make_shared<sf::RenderTexture>();
  bool needRedraw = true;
  LibLog::Logger log = LibLog::Logger(fmt::color::lime, "DRAW");
  sf::Color bgColor;

  std::shared_ptr<entt::observer> ob_visible;
  std::shared_ptr<entt::observer> ob_position;
  std::shared_ptr<entt::observer> ob_renderable;
  std::shared_ptr<entt::observer> ob_new_renderable;
  std::shared_ptr<entt::observer> ob_ineditor;

  std::mutex cache_mutex;

  std::thread cacheThread;

  std::shared_ptr<sf::Sprite> makeSprite(std::string cat, std::string key);
  std::array<int, 3> getWallSpec(std::shared_ptr<Cell> cell);
  std::optional<std::shared_ptr<Tile>> getTile(int x, int y, int z);
  sf::Color getColor(std::string color) {
    auto c = Color::fromHexString(color);
    return sf::Color(c.r, c.g, c.b, c.a);
  }
  std::shared_ptr<sf::RectangleShape> makeBg(sf::Vector2f pos);

public:
  ~DrawEngine() {
    entt::service_locator<Viewport>::reset();
    tilesTextures.clear();
    tilesCache.clear();
  }
  std::shared_ptr<LayersManager> layers;
  std::map<std::string, std::shared_ptr<Tile>> tilesCache{};
  std::vector<std::pair<int, int>> damage{};
  std::shared_ptr<TileSet> tileSet;
  std::vector<sf::Texture> tilesTextures;

  int vW = 0;
  int vH = 0;

  int redraws = 0;
  int tilesUpdated = 0;
  int cache_count = 0;

  sf::IntRect getTileRect(int x, int y) {
    return sf::IntRect((tileSet->size.first + tileSet->gap) * x,
                       (tileSet->size.second + tileSet->gap) * y,
                       tileSet->size.first, tileSet->size.second);
  }
  void loadTileset(fs::path path);
  std::optional<std::shared_ptr<Tile>> cacheTile(int x, int y, int z);

  DrawEngine(std::shared_ptr<sf::RenderWindow> w, fs::path PATH);
  void invalidate() {
    needRedraw = true;
    layers->invalidate();
  }
  void clearCache() {
    const std::lock_guard<std::mutex> lock(cache_mutex);
    tilesCache.clear();
  }
  void update();
  void observe();
  void renderTile(std::shared_ptr<Tile>);
  void updateTile(std::shared_ptr<Tile>);
  void renderEntity(entt::entity e);
  void updateEntity(entt::entity e);

  sf::Texture Draw();
  void resize();
};

};     // namespace hellfrost
#endif // __DRAWENGINE_H_
