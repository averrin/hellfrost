#include <thread>   // for access to this thread
#include <utility>   // for access to this thread
#include <chrono>   // for access to this thread

#include "app/application.hpp"

#include <imgui-sfml/imgui-SFML.h>
#include <imgui/imgui.h>

#include <libcolor/libcolor.hpp>
#include <app/fonts/material_design_icons.h>
#include <app/style/theme.h>
#include <SFML/Graphics.hpp>

// static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;
static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

Application::Application(std::string app_name, std::string version, int s)
    : APP_NAME(app_name), VERSION(version) {
  seed = s;
  fmt::print("Seed: {}\n", seed);
  srand(seed);
  setupGui();
}

int x = -4;
int y = 10;
int z = 0;
float scale = 1.f;

void Application::setupGui() {
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
    needRedraw = true;
    break;
  case sf::Event::Resized:
  case sf::Event::LostFocus:
  case sf::Event::GainedFocus:
    needRedraw = true;
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
  ImGui::Text("\n\nCache len: %d\n", view_map->tilesCache.size());
  ImGui::Text("Redraws: %d\n", redraws);
  ImGui::Text("Tiles updated: %d\n\n", tilesUpdated);
  if (ImGui::SliderInt("x", &x, -10, view_map->height*10)) {
    view_map->position.first = x;
    needRedraw = true;
  }
  if (ImGui::SliderInt("y", &y, -10, view_map->width*10)) {
    view_map->position.second = y;
    needRedraw = true;
  }
  if (ImGui::SliderInt("z", &z, -10, 10)) {
    view_map->z = z;
    needRedraw = true;
  }
  if (ImGui::SliderFloat("scale", &scale, 0.1f, 3.f)) {
    needRedraw = true;
    view_map->width /= scale;
    view_map->height /= scale;
  }
  // if (ImGui::Checkbox("house", &(view_map->regions[1]->active))) {
  //   view_map->tilesCache.clear(); //TODO: invalidate region cache only
  //   needRedraw = true;
  // }
  ImGui::End();
}

