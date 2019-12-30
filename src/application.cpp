#include <chrono>
#include <iostream>
#include <regex>
#include <thread>
#include <utility>

#include <app/application.hpp>

#include <imgui-sfml/imgui-SFML.h>
#include <imgui/imgui.h>

#include <SFML/Graphics.hpp>
#include <app/fonts/material_design_icons.h>
#include <app/style/theme.h>
#include <libcolor/libcolor.hpp>

#include <lss/game/cell.hpp>
#include <lss/generator/generator.hpp>
#include <lss/generator/mapUtils.hpp>

#include <app/style/theme.h>
#include <imgui-stl.hpp>

#include <app/editor.hpp>
#include <app/ui/drawEngine.hpp>

#include "duk_console.h"

#ifdef __APPLE__
float GUI_SCALE = 2.0f;
#else
float GUI_SCALE = 1.f;
#endif

// static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;
static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

void duk_log_ev(const std::string msg) {
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  emitter->publish<log_event>(msg);
}

void duk_regen(const int s) {
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  emitter->publish<regen_event>(s);
}

void duk_sleep(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Application::duk_log(const std::string msg) { duk_log_ev(msg); }

// TODO: add config file
auto DEFAULT_TILESET = "ascii";
auto DATA_FILE = "game.bin";
Application::Application(std::string app_name, fs::path path,
                         std::string version, int s)
    : APP_NAME(app_name), VERSION(version), PATH(path) {
  fmt::print("Seed: {}\n", seed);
  fmt::print("Path: {}\n", PATH.string());

  entt::monostate<"gui_scale"_hs>{} = GUI_SCALE;
  entt::monostate<"path"_hs>{} = PATH.string().data();

  entt::service_locator<entt::registry>::empty();
  entt::service_locator<event_emitter>::set(std::make_shared<event_emitter>());
  entt::service_locator<Viewport>::empty();
  entt::service_locator<GameData>::empty();
  entt::service_locator<GameData>::empty();
  entt::service_locator<std::mutex>::set(reg_mutex);

  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();
  viewport = std::make_shared<Viewport>();
  entt::service_locator<Viewport>::set(viewport);
  viewport->loadTileset(PATH / "tilesets" / DEFAULT_TILESET);
  engine = std::make_unique<DrawEngine>(window);
  engine->vW = viewport->width / viewport->scale;
  engine->vH = viewport->height / viewport->scale;

  entt::service_locator<DrawEngine>::set(engine);
  gm = std::make_shared<GameManager>(PATH / DATA_FILE);
  editor = std::make_unique<Editor>(gm, PATH);
  gm->setSeed(s);
  seed = gm->seed;
  setupGui();

  // TODO: move to separte class
  duk_console_init(duk_ctx, 0 /*flags*/);
  dukglue_register_function(duk_ctx, &duk_log_ev, "log");
  dukglue_register_function(duk_ctx, &duk_sleep, "sleep");
  dukglue_register_function(duk_ctx, &duk_regen, "regen");

  dukglue_register_global(duk_ctx, engine, "engine");
  dukglue_register_method(duk_ctx, &DrawEngine::invalidate, "invalidate");

  auto fileToEdit = PATH / "init.js";

  {
    std::ifstream t(fileToEdit);
    if (t.good()) {
      std::string str((std::istreambuf_iterator<char>(t)),
                      std::istreambuf_iterator<char>());
      duk_editor.SetText(str);
    }
  }
}

void Application::duk_exec(const char *code) {
  if (duk_peval_string(duk_ctx, code) != 0) {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    duk_get_prop_string(duk_ctx, -1, "stack");
    auto err = duk_safe_to_string(duk_ctx, -1);
    log.error(lu::red("DUK"),
              fmt::format("Error running '{}':\n{}", code, err));
    duk_pop(duk_ctx);
    emitter->publish<duk_error>(err);
  }
}

// TODO: remove by generating proper location in gm
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

int cache_count = 0;

// TODO: remove by generating proper location in gm
int lts_idx = 2;
std::vector<std::string> lts = {"dungeon", "cavern", "exterior"};
auto lt = std::vector<LocationType>{LocationType::DUNGEON, LocationType::CAVERN,
                                    LocationType::EXTERIOR};
auto locationType = lt[lts_idx];

void Application::setupGui() {

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  ImGui::CreateContext();
  Theme::Init();

  ImGuiIO &io = ImGui::GetIO();
  io.FontGlobalScale = GUI_SCALE;
  ImGui::GetStyle().ScaleAllSizes(GUI_SCALE);

  window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), APP_NAME,
                                sf::Style::Default, settings);
  window->setVerticalSyncEnabled(true);

  ImGui::SFML::Init(*window);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  window->resetGLStates();

  auto emitter = entt::service_locator<event_emitter>::get().lock();

  emitter->on<clear_markers_event>([&](const auto &event, auto &em) {
    markers.clear();
    duk_editor.SetErrorMarkers(markers);
  });

  emitter->on<log_event>([&](const auto &event, auto &em) {
    log.info(lu::yellow("DUK"), event.msg);
    console.AddLog(event.msg.data());
  });

  emitter->on<regen_event>([&](const auto &p, auto &em) {
    if (p.seed == -1) {
      seed = rand();
    } else if (p.seed >= 0) {
      seed = p.seed;
    }
    viewport->view_x = 0;
    viewport->view_y = 0;
    viewport->view_z = 0;
    genLocation(seed);
    engine->invalidate();
  });
  emitter->on<center_event>([&](const auto &p, auto &em) {
    // lockedPos = {p.x, p.y};
    viewport->view_x = p.x - viewport->width / 2 / GUI_SCALE;
    viewport->view_y = p.y - viewport->height / 2 / GUI_SCALE;
    engine->invalidate();
  });

  emitter->on<redraw_event>(
      [&](const auto &p, auto &em) { engine->invalidate(); });

  emitter->on<resize_event>([&](const auto &p, auto &em) {
    sf::FloatRect visibleArea(0, 0, window->getSize().x, window->getSize().y);
    auto sv = sf::View(visibleArea);
    window->setView(sv);
    viewport->width = window->getSize().x / viewport->tileSet.size.first + 1;
    viewport->height = window->getSize().y / viewport->tileSet.size.second + 1;
    engine->vW = viewport->width / viewport->scale / GUI_SCALE;
    engine->vH = viewport->height / viewport->scale / GUI_SCALE;
    // engine->tilesCache.clear();
    engine->resize(window->getSize());
    rectangle.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
    cursor.setSize(sf::Vector2f(viewport->tileSet.size.first,
                                viewport->tileSet.size.second) *
                   viewport->scale * GUI_SCALE);
    engine->invalidate();
  });

  emitter->on<duk_error>([&](auto event, auto &em) {
    std::string err = std::string(event.msg);
    std::string line;
    std::vector<std::string> lines;
    std::istringstream ss(err);
    std::regex re("eval:(\\d+)");
    std::smatch m;
    while (std::getline(ss, line, '\n')) {
      lines.push_back(line);
    }
    std::regex_search(lines.back(), m, re);

    markers.insert(std::make_pair<int, std::string>(
        std::atoi(std::string(m[m.size() - 1]).data()),
        std::string(lines.front())));
    duk_editor.SetErrorMarkers(markers);

    console.AddLog("[error] %s", event.msg.data());
  });

  emitter->on<exec_event>(
      [&](auto event, auto &em) { duk_exec(event.code.data()); });
}

