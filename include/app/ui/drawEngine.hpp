#ifndef __DRAWENGINE_H_
#define __DRAWENGINE_H_
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <app/ui/layers.hpp>
#include <app/ui/tile.hpp>
#include <app/ui/viewport.hpp>
#include <lss/deps.hpp>
#include <string>

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
}; // namespace hellfrost

class DrawEngine {
  sf::RenderWindow *window;
  std::shared_ptr<sf::RenderTexture> canvas =
      std::make_shared<sf::RenderTexture>();
  bool needRedraw = true;
  LibLog::Logger &log = LibLog::Logger::getInstance();
  sf::Color bgColor;

  std::shared_ptr<entt::observer> visible;
  std::shared_ptr<entt::observer> position;
  std::shared_ptr<entt::observer> renderable;
  std::shared_ptr<entt::observer> new_renderable;
  std::shared_ptr<entt::observer> ineditor;
  std::shared_ptr<entt::observer> glow;

  std::map<int, std::vector<entt::entity>> lightMap;
  bool fastRedraw = false;

public:
  ~DrawEngine() { tilesCache.clear(); }
  std::shared_ptr<LayersManager> layers;
  std::map<std::string, std::shared_ptr<Tile>> tilesCache{};
  std::vector<std::pair<int, int>> damage{};

  int alpha_per_d = 10;
  float alpha_blend_inc = 0.5;
  std::string blend_mode = "add";
  int max_bright = 100;

  int vW = 0;
  int vH = 0;

  int redraws = 0;
  int tilesUpdated = 0;
  bool roomDebug = false;

  std::optional<std::shared_ptr<Tile>> cacheTile(int x, int y, int z);
  void scheduleFastRedraw() {
    fastRedraw = true;
  }
  void updateExistingLight();
  void updateVisible();

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
  void update();
  void observe();
  void renderTile(std::shared_ptr<Tile>);
  void updateTile(std::shared_ptr<Tile>);
  void renderEntity(entt::entity e);
  void updateEntity(entt::entity e);
  void updateLight();

  sf::Color getGlowColorForCell(entt::entity e, std::shared_ptr<Cell> c, int);

  sf::Texture draw();
  void resize(sf::Vector2u);
};

#endif // __DRAWENGINE_H_
