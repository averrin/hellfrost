#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <entt/entt.hpp>

#include <hellfrost/game/gameManager.hpp>
#include <hellfrost/ui/drawEngine.hpp>

#include <editor/editor.hpp>

namespace hf = hellfrost;

class Application {
  sf::RectangleShape rectangle;
  std::shared_ptr<std::mutex> reg_mutex = std::make_shared<std::mutex>();
  LibLog::Logger &log = LibLog::Logger::getInstance();

  sf::Vector2<float> current_pos;
  void setupGui();

  std::shared_ptr<hf::GameManager> gm;
  std::shared_ptr<hf::DrawEngine> engine;
  std::shared_ptr<Editor> editor;

  sf::RenderWindow *window;
  void processEvent(sf::Event event);
  void drawStatusBar(float width, float height, float pos_x, float pos_y);
  void drawDocking(float);

  void genLocation(int);

public:
  Application(std::string, fs::path, std::string, int);
  ~Application();

  std::string APP_NAME;
  std::string VERSION;
  fs::path PATH;
  bool DEBUG = true;

  int serve();
};

#endif // __APPLICATION_H_