void Application::processEvent(sf::Event event) {
  current_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
  auto _x = int(current_pos.x /
                (viewport->tileSet.size.first * viewport->scale * GUI_SCALE));
  auto _y = int(current_pos.y /
                (viewport->tileSet.size.second * viewport->scale * GUI_SCALE));
  auto rx = _x + viewport->view_x;
  auto ry = _y + viewport->view_y;

  ImGui::SFML::ProcessEvent(event);
  switch (event.type) {
  case sf::Event::KeyPressed:
    switch (event.key.code) {
    case sf::Keyboard::Escape:
      window->close();
      break;
    case sf::Keyboard::Right:
      viewport->view_x += 1;
      engine->invalidate();
      break;
    case sf::Keyboard::Left:
      viewport->view_x -= 1;
      engine->invalidate();
      break;
    case sf::Keyboard::Up:
      if (!event.key.control) {
        viewport->view_y -= 1;
      } else {
        viewport->view_z += 1;
      }
      engine->invalidate();
      break;
    case sf::Keyboard::Down:
      if (!event.key.control) {
        viewport->view_y += 1;
      } else {
        viewport->view_z -= 1;
      }
      engine->invalidate();
      break;
    case sf::Keyboard::F1:
      debug = !debug;
      break;
    }
    break;
  case sf::Event::Resized: {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    emitter->publish<resize_event>();
  }
  // case sf::Event::LostFocus:
  case sf::Event::GainedFocus:
    engine->invalidate();
    break;
  case sf::Event::Closed:
    window->close();
    break;
  case sf::Event::MouseButtonPressed:
    if (event.mouseButton.button == sf::Mouse::Right) {
      lockedPos = {rx, ry};
    } else if (event.mouseButton.button == sf::Mouse::Middle) {
      lockedPos = std::nullopt;
    }

    break;
  }
}

