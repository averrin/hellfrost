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

#include <app/ui/tile.hpp>
#include <lss/game/cell.hpp>
#include <lss/generator/generator.hpp>

#include <app/style/theme.h>

// static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;
static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

Application::Application(std::string app_name, std::string version, int s)
    : APP_NAME(app_name), VERSION(version) {
  seed = s;
  fmt::print("Seed: {}\n", seed);
  srand(seed);
  setupGui();
}


//TODO: move to Application class
int x = -4;
int y = 10;
int z = 0;
float scale = 1.f;
bool cavern = false;
bool river = false;
bool lake = false;
bool torches = false;
bool corrupt = false;
int vW;
int vH;
int ts_idx = 0;
//TODO: read sets from folder
auto ts = std::vector<std::string>{"ascii", "vettlingr", "esoteric", "spacefox"};
std::optional<sf::Vector2<float>> lockedPos = std::nullopt;

void Application::setupGui() {

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  ImGui::CreateContext();
  Theme::Init();

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
    case sf::Keyboard::R:
      seed = rand();
      genLocation(seed);
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
    case sf::Event::MouseButtonPressed:
      if (event.mouseButton.button == sf::Mouse::Right) {
        lockedPos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
      } else
      if (event.mouseButton.button == sf::Mouse::Middle) {
        lockedPos = std::nullopt;
      }

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

    // fmt::print("measure win size\n");
    // auto ww = ImGui::GetWindowSize().x/view_map->tileSet.size.first;
    // auto wh = ImGui::GetWindowSize().y/view_map->tileSet.size.second;
    // fmt::print("{}.{}\n", ww, wh);
    // view_map->setSize( {ww, wh} );
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

void Application::drawCellInfo() {
    sf::Vector2<float> pos =
        window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    if (lockedPos) {
      pos = *lockedPos;
    }

    auto x = int(pos.x/(view_map->tileSet.size.first*scale));
    auto y = int(pos.y/(view_map->tileSet.size.second*scale));
    sf::RectangleShape bg;
    bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale);
    bg.setFillColor(sf::Color::Transparent);
    bg.setOutlineColor(sf::Color(200,124,70));
    auto rx = x+view_map->position.first;
    auto ry =  y+view_map->position.second;
    auto [cc, rz] = view_map->getCell(rx, ry, view_map->z);
    auto [region, _] = view_map->getRegion(rx, ry, rz);

    if (!cc || (*cc)->type == CellType::UNKNOWN) {
      bg.setOutlineColor(sf::Color(70, 70,70));
    }
    if (lockedPos) {
      bg.setOutlineColor(sf::Color(70, 70, 255));
    }
    bg.setOutlineThickness(1);
    bg.setPosition(sf::Vector2f(
                     x*view_map->tileSet.size.first,
                     y*view_map->tileSet.size.second
    )*scale);
    window->draw(bg);

    if (cc) {
      auto cell = *cc;
      ImGui::Begin("Current cell info");
      ImGui::Text("%s Position: %d.%d.%d\n", ICON_MDI_ARROW_ALL, cell->x, cell->y, rz);
      ImGui::Text("Type: %s\n", cell->type.name.c_str());
      ImGui::Text("PassThrough: %s\n", cell->passThrough ? ICON_MDI_CHECKBOX_MARKED_OUTLINE : ICON_MDI_CHECKBOX_BLANK_OUTLINE);
      ImGui::Text("SeeThrough: %s\n", cell->seeThrough ? ICON_MDI_CHECKBOX_MARKED_OUTLINE : ICON_MDI_CHECKBOX_BLANK_OUTLINE);
      ImGui::Text("Features: %d\n", cell->features.size());
      if (region) {
        auto objects = (*region)->location->getObjects(cell);
        drawObjects(objects);
      }
      ImGui::End();
    }
}