int Application::serve() {
  log.info("serve");
  sf::Clock deltaClock;

  std::vector<sf::Texture> tiles;

  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();

  view_map =  std::make_shared<Viewport>();
  for (auto path : view_map->tileSet.maps) {
    sf::Texture t;
    t.loadFromFile(path);
    tiles.push_back(t);
  }
  view_map->position = std::make_pair(x, y);

  // auto area = std::make_shared<Region>();
  // area->position = std::make_pair<int, int>(-10, -10);
  // area->active = true;
  // area->z = 0;

  // auto house = std::make_shared<Region>();
  // house->active = h_v;
  // house->position = std::make_pair<int, int>(10, 10);
  // house->z = 0;

  // auto roof = std::make_shared<Region>();
  // roof->active = h_v;
  // roof->position = house->position;
  // roof->z = 1;

  // area->fill(view_map->height*10, view_map->width*10, CellType::GROUND);
  // house->fill(20, 40, CellType::FLOOR);
  // house->walls(CellType::WALL);
  // roof->fill(house->cells.size(), house->cells[0].size(), CellType::ROOF);

  // auto inside = std::make_shared<Region>();
  // inside->position = house->position;
  // inside->z = 0;
  // inside->fill(10, 10, CellType::FLOOR);
  // inside->walls(CellType::WALL);
  // inside->active = true;

  // auto insideT = std::make_shared<Region>();
  // insideT->position = std::make_pair<int, int>(15, 15);
  // insideT->z = 1;
  // insideT->fill(10, 10, CellType::FLOOR);
  // insideT->walls(CellType::WALL);
  // insideT->active = true;

  // auto insideTr = std::make_shared<Region>();
  // insideTr->position = std::make_pair<int, int>(15, 15);
  // insideTr->z = 2;
  // insideTr->fill(10, 10, CellType::ROOF);
  // insideTr->active = true;

  // auto inside2 = std::make_shared<Region>();
  // inside2->position = std::make_pair<int, int>(15, 15);
  // inside2->z = 0;
  // inside2->fill(10, 10, CellType::FLOOR);
  // inside2->walls(CellType::WALL);
  // inside2->cells[0][2]->type = CellType::FLOOR;
  // inside2->active = true;

  // auto inside3 = std::make_shared<Region>();
  // inside3->position = std::make_pair<int, int>(10, 24);
  // inside3->z = 0;
  // inside3->fill(6, 6, CellType::FLOOR);
  // inside3->walls(CellType::WALL);
  // inside3->active = true;

  // view_map->regions.push_back(area);
  // view_map->regions.push_back(house);
  // view_map->regions.push_back(inside);
  // view_map->regions.push_back(inside2);
  // view_map->regions.push_back(inside3);
  // view_map->regions.push_back(roof);
  // view_map->regions.push_back(insideT);
  // view_map->regions.push_back(insideTr);
  view_map->tilesTextures = tiles;


  // auto hero = std
  auto spec = LocationSpec{"Dungeon"};
  spec.features.push_back(LocationFeature::TORCHES);
  spec.features.push_back(LocationFeature::STATUE);
  spec.features.push_back(LocationFeature::ALTAR);
  spec.features.push_back(LocationFeature::ICE);
  spec.features.push_back(LocationFeature::CORRUPT);
  auto l = generator->getLocation(spec);

  auto location = std::make_shared<Region>();
  location->cells = l->cells;
  location->position = {0,0};
  location->active = true;
  location->location = l;

  view_map->regions.push_back(location);

  auto bgColor = sf::Color(33, 33, 23);
  cacheTex = std::make_shared<sf::RenderTexture>();
  cacheTex->create(window->getSize().x, window->getSize().y);

  auto canvas = std::make_shared<sf::RenderTexture>();
  canvas->create(window->getSize().x, window->getSize().y);


  // auto bgThread = std::thread([&]() {
  //   sf::sleep(sf::milliseconds(2000));
  //   while(true) {
  //     for (auto ly = 0; ly < view_map->height; ly++) {
  //       for (auto lx = 0; lx < view_map->width; lx++) {
  //         auto t = view_map->getTile(lx, ly, view_map->z);
  //         if (!t->hasBackground) continue;
  //         t->bgColor = sf::Color(rand()%256, rand()%256, rand()%256, 60);
  //         t->hasBackground = true;
  //         damage.push_back({lx,ly});
  //       }
  //     }

  //     sf::sleep(sf::milliseconds(100));
  //   }
  // });

  window->clear(bgColor);
  cacheTex->clear(bgColor);
  canvas->clear(bgColor);
  redraws = 0;
  tilesUpdated = 0;
  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      processEvent(event);
    }

    float current_fps = ImGui::GetIO().Framerate;

    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(window->getSize().x,
    window->getSize().y));
    rectangle.setPosition(0, 0);

    if (needRedraw) {
      canvas->clear(bgColor);
      for (auto ly = 0; ly < view_map->height; ly++) {
        for (auto lx = 0; lx < view_map->width; lx++) {
          auto t = view_map->getTile(lx, ly, view_map->z);
          renderTile(canvas, t);
        }
      }
      needRedraw = false;
      canvas->display();
      cacheTex->display();
      rectangle.setTexture(&(canvas->getTexture()));
      redraws++;
    } else if (damage.size() > 0) {
      for (auto d : damage) {
        sf::RectangleShape bg;
        bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale);
        bg.setFillColor(bgColor);
        auto t = view_map->getTile(d.first, d.second, view_map->z);
        bg.setPosition(t->pos*scale);
        bg.move(-view_map->tileSet.size.first*view_map->position.first*scale,
                  -view_map->tileSet.size.second*view_map->position.second*scale );
        renderTile(canvas, t);
        canvas->display();
        cacheTex->display();
        rectangle.setTexture(&(canvas->getTexture()));
        tilesUpdated++;
      }
      damage.clear();
    } else {
      rectangle.setTexture(&(cacheTex->getTexture()));
    }

    window->draw(rectangle);

    ImGui::SFML::Update(*window, deltaClock.restart());
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    drawDocking();
    drawStatusBar(viewport->Size.x, 16.0f, 0.0f, viewport->Size.y - 24);
    drawMainWindow();

    ImGui::SFML::Render(*window);

    window->display();

    cacheTex->draw(rectangle);
    cacheTex->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}

void Application::renderTile(std::shared_ptr<sf::RenderTexture> canvas, std::shared_ptr<Tile> t) {
  sf::RectangleShape bg;
  if (t->hasBackground) {
    bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale);
    bg.setFillColor(t->bgColor);
    bg.setPosition(t->pos*scale);
    bg.move(-view_map->tileSet.size.first*view_map->position.first*scale,
              -view_map->tileSet.size.second*view_map->position.second*scale );
    if (t->sprites.size() == 0) {
      canvas->draw(bg);
    }
  }
  auto n = 0;
  for (auto sprite : t->sprites) {
    sprite->setPosition(t->pos*scale);
    sprite->move(-view_map->tileSet.size.first*view_map->position.first*scale,
              -view_map->tileSet.size.second*view_map->position.second*scale );
    auto s = *sprite;
    s.setScale(sf::Vector2f(scale, scale));
    canvas->draw(s);
    n++;
    if (n == t->bgLayer && t->hasBackground) {
      canvas->draw(bg);
    }
  }
}
