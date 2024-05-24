#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include "lss/game/location.hpp"
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <entt/entt.hpp>

// #include <liblog/liblog.hpp>
#include <lss/components.hpp>
#include <lss/gameManager.hpp>
#include <lss/utils.hpp>

#include <app/editor.hpp>
#include <app/ide.hpp>
#include <app/scriptWrappers.hpp>
#include <app/ui/drawEngine.hpp>
#include <app/ui/viewport.hpp>
#include <mutex>

// #define SOL_ALL_SAFETIES_ON 1
// #define SOL_CHECK_ARGUMENTS
// #define SOL_EXCEPTIONS_SAFE_PROPAGATION 1
#include <sol/sol.hpp>

class Object;
class Application {
  sf::RectangleShape rectangle;
  sf::RectangleShape cursor;

  std::thread jsThread;
  std::thread cacheThread;
  std::shared_ptr<std::mutex> reg_mutex = std::make_shared<std::mutex>();

  bool is_rb_pressed = false;
  bool is_lb_pressed = false;
  bool is_mb_pressed = false;
  sf::Vector2<float> start_drag_pos;

public:
  Application(std::string, fs::path, std::string, int);
  ~Application();

  void setupGui();

  std::string APP_NAME;
  std::string VERSION;
  fs::path PATH;

  bool debug = true;

  std::shared_ptr<GameManager> gm;
  std::shared_ptr<Editor> editor;
  std::shared_ptr<IDE> ide;

  std::shared_ptr<DrawEngine> engine;
  sf::RenderWindow *window;
  std::shared_ptr<Viewport> viewport;
  void processEvent(sf::Event event);
  int serve();
  void drawStatusBar(float width, float height, float pos_x, float pos_y);
  void drawDocking(float);
  void drawLocationWindow();
  void drawViewportWindow();
  void initConfig();
  void initLuaBindings();
  void updateScale();

  sol::state lua;

  void drawEntityInfo(entt::entity);

  void genLocation();
  void genLocation(int);
  void genLocation(int, LocationSpec);
  int seed;

  ll::Logger &log = ll::Logger::getInstance();
  ll::Logger luaLog = ll::Logger(fmt::color::aqua, "LUA");

  std::optional<std::pair<int, int>> lockedPos = std::nullopt;
  sf::Vector2<float> current_pos;
};

#endif // __APPLICATION_H_