void Application::drawObjects(std::vector<std::shared_ptr<Object>> objects) {
      ImGui::Text("%s Objects: %d\n", ICON_MDI_CUBE_OUTLINE, objects.size());
      auto enemies = utils::castObjects<Enemy>(objects);
      auto n = 0;
      if (enemies.size() > 0) {
        if (ImGui::TreeNode("e", "%s Enemies", ICON_MDI_SWORD_CROSS)) {
          for (auto e : enemies) {
            if (ImGui::TreeNode((void*)(intptr_t)n, "%s %s", ICON_MDI_SWORD_CROSS, e->type.name.c_str())) {
                ImGui::Text("Position: %d.%d", e->currentCell->x, e->currentCell->y);
                ImGui::TreePop();
            }
            n++;
          }
                ImGui::TreePop();
        }
      }
      auto terrain = utils::castObjects<Terrain>(objects);
      if (terrain.size() > 0) {
        if (ImGui::TreeNode("t", "%s Terrain", ICON_MDI_CUBE)) {
          for (auto t : terrain) {
            if (ImGui::TreeNode((void*)(intptr_t)n, "%s %s", ICON_MDI_CUBE, t->type.name.c_str())) {
                ImGui::Text("Position: %d.%d", t->currentCell->x, t->currentCell->y);
                ImGui::TreePop();
            }
            n++;
          }
                ImGui::TreePop();
        }
      }
      auto items = utils::castObjects<Item>(objects);
      if (items.size() > 0) {
        if (ImGui::TreeNode("i", "%s Items", ICON_MDI_SWORD)) {
          for (auto i : items) {
            if (ImGui::TreeNode((void*)(intptr_t)n, "%s %s", ICON_MDI_SWORD, i->getTitle(true).c_str())) {
                ImGui::Text("Type: %s", i->type.name.c_str());
                ImGui::Text("Position: %d.%d", i->currentCell->x, i->currentCell->y);
                ImGui::TreePop();
            }
            n++;
          }
                ImGui::TreePop();
        }
      }
}

void Application::drawObjectsWindow() {
  ImGui::Begin("Objects");
  drawObjects(view_map->regions.front()->location->objects);
  ImGui::End();
}

void Application::drawMainWindow() {
  ImGui::Begin("Viewport");

  std::vector<const char*> _ts;
  std::transform(ts.begin(), ts.end(), std::back_inserter(_ts), [](auto s) {
    char*r = new char[s.size()+1];
    std::strcpy(r, s.c_str());
    return r;
  });
  if (ImGui::Combo("Tileset", &ts_idx, _ts.data(), ts.size())) {
    view_map->loadTileset(fmt::format("tilesets/{}", ts[ts_idx]));
    view_map->tilesCache.clear();
    needRedraw = true;
  }
  ImGui::Text("Cache len: %d\n", view_map->tilesCache.size());
  ImGui::Text("Redraws: %d\n", redraws);
  ImGui::Text("Tiles updated: %d\n\n", tilesUpdated);
  if (ImGui::SliderInt("viewport->width", &view_map->width, 10, 200)) {
    needRedraw = true;
    vW = view_map->width / scale;
    if (vW > 200) vW = 200;
  }
  if (ImGui::SliderInt("viewport->height", &view_map->height, 10, 200)) {
    needRedraw = true;
    vH = view_map->height / scale;
    if (vH > 200) vH = 200;
  }
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
  if (ImGui::SliderFloat("scale", &scale, 0.3f, 2.f)) {
    needRedraw = true;
    vW = view_map->width / scale;
    vH = view_map->height / scale;
    if (vW > 200) vW = 200;
    if (vH > 200) vH = 200;
  }

  ImGui::End();

  ImGui::Begin("Location");
  ImGui::Text("Seed: %d\n", seed);
  if (ImGui::Checkbox("cavern", &cavern)) {
    genLocation(seed);
    needRedraw = true;
  }
  if (ImGui::Checkbox("river", &river)) {
    genLocation(seed);
    needRedraw = true;
  }
  if (ImGui::Checkbox("lake", &lake)) {
    genLocation(seed);
    needRedraw = true;
  }
  if (ImGui::Checkbox("torches", &torches)) {
    genLocation(seed);
    needRedraw = true;
  }
  if (ImGui::Checkbox("corrupt", &corrupt)) {
    genLocation(seed);
    needRedraw = true;
  }


  // if (ImGui::Checkbox("house", &(view_map->regions[1]->active))) {
  //   view_map->tilesCache.clear(); //TODO: invalidate region cache only
  //   needRedraw = true;
  // }
  ImGui::End();
}

