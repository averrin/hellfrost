#include <iostream>
#include <thread>   // for access to this thread
#include <utility>   // for access to this thread
#include <chrono>   // for access to this thread

#include <app/application.hpp>

#include <imgui-sfml/imgui-SFML.h>
#include <imgui/imgui.h>

#include <libcolor/libcolor.hpp>
#include <app/fonts/material_design_icons.h>
#include <app/style/theme.h>
#include <SFML/Graphics.hpp>

#include <lss/game/cell.hpp>
#include <lss/generator/generator.hpp>
#include <lss/generator/mapUtils.hpp>

#include <app/style/theme.h>
#include <imgui-stl.hpp>

namespace hf = hellfrost;

// static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;
static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

#ifdef __APPLE__
  float GUI_SCALE = 2.f;
#else
  float GUI_SCALE = 1.f;
#endif

auto DATA_FILE = "game.bin";
auto DEFAULT_TILESET = "ascii";
int ts_idx = 0;
std::vector<std::string> ts;
Application::Application(std::string app_name, fs::path path, std::string version, int s)
    : APP_NAME(app_name), VERSION(version), PATH(path) {
  fmt::print("Seed: {}\n", seed);
  fmt::print("Path: {}\n", PATH.string());

  auto n = 0;
  for (auto entity : fs::directory_iterator(PATH / "tilesets")) {
    if (fs::is_directory(entity)) {
      auto tileset = entity.path().filename().string();
      if (tileset == DEFAULT_TILESET) {
        ts_idx = n;
      }
      ts.push_back(tileset);
      n++;
    }
  }

  entt::service_locator<Viewport>::empty();
  entt::service_locator<GameData>::empty();
  gm = std::make_unique<GameManager>(PATH / DATA_FILE);
  gm->setSeed(s);
  seed = gm->seed;
  setupGui();
}


//TODO: move to Application class
int x = -4;
int y = 10;
int z = 0;
float scale = 1.f;
bool cave_pass = false;
bool statue = false;
bool altar = false;
bool treasure = false;
bool heal = false;
bool mana = false;
bool river = false;
bool lake = false;
bool torches = false;
bool corrupt = false;
bool ice = false;
int vW;
int vH;
std::optional<std::pair<int, int>> lockedPos = std::nullopt;

int lts_idx = 2;
std::vector<std::string> lts = {"dungeon", "cavern", "exterior"};
auto lt = std::vector<LocationType>{LocationType::DUNGEON, LocationType::CAVERN, LocationType::EXTERIOR};
auto locationType = LocationType::EXTERIOR;
event_emitter emitter;

