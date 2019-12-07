#include <thread>   // for access to this thread
#include <utility>   // for access to this thread

#include "app/application.hpp"

#include <imgui-sfml/imgui-SFML.h>
#include <imgui/imgui.h>

#include <libcolor/libcolor.hpp>
#include <app/fonts/material_design_icons.h>
#include <app/style/theme.h>
#include <SFML/Graphics.hpp>

// static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;
static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

Application::Application(std::string app_name, std::string version)
    : APP_NAME(app_name), VERSION(version) {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

  window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), APP_NAME,
                                sf::Style::Default, settings);
  window->setVerticalSyncEnabled(true);

  ImGui::SFML::Init(*window);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  window->resetGLStates();
}

int x = -4;
int y = 19;
int z = 0;

void Application::processEvent(sf::Event event) {
  ImGui::SFML::ProcessEvent(event);
  switch (event.type) {
  case sf::Event::KeyPressed:
    switch (event.key.code) {
    case sf::Keyboard::Escape:
      window->close();
      break;
    case sf::Keyboard::Right:
      x += 1;
      break;
    case sf::Keyboard::Left:
      x -= 1;
      break;
    case sf::Keyboard::Up:
      y -= 1;
      break;
    case sf::Keyboard::Down:
      y += 1;
      break;
    case sf::Keyboard::U:
      z += 1;
      break;
    case sf::Keyboard::D:
      z -= 1;
      break;
    }
    view_map->position = std::make_pair(x, y);
    view_map->z = z;
    break;
  case sf::Event::Closed:
    window->close();
    break;
  }
}

void Application::drawDocking() {
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
    // render our background and handle the pass-thru hole, so we ask Begin() to
    // not render a background.
    auto dockSpaceSize = viewport->Size;
    dockSpaceSize.y -= 16.0f; // remove the status bar
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(dockSpaceSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    // Dockspace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
    } else {
      // TODO: emit a log message
    }
    ImGui::End();
}

void Application::drawStatusBar(float width, float height, float pos_x,
                                    float pos_y) {
  // Draw status bar (no docking)
  ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiSetCond_Always);
  ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiSetCond_Always);
  ImGui::Begin("statusbar", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings |
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_NoResize);

  // Call the derived class to add stuff to the status bar
  // DrawInsideStatusBar(width - 45.0f, height);

  // Draw the common stuff
  ImGui::SameLine(width - 60.0f);
  Font font(Font::FAMILY_MONOSPACE);
  font.Normal().Regular().SmallSize();
  ImGui::PushFont(font.ImGuiFont());
  ImGui::Text("FPS: %ld", std::lround(ImGui::GetIO().Framerate));
  ImGui::PopFont();
  ImGui::End();
}

bool h_v = true;

void Application::drawMainWindow() {
  ImGui::Begin(APP_NAME.c_str());
  // ImGui::Text("\n\nSFML + ImGui starter (%s + %s)\n\n", VERSION.c_str(),
              // IMGUI_VERSION);
  ImGui::Text("\n\nCache len: %d\n\n", view_map->tilesCache.size());
  if (ImGui::SliderInt("x", &x, -10, view_map->height*10)) {
    view_map->position.first = x;
  }
  if (ImGui::SliderInt("y", &y, -10, view_map->width*10)) {
    view_map->position.second = y;
  }
  if (ImGui::SliderInt("z", &z, -10, 10)) {
    view_map->z = z;
  }
  if (ImGui::Checkbox("house", &(view_map->regions[1]->active))) {
    view_map->tilesCache.clear(); //TODO: invalidate region cache only
  }
  ImGui::End();
}