void Application::genLocation(int s) {
  auto label = "Generate location";
  log.start(label);
  auto spec = LocationSpec{"Dungeon"};
  spec.type = LocationType::DUNGEON;
  spec.threat = 1;
  if (cavern) {
    spec.type = LocationType::CAVERN;
    spec.floor = CellType::GROUND;
  }
  if (torches) {
  spec.features.push_back(LocationFeature::TORCHES);
  }
  // spec.features.push_back(LocationFeature::STATUE);
  // spec.features.push_back(LocationFeature::ALTAR);
  // spec.features.push_back(LocationFeature::ICE);
  if (corrupt) {
  spec.features.push_back(LocationFeature::CORRUPT);
  }
  if (river) {
    spec.features.push_back(LocationFeature::RIVER);
  }
  if (lake) {
    spec.features.push_back(LocationFeature::LAKE);
  }
  // spec.features.push_back(LocationFeature::VOID);
  srand(s);
  auto l = generator->getLocation(spec);
  log.stop(label);

  auto location = std::make_shared<Region>();
  location->cells = l->cells;
  location->position = {0,0};
  location->active = true;
  location->location = l;

  view_map->regions.clear();
  view_map->tilesCache.clear();
  view_map->regions.push_back(location);

}

int Application::serve() {
  log.info("serve");
  sf::Clock deltaClock;


  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();

  view_map =  std::make_shared<Viewport>();
  view_map->position = std::make_pair(x, y);

  vW = view_map->width / scale;
  vH = view_map->height / scale;

  genLocation(seed);

  view_map->loadTileset(fmt::format("tilesets/{}", ts[ts_idx]));

  auto c = Color::fromHexString(view_map->colors["PALETTE"]["BACKGROUND"]);
  auto bgColor = sf::Color(c.r, c.g, c.b, c.a);
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

    auto label = "frame";
    log.start(label, true);
    if (needRedraw) {
      canvas->clear(bgColor);
      log.start("tiles", true);
      for (auto ly = 0; ly < vH; ly++) {
        for (auto lx = 0; lx < vW; lx++) {
          auto t = view_map->getTile(lx, ly, view_map->z);
          if (t) {
            renderTile(canvas, *t);
          }
        }
      }
      log.stop("tiles", 50);
      needRedraw = false;
      canvas->display();
      cacheTex->display();
      rectangle.setTexture(&(canvas->getTexture()));
      redraws++;
    } else if (damage.size() > 0) {
      for (auto d : damage) {
        auto t = view_map->getTile(d.first, d.second, view_map->z);
        if (t) {
          sf::RectangleShape bg;
          bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale);
          bg.setFillColor(bgColor);
          bg.setPosition((*t)->pos*scale);
          bg.move(-view_map->tileSet.size.first*view_map->position.first*scale,
                    -view_map->tileSet.size.second*view_map->position.second*scale );
          renderTile(canvas, *t);
          canvas->display();
          cacheTex->display();
          rectangle.setTexture(&(canvas->getTexture()));
          tilesUpdated++;
        }
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
    drawCellInfo();
    drawMainWindow();
    drawObjectsWindow();

    ImGui::SFML::Render(*window);

    window->display();

    cacheTex->draw(rectangle);
    cacheTex->display();
    log.stop(label, 50);
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
    if (t->sprites.size() == 0 || t->bgLayer == 0) {
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
  sf::RectangleShape rectangle;
  rectangle.setOutlineColor(sf::Color(255,12,12));
  rectangle.setOutlineThickness(1);
  rectangle.setFillColor(sf::Color(255,12,12, 0));
  rectangle.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale);
  rectangle.setPosition(t->pos*scale);
    rectangle.move(-view_map->tileSet.size.first*view_map->position.first*scale,
              -view_map->tileSet.size.second*view_map->position.second*scale );
  // canvas->draw(rectangle);
}