namespace Widgets {
	void Meta(hf::meta& m) {
      ImGui::InputText("Name", m.name);
      ImGui::InputText("Description", m.description);
      ImGui::InputText("ID", m.id);
  }
	void Position(hf::position& p) {
      ImGui::SetNextItemWidth(80*GUI_SCALE);
      if (ImGui::InputInt("x##Position", &p.x)) {
        emitter.publish<redraw_event>();
      }
      ImGui::SameLine();
      ImGui::SetNextItemWidth(80*GUI_SCALE);
      if(ImGui::InputInt("y##Position", &p.y)) {
        emitter.publish<redraw_event>();
      }
      ImGui::SameLine();
      ImGui::SetNextItemWidth(80*GUI_SCALE);
      if(ImGui::InputInt("z##Position", &p.z)) {
        emitter.publish<redraw_event>();
      }
      ImGui::SameLine();
    if(ImGui::Button(ICON_MDI_TARGET)) {
      emitter.publish<center_event>(p.x, p.y);
    }
	}
	void Renderable(hf::position& p, hf::renderable& r, std::shared_ptr<Viewport> view_map) {
    if (ImGui::Checkbox("Hidden", &r.hidden)) {
        emitter.publish<damage_event>(p.x, p.y);
    }
    auto k = r.spriteKey;
    auto v = view_map->tileSet.sprites[k];
      sf::Sprite s;
      s.setTexture(view_map->tilesTextures[v[0]]);
      s.setTextureRect(view_map->getTileRect(v[1], v[2]));
      ImGui::Image(s,
        sf::Vector2f(view_map->tileSet.size.first*GUI_SCALE, view_map->tileSet.size.second*GUI_SCALE),
        sf::Color::White, sf::Color::Transparent);
      ImGui::SameLine();
      std::vector<const char*> _ts;
      int s_idx = 0;
      auto n = 0;
      std::transform(view_map->tileSet.sprites.begin(), view_map->tileSet.sprites.end(), std::back_inserter(_ts), [&](auto sk) {
        if (sk.first == r.spriteKey) {
          s_idx = n;
        }
        char*r = new char[sk.first.size()+1];
        std::strcpy(r, sk.first.c_str());
        n++;
        return r;
      });
      if (ImGui::Combo("Sprite", &s_idx, _ts.data(), _ts.size())) {
       r.spriteKey = std::string(_ts[s_idx]);
        emitter.publish<damage_event>(p.x, p.y);
      }

      ImGui::SetNextItemWidth(150);
      char*fg = new char[r.fgColor.size()+1];
      std::strcpy(fg, r.fgColor.c_str());
      if (ImGui::InputText("Color", fg, 10)) {
        r.fgColor = std::string(fg);
        emitter.publish<damage_event>(p.x, p.y);
      }
      ImGui::SameLine();
      auto color = Color::fromHexString(r.fgColor);
      float col[4] = { color.r/255.f,  color.g/255.f, color.b/255.f, color.a/255.f,};
      if (ImGui::ColorEdit4(r.fgColor.c_str(), col, ImGuiColorEditFlags_NoInputs
                            | ImGuiColorEditFlags_NoLabel
                            | ImGuiColorEditFlags_AlphaPreview
                            | ImGuiColorEditFlags_AlphaBar)) {
        auto c = Color(col[0]*255, col[1]*255, col[2]*255, col[3]*255);
        r.fgColor = c.hexA();
        emitter.publish<damage_event>(p.x, p.y);
      }

      if(ImGui::Checkbox("Has BG", &r.hasBg)) {
        emitter.publish<damage_event>(p.x, p.y);
      }
      if (r.hasBg) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        char*bg = new char[r.bgColor.size()+1];
        std::strcpy(fg, r.bgColor.c_str());
        if (ImGui::InputText("BG Color", fg, 10)) {
          r.bgColor = std::string(fg);
          emitter.publish<damage_event>(p.x, p.y);
        }
        ImGui::SameLine();
        color = Color::fromHexString(r.bgColor);
        float bcol[4] = { color.r/255.f,  color.g/255.f, color.b/255.f, color.a/255.f,};
        if (ImGui::ColorEdit4(r.bgColor.c_str(), bcol, ImGuiColorEditFlags_NoInputs
                              | ImGuiColorEditFlags_NoLabel
                              | ImGuiColorEditFlags_AlphaPreview
                              | ImGuiColorEditFlags_AlphaBar)) {
          auto c = Color(col[0]*255, bcol[1]*255, bcol[2]*255, bcol[3]*255);
          r.bgColor = c.hexA();
          emitter.publish<damage_event>(p.x, p.y);
        }
      }
	}
} // Widgets

void Application::setupGui() {

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  ImGui::CreateContext();
  Theme::Init();

  ImGuiIO &io = ImGui::GetIO();
  io.FontGlobalScale = GUI_SCALE;

  window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), APP_NAME,
                                sf::Style::Default, settings);
  window->setVerticalSyncEnabled(true);

  ImGui::SFML::Init(*window);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  window->resetGLStates();

  editor.registerTrivial<hf::meta>(gm->registry, "Meta");
  editor.registerTrivial<hf::position>(gm->registry, "Position");
  editor.registerTrivial<hf::renderable>(gm->registry, "Renderable");

  editor.registerComponentWidgetFn(
    gm->registry.type<hf::meta>(),
    [&](entt::registry& reg, auto e) {
      auto& p = reg.get<hf::meta>(e);
      Widgets::Meta(p);
    });
  editor.registerComponentWidgetFn(
    gm->registry.type<hf::position>(),
    [&](entt::registry& reg, auto e) {
      auto& p = reg.get<hf::position>(e);
      Widgets::Position(p);
    });
  editor.registerComponentWidgetFn(
    gm->registry.type<hf::renderable>(),
    [&](entt::registry& reg, auto e) {
      auto& r = reg.get<hf::renderable>(e);
      auto& p = reg.get<hf::position>(e);
      Widgets::Renderable(p, r, view_map);
    });

  emitter.on<center_event>([&](const auto &p, auto &em){
    lockedPos = {p.x, p.y};
    x = p.x-view_map->width/2/GUI_SCALE;
    y = p.y-view_map->height/2/GUI_SCALE;
    view_map->position = {x, y};
    needRedraw = true;
  });

  emitter.on<damage_event>([&](const auto &p, auto &em){
    auto rx = p.x-view_map->position.first;
    auto ry = p.y-view_map->position.second;
    damage.push_back({rx,ry});
  });
  emitter.on<redraw_event>([&](const auto &p, auto &em){
    needRedraw = true;
  });
}