void Application::drawDocking(float padding) {
  ImGuiViewport *vp = ImGui::GetMainViewport();
  ImGuiWindowFlags window_flags =
      /*ImGuiWindowFlags_MenuBar | */ ImGuiWindowFlags_NoDocking;

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
  // render our background and handle the pass-thru hole, so we ask Begin() to
  // not render a background.
  auto dockSpaceSize = vp->Size;
  dockSpaceSize.y -= padding; // remove the status bar
  ImGui::SetNextWindowPos(vp->Pos);
  ImGui::SetNextWindowSize(dockSpaceSize);
  ImGui::SetNextWindowViewport(vp->ID);
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
  ImGui::SameLine(8.0f * GUI_SCALE);
  Font font(Font::FAMILY_MONOSPACE);
  font.Normal().Regular().SmallSize();
  ImGui::PushFont(font.ImGuiFont());
  auto fps = std::lround(ImGui::GetIO().Framerate);
  if (fps > 55) {
    ImGui::Text("FPS: %ld", fps);
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text,
                          (ImVec4)ImColor::HSV(1.f / 7.f, 0.86f, 1.0f));
    ImGui::Text("FPS: %ld", fps);
    ImGui::PopStyleColor(1);
  }
  ImGui::SameLine();
  current_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
  auto _x = int(current_pos.x /
                (viewport->tileSet.size.first * viewport->scale * GUI_SCALE));
  auto _y = int(current_pos.y /
                (viewport->tileSet.size.second * viewport->scale * GUI_SCALE));
  ImGui::Text("|  pos: %d.%d", _x, _y);
  auto rx = _x + viewport->view_x;
  auto ry = _y + viewport->view_y;
  ImGui::SameLine();
  ImGui::Text("|  cell: %d.%d", rx, ry);
  ImGui::SameLine();
  auto cache_full =
      gm->location->cells.front().size() * gm->location->cells.size();
  if (cache_count / float(cache_full) != 1.f) {
    ImGui::Text("|  cache:");
    ImGui::SameLine();
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

    ImGui::AlignTextToFramePadding();
    ImGui::BufferingBar("##buffer_bar",
                        cache_count / float(cache_full),
                        ImVec2(100*GUI_SCALE, 3*GUI_SCALE), bg, col);
  } else {
    ImGui::Text("|  cache: %d/%lu", cache_count, cache_full);
  }
  ImGui::PopFont();
  ImGui::End();
}

void Application::drawViewportWindow() {
  if (!ImGui::Begin("Viewport")) {
    ImGui::End();
    return;
  }

  auto cache_full =
      gm->location->cells.front().size() * gm->location->cells.size();
  ImGui::Text("Cache len: %d/%lu", cache_count, cache_full);
  ImGui::Text("Redraws: %d", engine->redraws);
  ImGui::Text("Tiles updated: %d", engine->tilesUpdated);
  ImGui::Text("Objects in render: %d\n", engine->layers->size());
  for (auto [k, l] : engine->layers->layers) {
    ImGui::AlignTextToFramePadding();
    ImGui::BulletText(fmt::format("{}: {}", k, l->children.size()).c_str());
    ImGui::SameLine();
    if (ImGui::Checkbox(fmt::format("##{}", k).c_str(), &l->enabled)) {
      engine->invalidate();
    }
  }
  ImGui::Text("\n");
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  if (ImGui::SliderInt("width", &viewport->width, 10, 200)) {
    engine->invalidate();
    engine->vW = viewport->width / viewport->scale / GUI_SCALE;
    if (engine->vW > 200)
      engine->vW = 200;
  }
  if (ImGui::SliderInt("height", &viewport->height, 10, 200)) {
    engine->invalidate();
    engine->vH = viewport->height / viewport->scale / GUI_SCALE;
    if (engine->vH > 200)
      engine->vH = 200;
  }
  if (ImGui::SliderInt("x", &viewport->view_x, -viewport->width, gm->location->cells.front().size())) {
    engine->invalidate();
  }
  if (ImGui::SliderInt("y", &viewport->view_y, -viewport->height, gm->location->cells.size())) {
    engine->invalidate();
  }
  if (ImGui::SliderInt("z", &viewport->view_z, -10, 10)) {
    engine->invalidate();
  }
  if (ImGui::SliderFloat("scale", &viewport->scale, 0.3f, 2.f)) {
    engine->vW = viewport->width / viewport->scale / GUI_SCALE;
    engine->vH = viewport->height / viewport->scale / GUI_SCALE;
    if (engine->vW > 200)
      engine->vW = 200;
    if (engine->vH > 200)
      engine->vH = 200;
    emitter->publish<resize_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_FORMAT_SIZE)) {
    viewport->scale = 1;
    emitter->publish<resize_event>();
  }

  ImGui::End();
}

