#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <dukglue/dukglue.h>
#include <entt/entt.hpp>
#include <imguicolortextedit/TextEditor.h>

#include <liblog/liblog.hpp>
#include <lss/components.hpp>
#include <lss/gameManager.hpp>

#include <app/console.hpp>
#include <app/editor.hpp>
#include <app/scriptWrappers.hpp>
#include <app/ui/drawEngine.hpp>
#include <app/ui/viewport.hpp>
#include <mutex>

class Object;
class Application {
  sf::RectangleShape rectangle;
  sf::RectangleShape cursor;
  duk_context *duk_ctx = duk_create_heap_default();

  std::thread jsThread;
  std::thread cacheThread;
  std::shared_ptr<std::mutex> reg_mutex = std::make_shared<std::mutex>();

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

  std::shared_ptr<DrawEngine> engine;
  sf::RenderWindow *window;
  std::shared_ptr<Viewport> viewport;
  void processEvent(sf::Event event);
  int serve();
  void drawStatusBar(float width, float height, float pos_x, float pos_y);
  void drawDocking(float);
  void drawLocationWindow();
  void drawViewportWindow();
  void dukEditorWindow();

  Console console;
  TextEditor duk_editor;
  TextEditor::ErrorMarkers markers;

  void drawEntityInfo(entt::entity);

  void genLocation(int);
  int seed;

  LibLog::Logger &log = LibLog::Logger::getInstance();

  std::optional<std::pair<int, int>> lockedPos = std::nullopt;
  sf::Vector2<float> current_pos;

  void duk_log(const std::string msg);
  void duk_exec(const char *code);
};

#endif // __APPLICATION_H_