void Application::processEvent(sf::Event event) {
  auto pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
  auto _x = int(pos.x/(view_map->tileSet.size.first*scale*GUI_SCALE));
  auto _y = int(pos.y/(view_map->tileSet.size.second*scale*GUI_SCALE));
  auto rx = _x+view_map->position.first;
  auto ry = _y+view_map->position.second;


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
    case sf::Keyboard::F:
    {
      auto [cc, _z] = view_map->getCell(rx, ry, view_map->z);
      mapUtils::updateCell(*cc, CellType::FLOOR, (*cc)->features);
      view_map->tilesCache.clear();
      break;
    }
    case sf::Keyboard::W:
    {
      auto [cc, _z] = view_map->getCell(rx, ry, view_map->z);
      mapUtils::updateCell(*cc, CellType::WALL, (*cc)->features);
      view_map->tilesCache.clear();
      break;
    }
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
        lockedPos = {rx, ry};
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
        /*ImGuiWindowFlags_MenuBar | */ImGuiWindowFlags_NoDocking;

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
    ImGui::Begin("DockSpace", nullptr, window_flags);
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
  ImGui::SameLine(width - 64.0f*GUI_SCALE);
  Font font(Font::FAMILY_MONOSPACE);
  font.Normal().Regular().SmallSize();
  ImGui::PushFont(font.ImGuiFont());
  ImGui::Text("FPS: %ld", std::lround(ImGui::GetIO().Framerate));
  ImGui::PopFont();
  ImGui::End();
}

void Application::drawCellInfo() {
    sf::Vector2<float> pos =
        window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    auto x = int(pos.x/(view_map->tileSet.size.first*scale*GUI_SCALE));
    auto y = int(pos.y/(view_map->tileSet.size.second*scale*GUI_SCALE));

    sf::RectangleShape bg;
    bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale*GUI_SCALE);
    bg.setFillColor(sf::Color::Transparent);
    bg.setOutlineColor(sf::Color(200,124,70));
    auto rx = x+view_map->position.first;
    auto ry =  y+view_map->position.second;
    if (lockedPos) {
      rx = (*lockedPos).first;
      ry = (*lockedPos).second;
      x = rx - view_map->position.first;
      y = ry - view_map->position.second;
    }

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
    )*scale*GUI_SCALE);
    window->draw(bg);

    if (cc) {
      auto cell = *cc;
      ImGui::Begin("Current cell info");
      ImGui::Text("%s Position: %d.%d.%d\n", ICON_MDI_ARROW_ALL, cell->x, cell->y, rz);
      ImGui::Text("Type: %s\n", cell->type.name.c_str());
      ImGui::Text("PassThrough: %s\n", cell->passThrough ? ICON_MDI_CHECKBOX_MARKED_OUTLINE : ICON_MDI_CHECKBOX_BLANK_OUTLINE);
      ImGui::Text("SeeThrough: %s\n", cell->seeThrough ? ICON_MDI_CHECKBOX_MARKED_OUTLINE : ICON_MDI_CHECKBOX_BLANK_OUTLINE);
      ImGui::Text("Features: %lu\n", cell->features.size());

      std::map<CellFeature, std::string> features = {
        {CellFeature::BLOOD, "blood"},
        {CellFeature::CAVE, "cave"},
        {CellFeature::FROST, "frost"},
        {CellFeature::ACID, "acid"},
        {CellFeature::CORRUPT, "corrupt"}
      };

      for (auto f : cell->features) {
        ImGui::BulletText("%s", features[f].c_str());
      }
      if (region) {
        auto objects = (*region)->location->getObjects(cell);
        drawObjects(objects);
      }

      auto ents = gm->registry.view<hf::position>();

      if (ents.size() > 0) {
        if (ImGui::TreeNode("ents", "%s Entities", ICON_MDI_CUBE_OUTLINE)) {
          for (auto e : ents) {
            auto p = gm->registry.get<hf::position>(e);
            if (cell->x == p.x && cell->y == p.y) {
              drawEntityInfo(e);
            }
          }
          ImGui::TreePop();
        }
      }

      ImGui::End();
    }
}