int Application::serve() {
  log.info("serve");
  sf::Clock deltaClock;

  sf::Texture tiles;
  tiles.loadFromFile("ascii_tiles.png");

  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();

  view_map =  std::make_shared<Viewport>();
  view_map->position = std::make_pair(x, y);
  auto area = std::make_shared<Region>();
  area->position = std::make_pair<int, int>(-10, -10);
  area->active = true;
  area->z = 0;

  auto house = std::make_shared<Region>();
  house->active = h_v;
  house->position = std::make_pair<int, int>(10, 10);
  house->z = 0;

  auto roof = std::make_shared<Region>();
  roof->active = h_v;
  roof->position = house->position;
  roof->z = 1;

  area->fill(view_map->height*10, view_map->width*10, CellType::GROUND);
  house->fill(20, 40, CellType::FLOOR);
  house->walls(CellType::WALL);
  roof->fill(house->cells.size(), house->cells[0].size(), CellType::ROOF);
  // house->cells[0][10]->type = CellType::FLOOR;
  // house->cells[1][5]->type = CellType::WALL;
  // house->cells[5][1]->type = CellType::WALL;
  // house->cells[house->cells.size()-2][5]->type = CellType::WALL;
  // house->cells[5][house->cells[0].size()-2]->type = CellType::WALL;

  auto inside = std::make_shared<Region>();
  inside->position = house->position;
  inside->z = 0;
  inside->fill(10, 10, CellType::FLOOR);
  inside->walls(CellType::WALL);
  inside->active = true;

  auto insideT = std::make_shared<Region>();
  insideT->position = std::make_pair<int, int>(15, 15);
  insideT->z = 1;
  insideT->fill(10, 10, CellType::FLOOR);
  insideT->walls(CellType::WALL);
  insideT->active = true;

  auto insideTr = std::make_shared<Region>();
  insideTr->position = std::make_pair<int, int>(15, 15);
  insideTr->z = 2;
  insideTr->fill(10, 10, CellType::ROOF);
  insideTr->active = true;

  auto inside2 = std::make_shared<Region>();
  inside2->position = std::make_pair<int, int>(15, 15);
  inside2->z = 0;
  inside2->fill(10, 10, CellType::FLOOR);
  inside2->walls(CellType::WALL);
  inside2->active = true;

  auto inside3 = std::make_shared<Region>();
  inside3->position = std::make_pair<int, int>(10, 24);
  inside3->z = 0;
  inside3->fill(6, 6, CellType::FLOOR);
  inside3->walls(CellType::WALL);
  inside3->active = true;

  view_map->regions.push_back(area);
  view_map->regions.push_back(house);
  view_map->regions.push_back(inside);
  view_map->regions.push_back(inside2);
  view_map->regions.push_back(inside3);
  view_map->regions.push_back(roof);
  view_map->regions.push_back(insideT);
  view_map->regions.push_back(insideTr);
  view_map->tilesTexture = tiles;



  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      processEvent(event);
    }

    static float wanted_fps;
    // if (sleep_when_inactive &&
    //     !(SDL_GetWindowFlags(window_) & SDL_WINDOW_INPUT_FOCUS)) {
    //   wanted_fps = 20.0f;
    // } else {
      wanted_fps = 90.0f;
    // }
    float current_fps = ImGui::GetIO().Framerate;
    float frame_time = 1000 / current_fps;
    auto wait_time = std::lround(1000 / wanted_fps - frame_time);
    if (wanted_fps < current_fps) {
      std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
    }

    auto bgColor = sf::Color(33, 33, 23);
    window->clear(bgColor);

    auto scale = 0.8f;
    for (auto ly = 0; ly < view_map->height; ly++) {
      for (auto lx = 0; lx < view_map->width; lx++) {
        auto t = view_map->getTile(lx, ly, view_map->z);
        renderTile(t);
      }
    }
    // window->setScale(sf::Vector2f(0.5,0.5));

    ImGui::SFML::Update(*window, deltaClock.restart());

    drawDocking();

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    drawStatusBar(viewport->Size.x, 16.0f, 0.0f, viewport->Size.y - 24);

    drawMainWindow();
    ImGui::SFML::Render(*window);
    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}

void Application::renderTile(std::shared_ptr<Tile> t) {
  auto sprites = t->sprites;
  sf::RectangleShape bg;
  bg.setSize(sf::Vector2f(20, 20));
  bg.setFillColor(t->bgColor);
  bg.setPosition(sprites.front()->getPosition());
  window->draw(bg);
  for (auto sprite : sprites) {
    sprite->setPosition(t->pos);
    sprite->move(-view_map->SPRITE_TILE_SIZE.first*view_map->position.first,
              -view_map->SPRITE_TILE_SIZE.second*view_map->position.second );
    window->draw(*sprite);
  }
}
