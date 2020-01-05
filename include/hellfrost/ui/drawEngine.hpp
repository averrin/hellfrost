#ifndef __DRAWENGINE_H_
#define __DRAWENGINE_H_
#include <mutex>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <hellfrost/game/components.hpp>
#include <hellfrost/deps.hpp>

#include <hellfrost/ui/layers.hpp>
#include <hellfrost/ui/tile.hpp>
#include <hellfrost/ui/viewport.hpp>

namespace hellfrost {
struct renderable {
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
  sf::RenderWindow *window;
  std::shared_ptr<sf::RenderTexture> canvas =
      std::make_shared<sf::RenderTexture>();
  bool needRedraw = true;
  LibLog::Logger &log = LibLog::Logger::getInstance();
  sf::Color bgColor;

  std::shared_ptr<entt::observer> ob_visible;
  std::shared_ptr<entt::observer> ob_position;
  std::shared_ptr<entt::observer> ob_renderable;
  std::shared_ptr<entt::observer> ob_new_renderable;
  std::shared_ptr<entt::observer> ob_ineditor;

  std::mutex cache_mutex;
  sf::RectangleShape cursor;

  std::thread cacheThread;
public:
  ~DrawEngine() { tilesCache.clear(); }
  std::shared_ptr<LayersManager> layers;
  std::map<std::string, std::shared_ptr<Tile>> tilesCache{};
  std::vector<std::pair<int, int>> damage{};

  int vW = 0;
  int vH = 0;

  int redraws = 0;
  int tilesUpdated = 0;
  int cache_count = 0;

  std::optional<std::shared_ptr<Tile>> cacheTile(int x, int y, int z);

  DrawEngine(sf::RenderWindow *w) : window(w) {
    layers = std::make_shared<LayersManager>();

    auto viewport = entt::service_locator<Viewport>::get().lock();
    auto c = Color::fromHexString(viewport->colors["PALETTE"]["BACKGROUND"]);
    bgColor = sf::Color(c.r, c.g, c.b, c.a);
  }
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
  void resize(sf::Vector2u);
};

}; // namespace hellfrost
#endif // __DRAWENGINE_H_