void Application::centerObject(std::shared_ptr<Object> o) {
    lockedPos = {o->currentCell->x, o->currentCell->y};
    x = o->currentCell->x-view_map->width/2/GUI_SCALE;
    y = o->currentCell->y-view_map->height/2/GUI_SCALE;
    view_map->position = {x, y};
    needRedraw = true;
}

void Application::drawObjects(std::vector<std::shared_ptr<Object>> objects) {
      ImGui::Text("%s Objects: %lu\n", ICON_MDI_CUBE_OUTLINE, objects.size());
      auto enemies = utils::castObjects<Enemy>(objects);
      auto n = 0;
      if (enemies.size() > 0) {
        if (ImGui::TreeNode("e", "%s Enemies", ICON_MDI_SWORD_CROSS)) {
          for (auto e : enemies) {
            if (ImGui::TreeNode((void*)(intptr_t)n, "%s %s", ICON_MDI_SWORD_CROSS, e->type.name.c_str())) {
                ImGui::BulletText("Position: %d.%d", e->currentCell->x, e->currentCell->y);
                ImGui::SameLine();
                if (ImGui::SmallButton(ICON_MDI_TARGET)) {
                  centerObject(e);
                }
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
                ImGui::BulletText("Type: %s", i->type.name.c_str());
                ImGui::BulletText("Category: %s", i->type.category.name.c_str());
                ImGui::BulletText("Position: %d.%d", i->currentCell->x, i->currentCell->y);
                ImGui::SameLine();
                if (ImGui::SmallButton(ICON_MDI_TARGET)) {
                  centerObject(i);
                }
                ImGui::TreePop();
            }
            n++;
          }
          ImGui::TreePop();
        }
      }
      auto doors = utils::castObjects<Door>(objects);
      if (doors.size() > 0) {
        if (ImGui::TreeNode("d", "%s Doors", ICON_MDI_PLUS)) {
          for (auto d : doors) {
            if (ImGui::TreeNode((void*)(intptr_t)n, "%s %s", ICON_MDI_PLUS, "door")) {
                ImGui::BulletText("Hidden: %s", d->hidden ? ICON_MDI_CHECKBOX_MARKED_OUTLINE : ICON_MDI_CHECKBOX_BLANK_OUTLINE);
                ImGui::BulletText("Locked: %s", d->locked ? ICON_MDI_CHECKBOX_MARKED_OUTLINE : ICON_MDI_CHECKBOX_BLANK_OUTLINE);
                ImGui::BulletText("Position: %d.%d", d->currentCell->x, d->currentCell->y);
                ImGui::SameLine();
                if (ImGui::SmallButton(ICON_MDI_TARGET)) {
                  centerObject(d);
                }
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

  auto ents = gm->registry.view<hf::ingame>();
  if (ImGui::TreeNode("ents", "%s Entities: %lu", ICON_MDI_CUBE_OUTLINE, ents.size())) {
    if (ImGui::Button("New Entity")) {
      auto e = gm->registry.create();
      gm->registry.assign<hf::ingame>(e);
    }

    for (auto e : ents) {
      drawEntityInfo(e);
    }
    ImGui::TreePop();
  }
  ImGui::End();
}

void Application::drawEntityInfo(entt::entity e) {
    auto t = gm->registry.has<hf::meta>(e) ? gm->registry.get<hf::meta>(e).name.c_str() : "Entity";
    t = gm->registry.has<hf::ineditor>(e) ? gm->registry.get<hf::ineditor>(e).name.c_str() : t;
    auto icon = gm->registry.has<hf::ineditor>(e) ? gm->registry.get<hf::ineditor>(e).icon.c_str() : ICON_MDI_CUBE_OUTLINE;
    auto title = fmt::format("{} {}: {}", icon, t, (int)gm->registry.entity(e));
    if (ImGui::TreeNode(title.c_str())) {
      editor.renderImGui(gm->registry, e);
      ImGui::TreePop();
    }
}

void Application::saveTileset() {
  auto path = PATH / fs::path("tilesets") / ts[ts_idx];
  std::ofstream o(path / "colors.json");
  o << std::setw(4) << view_map->colors << std::endl;

  json j;
  j["TILEMAPS"] = view_map->tileSet.maps;
  j["SIZE"] = view_map->tileSet.size;
  j["GAP"] = view_map->tileSet.gap;
  j["SPRITES"] = view_map->tileSet.sprites;

  std::ofstream o2(path / "tiles.json");
  o2 << std::setw(4) << j << std::endl;
}

void Application::drawTilesetWindow() {
  ImGui::Begin("Tileset");
  std::vector<const char*> _ts;
  std::transform(ts.begin(), ts.end(), std::back_inserter(_ts), [](auto s) {
    char*r = new char[s.size()+1];
    std::strcpy(r, s.c_str());
    return r;
  });
  if (ImGui::Combo("Tileset", &ts_idx, _ts.data(), ts.size())) {
    auto path = PATH / fs::path("tilesets") / ts[ts_idx];
    view_map->loadTileset(path);
    view_map->tilesCache.clear();
    needRedraw = true;
  }
  ImGui::BulletText("Size: %dx%d; gap: %d\n", view_map->tileSet.size.first, view_map->tileSet.size.second, view_map->tileSet.gap);
  ImGui::BulletText("Maps: %lu\n", view_map->tileSet.maps.size());

  if(ImGui::Button("Reload")) {
    auto path = PATH / fs::path("tilesets") / ts[ts_idx];
    view_map->loadTileset(path);
    view_map->tilesCache.clear();
    needRedraw = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
   saveTileset();
  }
  ImGui::Separator();

  if (ImGui::TreeNode(fmt::format("Colors [{}]", view_map->colors.size()).c_str())) {
    for (auto& el : view_map->colors.items()) {
        if (ImGui::TreeNode(fmt::format("{} [{}]", el.key().c_str(), el.value().size()).c_str())) {
          for (auto& e : el.value().items()) {
            auto color = Color::fromHexString(e.value());
            float col[4] = { color.r/255.f,  color.g/255.f, color.b/255.f, color.a/255.f,};
            ImGui::Button(e.key().c_str());
            ImGui::SameLine(220);
            if (ImGui::ColorEdit4(e.key().c_str(), col, ImGuiColorEditFlags_NoInputs
                                  | ImGuiColorEditFlags_NoLabel
                                  | ImGuiColorEditFlags_AlphaPreview
                                  | ImGuiColorEditFlags_AlphaBar)) {
              auto c = Color(col[0]*255, col[1]*255, col[2]*255, col[3]*255);
              view_map->colors[el.key()][e.key()] = c.hexA();
              view_map->tilesCache.clear();
              needRedraw = true;
            }
            ImGui::SameLine();
            ImGui::Text("%s", color.hexA().c_str());
          }
          ImGui::TreePop();
        }
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode(fmt::format("Sprites [{}]", view_map->tileSet.sprites.size()).c_str())) {
    for (auto [k, v] : view_map->tileSet.sprites) {
      sf::Sprite s;
      s.setTexture(view_map->tilesTextures[v[0]]);
      s.setTextureRect(view_map->getTileRect(v[1], v[2]));
      ImGui::Image(s,
        sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*GUI_SCALE,
        sf::Color::White, sf::Color::Transparent);
      ImGui::SameLine();
      ImGui::Button(k.c_str());
      ImGui::SameLine(220);
      ImGui::SetNextItemWidth(80);
      if (ImGui::InputInt(fmt::format("##{}{}", k, 0).c_str(), &(view_map->tileSet.sprites[k][0]))) {
        view_map->tilesCache.clear();
        needRedraw = true;
      }
      ImGui::SameLine();
      ImGui::SetNextItemWidth(80);
      if (ImGui::InputInt(fmt::format("##{}{}", k, 1).c_str(), &(view_map->tileSet.sprites[k][1]))) {
        view_map->tilesCache.clear();
        needRedraw = true;
      }
      ImGui::SameLine();
      ImGui::SetNextItemWidth(80);
      if (ImGui::InputInt(fmt::format("##{}{}", k, 2).c_str(), &(view_map->tileSet.sprites[k][2]))) {
        view_map->tilesCache.clear();
        needRedraw = true;
      }
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Preview")) {
    auto n = 0;
    for (auto t : view_map->tilesTextures) {
      auto size = t.getSize();
      sf::Sprite s;
      s.setTexture(view_map->tilesTextures[n]);
      ImGui::Text("%s", view_map->tileSet.maps[n].c_str());
      ImGui::Image(s,
        sf::Vector2f(size.x, size.y),
        sf::Color::White, sf::Color::Transparent);
      ImGui::Text("\n");
      n++;
    }
    ImGui::TreePop();
  }

  ImGui::End();

}

void Application::drawMainWindow() {
  ImGui::Begin("Viewport");
  ImGui::Text("Cache len: %lu\n", view_map->tilesCache.size());
  ImGui::Text("Redraws: %d\n", redraws);
  ImGui::Text("Tiles updated: %d\n\n", tilesUpdated);
  if (ImGui::SliderInt("viewport->width", &view_map->width, 10, 200)) {
    needRedraw = true;
    vW = view_map->width / scale*GUI_SCALE;
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
  ImGui::SameLine();
  if (ImGui::SmallButton(ICON_MDI_FORMAT_SIZE)) {
    scale = 1;
    vW = view_map->width / scale;
    vH = view_map->height / scale;
    needRedraw = true;
  }

  ImGui::End();

  ImGui::Begin("Location");
  if (ImGui::InputInt("Seed", &seed)) {
    genLocation(seed);
    needRedraw = true;
  }
  ImGui::SameLine();
  if (ImGui::SmallButton(ICON_MDI_DICE_3)) {
    seed = rand();
    genLocation(seed);
    needRedraw = true;
  }
  ImGui::Separator();
  std::vector<const char*> _lts;
  std::transform(lts.begin(), lts.end(), std::back_inserter(_lts), [](auto s) {
    char*r = new char[s.size()+1];
    std::strcpy(r, s.c_str());
    return r;
  });
  if (ImGui::Combo("Location type", &lts_idx, _lts.data(), lts.size())) {
    locationType = lt[lts_idx];
    genLocation(seed);
    needRedraw = true;
  }

  ImGui::Text("\nFeatures\n");
  if (locationType == LocationType::CAVERN) {
    if (ImGui::Checkbox("river", &river) || ImGui::Checkbox("lake", &lake)) {
      genLocation(seed);
      needRedraw = true;
    }
  } else if (locationType == LocationType::EXTERIOR) {
  } else {
    river = false;
    lake = false;
    if (ImGui::Checkbox("cave_pass", &cave_pass)) {
      genLocation(seed);
      needRedraw = true;
    }
  }
  if (locationType != LocationType::EXTERIOR) {
    if (ImGui::Checkbox("torches", &torches)
        || ImGui::Checkbox("statue", &statue)
        || ImGui::Checkbox("altar", &altar)
        || ImGui::Checkbox("treasure", &treasure)
        || ImGui::Checkbox("heal", &heal)
        || ImGui::Checkbox("mana", &mana)
        || ImGui::Checkbox("ice", &ice)
        || ImGui::Checkbox("corrupt", &corrupt)) {
      genLocation(seed);
      needRedraw = true;
    }
  }
  ImGui::End();

  ImGui::Begin("Specification");
  if(ImGui::Button("Apply")) {
    view_map->tilesCache.clear();
    // gm->reset();
    genLocation(seed);
    needRedraw = true;
  }
  ImGui::SameLine();
  if(ImGui::Button("Reload")) {
    gm->loadData();
    // gm->reset();

    view_map->tilesCache.clear();
    genLocation(seed);
    needRedraw = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    gm->saveData();
  }
  ImGui::Separator();


  auto data = entt::service_locator<GameData>::get().lock();

  if (ImGui::CollapsingHeader("Probabilities")) {
    for (auto& [k, _] : data->probability) {
      ImGui::SetNextItemWidth(80);
      ImGui::InputFloat(k.c_str(), &data->probability[k]);
    }
  }
  if (ImGui::CollapsingHeader("Item specs")) {
    for (auto& [k, spec] : data->itemSpecs) {
      char*name = new char[spec.name.size()+1];
      std::strcpy(name, spec.name.c_str());
      if (ImGui::TreeNode(k.c_str())) {
        ImGui::Button("Delete");
        if (ImGui::InputText("Name", name, spec.name.size()+1)) {
        }
        ImGui::Text("Category: %s", spec.category.name.c_str());
        ImGui::InputInt("Durability", &spec.durability);
        ImGui::Checkbox("Identified", &spec.identified);
        ImGui::TreePop();
      }
    }
  }
  if (ImGui::CollapsingHeader("Terrain specs")) {
    for (auto& [k, spec] : data->terrainSpecs) {
      if (ImGui::TreeNode(k.c_str())) {
        ImGui::Button("Delete");
        ImGui::InputText("Name", spec.name);
        ImGui::InputText("Sign", spec.sign);
        ImGui::Checkbox("seeThrough", &spec.seeThrough);
        ImGui::SameLine();
        ImGui::Checkbox("passThrough", &spec.passThrough);
        ImGui::InputInt("action points left", &spec.apLeft);
        ImGui::Checkbox("destructable", &spec.destructable);
        if (spec.light.distance > 0) {
          if (ImGui::CollapsingHeader("Light specs")) {
            ImGui::BulletText("Light distance: %f", spec.light.distance);
            ImGui::BulletText("Light type: %d", spec.light.type);
            ImGui::BulletText("Light stable: %s", spec.light.stable ? "true" : "false");
          }
        }
        ImGui::TreePop();
      }
    }
  }
  ImGui::End();
}

void Application::genLocation(int s) {
  gm->setSeed(s);

  auto label = "Generate location";
  log.start(label);
  auto spec = LocationSpec{"Dungeon"};
  spec.type = LocationType::DUNGEON;
  spec.threat = 1;
  if (locationType == LocationType::CAVERN) {
    spec.type = LocationType::CAVERN;
    spec.floor = CellType::GROUND;
    spec.cellFeatures = {CellFeature::CAVE};
    cave_pass = false;
  } else if (locationType == LocationType::EXTERIOR) {
    spec.type = LocationType::EXTERIOR;
    spec.floor = CellType::GROUND;
    cave_pass = false;
    river = false;
    lake = false;
  } else
  if (locationType == LocationType::DUNGEON) {
    spec.type = LocationType::DUNGEON;
    spec.floor = CellType::FLOOR;
    river = false;
    lake = false;
  }

  std::vector<bool> flags = {
    cave_pass, statue,altar,
    treasure,heal,
    mana,river,
    lake,torches,
    corrupt,ice,};

  std::vector<LocationFeature> features = {
    LocationFeature::CAVE_PASSAGE,
    LocationFeature::STATUE,
    LocationFeature::ALTAR,
    LocationFeature::TREASURE_SMALL,
    LocationFeature::HEAL,
    LocationFeature::MANA,
    LocationFeature::RIVER,
    LocationFeature::LAKE,
    LocationFeature::TORCHES,
    LocationFeature::CORRUPT,
    LocationFeature::ICE,
  };

  auto n = 0;
  for (auto flag : flags) {
    if (flag) {
      spec.features.push_back(features[n]);
    }
    n++;
  }

  gm->gen(spec);
  log.stop(label);
  view_map->regions.clear();
  view_map->tilesCache.clear();

  auto location = std::make_shared<Region>();
  location->cells = gm->location->cells;
  location->position = {0,0};
  location->active = true;
  location->location = gm->location;
  view_map->regions.push_back(location);
}

int Application::serve() {
  log.info("serve");
  sf::Clock deltaClock;

  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();

  view_map = std::make_shared<Viewport>();

  entt::service_locator<Viewport>::set(view_map);
  view_map->position = std::make_pair(x, y);

  vW = view_map->width / scale;
  vH = view_map->height / scale;

  view_map->loadTileset(PATH / "tilesets" / ts[ts_idx]);
  genLocation(seed);

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
      std::list<int> h(vH);
      std::iota(h.begin(), h.end(), 0);
      std::list<int> w(vW);
      std::iota(w.begin(), w.end(), 0);
// #ifdef __APPLE__
      for (auto ly : h) {
        for (auto lx : w) {
          auto t = view_map->getTile(lx, ly, view_map->z);
          if (t) {
            renderTile(canvas, *t);
          }
        }
      }

      auto ents = gm->registry.group(entt::get<hf::position, hf::renderable>);
      for (auto e : ents) {
        renderEntity(canvas, e);
      }
// #else
//       #include <execution>
//       #include <algorithm>
//       std::for_each(std::execution::par, h.begin(), h.end(), [&](auto ly){
//         std::for_each(std::execution::par, w.begin(), w.end(), [&, =ly](auto lx){
//           auto t = view_map->getTile(lx, ly, view_map->z);
//           if (t) {
//             renderTile(canvas, *t);
//           }
//         });
//       });
// #endif
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
          bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale*GUI_SCALE);
          bg.setFillColor(bgColor);
          bg.setPosition((*t)->pos*scale*GUI_SCALE);
          bg.move(-view_map->tileSet.size.first*view_map->position.first*scale*GUI_SCALE,
                    -view_map->tileSet.size.second*view_map->position.second*scale*GUI_SCALE );
          canvas->draw(bg);
          renderTile(canvas, *t);

          auto ents = gm->registry.group(entt::get<hf::position, hf::renderable>);
          for (auto e : ents) {
            auto p = gm->registry.get<hf::position>(e);
            // fmt::print("{}.{} != {}.{}\n", p.x, p.y, d.first, d.second);
            auto rx = p.x-view_map->position.first;
            auto ry = p.y-view_map->position.second;
            if (rx == d.first && ry == d.second) {
              renderEntity(canvas, e);
              break;
            }
          }
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
    drawStatusBar(viewport->Size.x, 16.0f*GUI_SCALE, 0.0f, viewport->Size.y - 24);
    drawCellInfo();
    drawMainWindow();
    drawTilesetWindow();
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

void Application::renderEntity(std::shared_ptr<sf::RenderTexture> canvas, entt::entity e) {
    auto [pos, render] = gm->registry.get<hf::position, hf::renderable>(e);
    if (render.hidden) return;
    if (render.hasBg) {
      sf::RectangleShape bg;
      bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale*GUI_SCALE);
      bg.setFillColor(view_map->getColor(render.bgColor));
      bg.setPosition(sf::Vector2f(pos.x*view_map->tileSet.size.first, pos.y*view_map->tileSet.size.second)*scale*GUI_SCALE);
      bg.move(-view_map->tileSet.size.first*view_map->position.first*scale*GUI_SCALE,
              -view_map->tileSet.size.second*view_map->position.second*scale*GUI_SCALE );
      canvas->draw(bg);
    }
    auto sprite = view_map->makeSprite("", render.spriteKey);
    sprite->setColor(view_map->getColor(render.fgColor));
    sprite->setPosition(sf::Vector2f(pos.x*view_map->tileSet.size.first, pos.y*view_map->tileSet.size.second)*scale*GUI_SCALE);
    sprite->move(-view_map->tileSet.size.first*view_map->position.first*scale*GUI_SCALE,
              -view_map->tileSet.size.second*view_map->position.second*scale*GUI_SCALE );
    auto s = *sprite;
    s.setScale(sf::Vector2f(scale*GUI_SCALE, scale*GUI_SCALE));
    canvas->draw(s);
}

void Application::renderTile(std::shared_ptr<sf::RenderTexture> canvas, std::shared_ptr<Tile> t) {
  sf::RectangleShape bg;
  if (t->hasBackground) {
    bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale*GUI_SCALE);
    bg.setFillColor(t->bgColor);
    bg.setPosition(t->pos*scale*GUI_SCALE);
    bg.move(-view_map->tileSet.size.first*view_map->position.first*scale*GUI_SCALE,
              -view_map->tileSet.size.second*view_map->position.second*scale*GUI_SCALE );
    if (t->sprites.size() == 0 || t->bgLayer == 0) {
      canvas->draw(bg);
    }
  }
  auto n = 0;
  for (auto sprite : t->sprites) {
    sprite->setPosition(t->pos*scale*GUI_SCALE);
    sprite->move(-view_map->tileSet.size.first*view_map->position.first*scale*GUI_SCALE,
              -view_map->tileSet.size.second*view_map->position.second*scale*GUI_SCALE );
    auto s = *sprite;
    s.setScale(sf::Vector2f(scale*GUI_SCALE, scale*GUI_SCALE));
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
  rectangle.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale*GUI_SCALE);
  rectangle.setPosition(t->pos*scale*GUI_SCALE);
    rectangle.move(-view_map->tileSet.size.first*view_map->position.first*scale*GUI_SCALE,
              -view_map->tileSet.size.second*view_map->position.second*scale*GUI_SCALE );
  // canvas->draw(rectangle);
}

Application::~Application() {
  entt::service_locator<GameData>::reset();
  entt::service_locator<Viewport>::reset();
}