void Application::drawLocationWindow() {
  if (!ImGui::Begin("Location")) {
    ImGui::End();
    return;
  }

  if (ImGui::InputInt("Seed", &seed)) {
    genLocation(seed);
    engine->invalidate();
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_DICE_3)) {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    emitter->publish<regen_event>(-1);
  }
  ImGui::Separator();
  if (ImGui::Combo("Location type", &lts_idx, lts)) {
    locationType = lt[lts_idx];
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    emitter->publish<regen_event>(seed);
  }

  if (locationType != LocationType::EXTERIOR) {
    ImGui::Text("\nFeatures\n");
  } else {
    ImGui::Text("\nNo features yet\n");
  }
  if (locationType == LocationType::CAVERN) {
    if (ImGui::Checkbox("river", &river) || ImGui::Checkbox("lake", &lake)) {
      genLocation(seed);
      engine->invalidate();
    }
  } else if (locationType == LocationType::EXTERIOR) {
  } else {
    river = false;
    lake = false;
    if (ImGui::Checkbox("cave_pass", &cave_pass)) {
      genLocation(seed);
      engine->invalidate();
    }
  }
  if (locationType != LocationType::EXTERIOR) {
    if (ImGui::Checkbox("torches", &torches) ||
        ImGui::Checkbox("statue", &statue) ||
        ImGui::Checkbox("altar", &altar) ||
        ImGui::Checkbox("treasure", &treasure) ||
        ImGui::Checkbox("heal", &heal) || ImGui::Checkbox("mana", &mana) ||
        ImGui::Checkbox("ice", &ice) || ImGui::Checkbox("corrupt", &corrupt)) {
      genLocation(seed);
      engine->invalidate();
    }
  }
  ImGui::End();
}

void Application::genLocation(int s) {
  if (cacheThread.joinable()) {
    cacheThread.join();
  }

  auto emitter = entt::service_locator<event_emitter>::get().lock();
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
  } else if (locationType == LocationType::DUNGEON) {
    spec.type = LocationType::DUNGEON;
    spec.floor = CellType::FLOOR;
    river = false;
    lake = false;
  }

  std::vector<bool> flags = {
      cave_pass, statue, altar,   treasure, heal, mana,
      river,     lake,   torches, corrupt,  ice,
  };

  std::vector<LocationFeature> features = {
      LocationFeature::CAVE_PASSAGE, LocationFeature::STATUE,
      LocationFeature::ALTAR,        LocationFeature::TREASURE_SMALL,
      LocationFeature::HEAL,         LocationFeature::MANA,
      LocationFeature::RIVER,        LocationFeature::LAKE,
      LocationFeature::TORCHES,      LocationFeature::CORRUPT,
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
  viewport->regions.clear();

  auto location = std::make_shared<Region>();
  location->cells = gm->location->cells;
  location->position = {0, 0};
  location->active = true;
  location->location = gm->location;
  viewport->regions.push_back(location);

  engine->update();
  emitter->publish<resize_event>();

  auto rw = std::make_shared<RegistryWrapper>(gm->registry);
  dukglue_register_global(duk_ctx, location, "location");
  dukglue_register_global(duk_ctx, rw, "registry");
  dukglue_register_method(duk_ctx, &RegistryWrapper::size, "size");
  dukglue_register_method(duk_ctx, &RegistryWrapper::create, "create");
  dukglue_register_method(duk_ctx, &EntityWrapper::move, "move");
  dukglue_register_method(duk_ctx, &EntityWrapper::center, "center");
  dukglue_register_method(duk_ctx, &EntityWrapper::select, "select");
  dukglue_register_method(duk_ctx, &EntityWrapper::unselect, "unselect");
  dukglue_register_method(duk_ctx, &EntityWrapper::remove, "remove");
  dukglue_register_method(duk_ctx, &EntityWrapper::hide, "hide");
  dukglue_register_method(duk_ctx, &EntityWrapper::show, "show");

  dukglue_register_property(duk_ctx, &EntityWrapper::getId, nullptr, "id");
  dukglue_register_property(duk_ctx, &EntityWrapper::getX, nullptr, "x");
  dukglue_register_property(duk_ctx, &EntityWrapper::getY, nullptr, "y");
  dukglue_register_property(duk_ctx, &EntityWrapper::getZ, nullptr, "z");

  cache_count = 0;
  engine->tilesCache.clear();
  cacheThread = std::thread([&]() {
    for (auto x = 0; x < gm->location->cells.front().size(); x++) {
      for (auto y = 0; y < gm->location->cells.size(); y++) {
        auto t = engine->cacheTile(x, y, viewport->view_z);
        cache_count++;
      }
    }
    log.info("cache thread ends");
  });
}

