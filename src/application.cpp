#include <iostream>
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
#include "lss/generator/mapUtils.hpp"

#include <app/style/theme.h>

// static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;
static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

Application::Application(std::string app_name, std::string version, int s)
    : APP_NAME(app_name), VERSION(version) {
  seed = s;
  fmt::print("Seed: {}\n", seed);
  srand(seed);

  loadSpec();
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
int ts_idx = 0;
//TODO: read sets from folder
auto ts = std::vector<std::string>{"ascii", "vettlingr", "esoteric", "spacefox"};
std::optional<std::pair<int, int>> lockedPos = std::nullopt;

int lts_idx = 2;
auto lts = std::vector<std::string>{"dungeon", "cavern", "exterior"};
auto lt = std::vector<LocationType>{LocationType::DUNGEON, LocationType::CAVERN, LocationType::EXTERIOR};
auto locationType = LocationType::EXTERIOR;

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
  auto pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
  auto _x = int(pos.x/(view_map->tileSet.size.first*scale));
  auto _y = int(pos.y/(view_map->tileSet.size.second*scale));
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
    auto x = int(pos.x/(view_map->tileSet.size.first*scale));
    auto y = int(pos.y/(view_map->tileSet.size.second*scale));

    sf::RectangleShape bg;
    bg.setSize(sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second)*scale);
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

      std::map<CellFeature, std::string> features = {
        {CellFeature::BLOOD, "blood"},
        {CellFeature::CAVE, "cave"},
        {CellFeature::FROST, "frost"},
        {CellFeature::ACID, "acid"},
        {CellFeature::CORRUPT, "corrupt"}
      };

      for (auto f : cell->features) {
       ImGui::BulletText(features[f].c_str());
      }
      if (region) {
        auto objects = (*region)->location->getObjects(cell);
        drawObjects(objects);
      }

      ImGui::End();
    }
}

void Application::centerObject(std::shared_ptr<Object> o) {
    lockedPos = {o->currentCell->x, o->currentCell->y};
    x = o->currentCell->x-view_map->width/2;
    y = o->currentCell->y-view_map->height/2;
    view_map->position = {x, y};
    needRedraw = true;
}

void Application::drawObjects(std::vector<std::shared_ptr<Object>> objects) {
      ImGui::Text("%s Objects: %d\n", ICON_MDI_CUBE_OUTLINE, objects.size());
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
      auto terrain = utils::castObjects<Terrain>(objects);
      if (terrain.size() > 0) {
        if (ImGui::TreeNode("t", "%s Terrain", ICON_MDI_CUBE)) {
          for (auto t : terrain) {
            if (ImGui::TreeNode((void*)(intptr_t)n, "%s %s", ICON_MDI_CUBE, t->type.name.c_str())) {
                ImGui::BulletText("Position: %d.%d", t->currentCell->x, t->currentCell->y);
                ImGui::SameLine();
                if (ImGui::SmallButton(ICON_MDI_TARGET)) {
                  centerObject(t);
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
  ImGui::End();
}

void Application::saveTileset() {
  auto path = fmt::format("tilesets/{}", ts[ts_idx]);
  std::ofstream o(fmt::format("{}/colors.json", path));
  o << std::setw(4) << view_map->colors << std::endl;

  json j;
  j["TILEMAPS"] = view_map->tileSet.maps;
  j["SIZE"] = view_map->tileSet.size;
  j["GAP"] = view_map->tileSet.gap;
  j["SPRITES"] = view_map->tileSet.sprites;

  std::ofstream o2(fmt::format("{}/tiles.json", path));
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
    view_map->loadTileset(fmt::format("tilesets/{}", ts[ts_idx]));
    view_map->tilesCache.clear();
    needRedraw = true;
  }
  ImGui::BulletText("Size: %dx%d; gap: %d\n", view_map->tileSet.size.first, view_map->tileSet.size.second, view_map->tileSet.gap);
  ImGui::BulletText("Maps: %d\n", view_map->tileSet.maps.size());

  if(ImGui::Button("Reload")) {
    view_map->loadTileset(fmt::format("tilesets/{}", ts[ts_idx]));
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
            ImGui::Text(color.hexA().c_str());
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
        sf::Vector2f(view_map->tileSet.size.first, view_map->tileSet.size.second),
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
      ImGui::Text(view_map->tileSet.maps[n].c_str());
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
    genLocation(seed);
    needRedraw = true;
  }
  ImGui::SameLine();
  if(ImGui::Button("Reload")) {
    loadSpec();

    view_map->tilesCache.clear();
    genLocation(seed);
    needRedraw = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    saveSpec();
  }
  ImGui::Separator();

  if (ImGui::CollapsingHeader("Probabilities")) {
    for (auto [k, _] : generator->probabilities) {
      ImGui::SetNextItemWidth(80);
      ImGui::InputFloat(k.c_str(), &generator->probabilities[k]);
    }
  }
  ImGui::End();
}

void Application::saveSpec() {
    std::ofstream file(fmt::format("location.bin"));
    json j;
    j["PROBABILITIES"] = generator->probabilities;
    auto bj = json::to_bson(j);
    std::copy(bj.begin(), bj.end(), std::ostreambuf_iterator(file));
}

void Application::loadSpec() {
    std::ifstream file(fmt::format("location.bin"));
    std::istreambuf_iterator iter(file);
    std::vector<char> bj;
    std::copy(iter, std::istreambuf_iterator<char>{}, std::back_inserter(bj));
    json p = json::from_bson(bj);

    auto probs = p["PROBABILITIES"].get<std::map<std::string, float>>();
    generator = std::make_shared<Generator>(probs);
}

void Application::genLocation(int s) {
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