int Application::serve() {
  log.info("serve");
  sf::Clock deltaClock;

  genLocation(seed);

  auto c = Color::fromHexString(viewport->colors["PALETTE"]["BACKGROUND"]);
  auto bgColor = sf::Color(c.r, c.g, c.b, c.a);

  engine->resize(window->getSize());

  window->clear(bgColor);
  rectangle.setPosition(0, 0);
  rectangle.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));

  cursor.setSize(sf::Vector2f(viewport->tileSet.size.first,
                              viewport->tileSet.size.second) *
                 viewport->scale * GUI_SCALE);
  cursor.setFillColor(sf::Color::Transparent);
  cursor.setOutlineThickness(1);

  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      processEvent(event);
    }
    sf::FloatRect visibleArea(0, 0, window->getSize().x, window->getSize().y);
    auto sv = sf::View(visibleArea);
    window->setView(sv);

    float current_fps = ImGui::GetIO().Framerate;

    auto t = engine->draw();
    rectangle.setTexture(&t);

    window->draw(rectangle);
    // window->clear(sf::Color::Cyan);

    auto x = int(current_pos.x /
                 (viewport->tileSet.size.first * viewport->scale * GUI_SCALE));
    auto y = int(current_pos.y /
                 (viewport->tileSet.size.second * viewport->scale * GUI_SCALE));
    auto rx = x + viewport->view_x;
    auto ry = y + viewport->view_y;
    if (lockedPos) {
      rx = (*lockedPos).first;
      ry = (*lockedPos).second;
      x = rx - viewport->view_x;
      y = ry - viewport->view_y;
    }
    auto [cc, rz] = viewport->getCell(rx, ry, viewport->view_z);

    if (!cc || (*cc)->type == CellType::UNKNOWN) {
      cursor.setOutlineColor(sf::Color(70, 70, 70));
    } else {
      cursor.setOutlineColor(sf::Color(200, 124, 70));
    }
    if (lockedPos) {
      cursor.setOutlineColor(sf::Color(70, 70, 255));
    }
    cursor.setPosition(sf::Vector2f(x * viewport->tileSet.size.first,
                                    y * viewport->tileSet.size.second) *
                       viewport->scale * GUI_SCALE);
    window->draw(cursor);

    ImGui::SFML::Update(*window, deltaClock.restart());
    ImGuiViewport *vp = ImGui::GetMainViewport();
    drawStatusBar(vp->Size.x, 16.0f * GUI_SCALE, 0.0f, vp->Size.y - 24*GUI_SCALE);

    if (debug) {
      drawDocking(24.0f * GUI_SCALE);

      editor->drawCellInfo(cc);

      drawLocationWindow();
      drawViewportWindow();
      editor->drawTilesetWindow();
      editor->drawSpecWindow();
      editor->drawObjectsWindow();
      editor->drawSelectedInfo();
      console.Draw("Console");

      dukEditorWindow();
    }
    ImGui::SFML::Render(*window);

    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}

void Application::dukEditorWindow() {
  if (!ImGui::Begin("Script")) {
    ImGui::End();
    return;
  }

  if (ImGui::Button(ICON_MDI_CONTENT_SAVE)) {
    auto fileToSave = PATH / "init.js";
    std::ofstream o(fileToSave);

    o << std::setw(4) << duk_editor.GetText().data() << std::endl;
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_PLAY)) {
    markers.clear();
    duk_editor.SetErrorMarkers(markers);

    if (jsThread.joinable()) {
      jsThread.join();
    }
    jsThread = std::thread([&]() { duk_exec(duk_editor.GetText().data()); });
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_STOP)) {
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_UNDO)) {
    duk_editor.Undo();
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_REDO)) {
    duk_editor.Redo();
  }

  duk_editor.Render("Init script");
  ImGui::End();
}

Application::~Application() {
  entt::service_locator<GameData>::reset();
  entt::service_locator<Viewport>::reset();
  if (jsThread.joinable()) {
    jsThread.join();
  }
  if (cacheThread.joinable()) {
    cacheThread.join();
  }
}
