#include "lss/game/location.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <utility>

#define SEQUENTITY_IMPLEMENTATION
#include <app/application.hpp>

#include <imgui.h>

#include <SFML/Graphics.hpp>
// #include <app/fonts/material_design_icons.h>
// #include <app/style/theme.h>
#include <libcolor/libcolor.hpp>

// #include <app/style/theme.h>
#include <imgui-stl.hpp>

#include <app/editor.hpp>
#include <app/ui/drawEngine.hpp>
#include <app/ui/viewport.hpp>

#include <IconsFontAwesome6.h>
#include <lua/logger.hpp>
#include <random.hpp>
// #include <sol/sol_ImGui.hpp>
using Random = effolkronium::random_static;

#ifdef __APPLE__
float GUI_SCALE = 1.0f;
#else
float GUI_SCALE = 1.f;
#endif

static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

// TODO: add config file
std::string DEFAULT_TILESET = "ascii";
std::string DATA_FILE = "game.bin";

void Application::initConfig() {
  fmt::print("Path: {}\n", PATH.string());
  auto label = "exec config";
  luaLog.setParent(&log);
  injectLogger(lua, luaLog);

  log.start(label);

  lua.set("APP", APP_NAME);
  lua.set("VERSION", VERSION);
  lua.set("PATH", PATH.string());
  lua.set_function("exit", [&]() { window->close(); });

  // auto l = Logger(fmt::color::lime, "INPUT");
  // l.setParent(&log);
  // initInput(lua, &l);

  auto cp = fs::path(PATH / "scripts" / "config.lua");
  if (!fs::exists(cp)) {
    log.error("config.lua doesn't exist!");
    exit(111);
  }
  lua.script_file(cp);

  winPrefix = lua.get<std::string>("win_prefix");
  altPrefix = lua.get<std::string>("alt_prefix");
  ctrlPrefix = lua.get<std::string>("ctrl_prefix");
  shiftPrefix = lua.get<std::string>("shift_prefix");
  keyDelim = lua.get<std::string>("key_delim");

  auto df = lua.get<std::string>("data_file");
  if (df != "") {
    DATA_FILE = df;
  }
  log.info("Main data file: {}", DATA_FILE);
  auto ts = lua.get<std::string>("tileset");
  if (ts != "") {
    DEFAULT_TILESET = ts;
  }
  log.info("Tileset: {}", DEFAULT_TILESET);
  auto s = lua.get<int>("seed");
  if (s > 0) {
    seed = s;
  }
  log.info("Seed: {}", seed);

  // auto _lt = lua.get<std::string>("location_type");
  // if (_lt != "") {
  //   for (auto n = 0; n < lts.size(); n++) {
  //     if (lts[n] == _lt) {
  //       lts_idx = n;
  //     }
  //   }
  //   locationType = Location::A_LOCATION_TYPES[lts_idx];
  // }

  log.stop(label);
}

Application::Application(std::string app_name, fs::path path,
                         std::string version, int s)
    : APP_NAME(app_name), VERSION(version), PATH(path) {

  lua.open_libraries(sol::lib::base);

  initConfig();

  entt::monostate<"gui_scale"_hs>{} = GUI_SCALE;
  entt::monostate<"path"_hs>{} = PATH.string().data();
  entt::monostate<"tileset"_hs>{} = DEFAULT_TILESET.data();

  // entt::locator<entt::registry>::empty();
  // entt::locator<event_emitter>::emplace();
  // entt::locator<Viewport>::empty();
  // entt::locator<GameData>::empty();
  // entt::locator<GameData>::empty();
  entt::locator<std::mutex *>::emplace(reg_mutex.get());

  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();
  // viewport = std::make_shared<Viewport>();
  auto &viewport = entt::locator<Viewport>::emplace();
  viewport.loadTileset(PATH / "tilesets" / DEFAULT_TILESET);
  auto &engine = entt::locator<DrawEngine>::emplace(window);
  engine.vW = viewport.width / viewport.scale;
  engine.vH = viewport.height / viewport.scale;
  engine.alpha_per_d = lua.get<sol::table>("light")["alpha_per_d"];
  engine.alpha_blend_inc = lua.get<sol::table>("light")["alpha_blend_inc"];
  engine.blend_mode = lua.get<sol::table>("light")["blend_mode"];
  engine.max_bright = lua.get<sol::table>("light")["max_bright"];

  gm = std::make_shared<GameManager>(PATH / DATA_FILE);
  editor = std::make_unique<Editor>(gm, PATH);
  ide = std::make_unique<IDE>(PATH);
  if (seed > 0) {
    s = seed;
  }
  gm->setSeed(s);
  seed = gm->seed;

  initLuaBindings();

  setupGui();
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

void Application::setupGui() {

  sf::ContextSettings settings;
  // settings.antialiasingLevel = 8;
  settings.antialiasingLevel = 4;
  ImGui::CreateContext();
  // Theme::Init();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.FontGlobalScale = GUI_SCALE;
  ImGui::GetStyle().ScaleAllSizes(GUI_SCALE);

  window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), APP_NAME,
                                sf::Style::Default, settings);

  window->setVerticalSyncEnabled(true);

  log.debug("Sfml init: {}", ImGui::SFML::Init(*window, false));

  window->resetGLStates();

  event_emitter emitter{};

  emitter.on<regen_event>([&](const auto &p, auto &em) {
    if (p.seed == -1) {
      seed = rand();
    } else if (p.seed >= 0) {
      seed = p.seed;
    }
    auto &viewport = entt::locator<Viewport>::value();
    auto &engine = entt::locator<DrawEngine>::value();
    viewport.view_x = 0;
    viewport.view_y = 0;
    viewport.view_z = 0;
    if (p.specKey != "") {
      genLocation(seed, *gm->locationSpecs[p.specKey]);
    } else {
      genLocation(seed);
    }
    engine.invalidate();
  });
  emitter.on<center_event>([&](const auto &p, auto &em) {
    // lockedPos = {p.x, p.y};
    auto &viewport = entt::locator<Viewport>::value();
    auto &engine = entt::locator<DrawEngine>::value();
    viewport.view_x = p.x - viewport.width / 2 / GUI_SCALE;
    viewport.view_y = p.y - viewport.height / 2 / GUI_SCALE;
    engine.invalidate();
  });
  emitter.on<mouse_center_event>([&](const auto &p, auto &em) {
    // lockedPos = {p.x, p.y};
    auto &viewport = entt::locator<Viewport>::value();
    auto &engine = entt::locator<DrawEngine>::value();
    current_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    auto _x = int(current_pos.x /
                  (viewport.tileSet.size.first * viewport.scale * GUI_SCALE));
    auto _y = int(current_pos.y /
                  (viewport.tileSet.size.second * viewport.scale * GUI_SCALE));
    auto rx = _x + viewport.view_x;
    auto ry = _y + viewport.view_y;
    // fmt::print("{}.{} => {}.{}\n", p.x, p.y, rx, ry);
    viewport.view_x += p.x - rx;
    viewport.view_y += p.y - ry;
    engine.invalidate();
  });

  emitter.on<redraw_event>([&](const auto &p, auto &em) {
    auto &engine = entt::locator<DrawEngine>::value();
    engine.invalidate();
  });

  emitter.on<resize_event>([&](const auto &p, auto &em) {
    sf::FloatRect visibleArea(0, 0, window->getSize().x, window->getSize().y);
    auto sv = sf::View(visibleArea);
    window->setView(sv);
    auto &viewport = entt::locator<Viewport>::value();
    auto &engine = entt::locator<DrawEngine>::value();
    viewport.width = window->getSize().x / viewport.tileSet.size.first + 1;
    viewport.height = window->getSize().y / viewport.tileSet.size.second + 1;
    engine.vW = viewport.width / viewport.scale / GUI_SCALE;
    engine.vH = viewport.height / viewport.scale / GUI_SCALE;
    // engine.tilesCache.clear();
    engine.resize(window->getSize());
    rectangle.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
    cursor.setSize(sf::Vector2f(viewport.tileSet.size.first,
                                viewport.tileSet.size.second) *
                   viewport.scale * GUI_SCALE);
    engine.invalidate();
  });

  // emitter.on<duk_error>([&](auto event, auto &em) {
  //   std::string err = std::string(event.msg);
  //   std::string line;
  //   std::vector<std::string> lines;
  //   std::istringstream ss(err);
  //   std::regex re("eval:(\\d+)");
  //   std::smatch m;
  //   while (std::getline(ss, line, '\n')) {
  //     lines.push_back(line);
  //   }
  //   std::regex_search(lines.back(), m, re);

  //   markers.insert(std::make_pair<int, std::string>(
  //       std::atoi(std::string(m[m.size() - 1]).data()),
  //       std::string(lines.front())));
  //   duk_editor.SetErrorMarkers(markers);

  //   console.AddLog("[error] %s", event.msg.data());
  // });

  /* emitter.on<exec_event>(
      [&](auto event, auto &em) { duk_exec(event.code.data()); }); */

  ide->init();

  // sol_ImGui::Init(lua);

  // std::function<void()> scale_f = [&]() {
  //   auto _x = int(current_pos.x /
  //                 (viewport.tileSet.size.first * viewport.scale *
  //                 GUI_SCALE));
  //   auto _y = int(current_pos.y / (viewport.tileSet.size.second *
  //                                  viewport.scale * GUI_SCALE));
  //   auto rx = _x + viewport.view_x;
  //   auto ry = _y + viewport.view_y;
  //   event_emitter emitter{};
  //   emitter.publish(center_event{rx, ry});
  //   updateScale();
  // };
  // db_scale_f = Debounced(scale_f, 500);
}

void Application::processKeyboardEvent(sf::Event event) {

  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureKeyboard)
    return;

  auto event_str = getKeyName(event.key.code, event.key.system, event.key.alt,
                              event.key.control, event.key.shift);
  // fmt::print("Key pressed: {}\n", event_str);

  if (mapping.find(event_str) != mapping.end()) {
    mapping[event_str]();
  }

  auto &engine = entt::locator<DrawEngine>::value();
  auto &viewport = entt::locator<Viewport>::value();
  current_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
  auto _x = int(current_pos.x /
                (viewport.tileSet.size.first * viewport.scale * GUI_SCALE));
  auto _y = int(current_pos.y /
                (viewport.tileSet.size.second * viewport.scale * GUI_SCALE));
  auto rx = _x + viewport.view_x;
  auto ry = _y + viewport.view_y;
  switch (event.key.code) {
  case sf::Keyboard::Escape: {
    window->close();
    break;
  }
  case sf::Keyboard::Right: {
    viewport.view_x += 1;
    engine.invalidate();
    auto i = 0;
    for (auto p : gm->location->creatures) {
      auto e = p.first;
      gm->commit(lss::Action(p.first, "creatureMove", 200, [=]() {
        gm->moveCreature(gm->location->creatures[e], Direction::E);
      }));
      i++;
      if (i > 5) {
        break;
      }
    }
    break;
  }
  case sf::Keyboard::Left: {
    viewport.view_x -= 1;
    engine.invalidate();
    break;
  }
  case sf::Keyboard::Up: {
    if (!event.key.control) {
      viewport.view_y -= 1;
    } else {
      viewport.view_z += 1;
    }
    engine.invalidate();
    break;
  }
  case sf::Keyboard::Down: {
    if (!event.key.control) {
      viewport.view_y += 1;
    } else {
      viewport.view_z -= 1;
    }
    engine.invalidate();
    break;
  }
  case sf::Keyboard::F1: {
    debug = !debug;
    break;
  }
  case sf::Keyboard::F2: {
    event_emitter emitter{};
    emitter.publish(regen_event{-1});
    break;
  }
  case sf::Keyboard::M: {
    auto ents = gm->registry.view<hf::ineditor>();

    if (ents.size() > 0) {
      for (auto e : ents) {
        auto ie = gm->registry.get<hf::ineditor>(e);
        if (ie.selected) {
          auto p = gm->registry.get<hf::position>(e);
          p.x = rx;
          p.y = ry;
          gm->registry.emplace_or_replace<hellfrost::position>(e, p);
        }
      }
    }

    break;
  }
  }
}

void Application::processEvent(sf::Event event) {
  auto &viewport = entt::locator<Viewport>::value();
  auto &engine = entt::locator<DrawEngine>::value();
  ImGuiIO &io = ImGui::GetIO();
  current_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
  auto _x = int(current_pos.x /
                (viewport.tileSet.size.first * viewport.scale * GUI_SCALE));
  auto _y = int(current_pos.y /
                (viewport.tileSet.size.second * viewport.scale * GUI_SCALE));
  auto rx = _x + viewport.view_x;
  auto ry = _y + viewport.view_y;

  ImGui::SFML::ProcessEvent(*window, event);
  switch (event.type) {
  case sf::Event::KeyReleased:
    switch (event.key.code) {}
    break;
  case sf::Event::KeyPressed:
    processKeyboardEvent(event);
    break;
  case sf::Event::Resized: {
    event_emitter emitter{};
    emitter.publish(resize_event{});
  }
  // case sf::Event::LostFocus:
  case sf::Event::GainedFocus:
    engine.invalidate();
    break;
  case sf::Event::Closed:
    window->close();
    break;
  case sf::Event::MouseMoved:
    if (io.WantCaptureMouse)
      break;
    if (is_mb_pressed) {
      event_emitter emitter{};
      auto _sx = int(start_drag_pos.x / (viewport.tileSet.size.first *
                                         viewport.scale * GUI_SCALE));
      auto _sy = int(start_drag_pos.y / (viewport.tileSet.size.second *
                                         viewport.scale * GUI_SCALE));

      emitter.publish(mouse_center_event{_sx, _sy});
    }
    if (is_rb_pressed) {
      auto ents = gm->registry.view<hf::ineditor>();

      if (ents.size() > 0) {
        for (auto e : ents) {
          auto ie = gm->registry.get<hf::ineditor>(e);
          if (ie.selected) {
            auto p = gm->registry.get<hf::position>(e);
            p.x = rx;
            p.y = ry;
            gm->registry.emplace_or_replace<hellfrost::position>(e, p);
          }
        }
      }
    }
    break;
  case sf::Event::MouseButtonReleased:
    if (io.WantCaptureMouse)
      break;
    start_drag_pos = {0, 0};
    if (event.mouseButton.button == sf::Mouse::Right) {
      is_rb_pressed = false;
    } else if (event.mouseButton.button == sf::Mouse::Middle) {
      is_mb_pressed = false;
    }
    break;
  case sf::Event::MouseButtonPressed:
    if (io.WantCaptureMouse)
      break;
    start_drag_pos = current_pos;
    if (event.mouseButton.button == sf::Mouse::Right) {
      is_rb_pressed = true;
      lockedPos = {rx, ry};

      auto ents = gm->registry.view<hf::ineditor>();

      if (ents.size() > 0) {
        for (auto e : ents) {
          auto p = gm->registry.get<hf::ineditor>(e);
          if (p.selected) {
            p.selected = false;
            gm->registry.emplace_or_replace<hellfrost::ineditor>(e, p);
          }
        }
      }

      auto _ents = gm->registry.view<hf::position>();

      if (_ents.size() > 0) {
        for (auto e : _ents) {
          auto p = gm->registry.get<hf::position>(e);
          if (!p.movable)
            continue;
          auto ie = gm->registry.get<hf::ineditor>(e);
          if (p.x == rx && p.y == ry) {
            ie.selected = true;
            gm->registry.emplace_or_replace<hellfrost::ineditor>(e, ie);
          }
        }
      }

    } else if (event.mouseButton.button == sf::Mouse::Middle) {
      is_mb_pressed = true;
      lockedPos = std::nullopt;

      auto ents = gm->registry.view<hf::ineditor>();

      if (ents.size() > 0) {
        for (auto e : ents) {
          auto p = gm->registry.get<hf::ineditor>(e);
          if (p.selected) {
            p.selected = false;
            gm->registry.emplace_or_replace<hellfrost::ineditor>(e, p);
          }
        }
      }
    }
    break;
  case sf::Event::MouseWheelScrolled:
    if (io.WantCaptureMouse)
      break;
    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
      viewport.scale += event.mouseWheelScroll.delta * 0.05f; // step
      updateScale();
      event_emitter emitter{};
      emitter.publish(mouse_center_event{rx, ry});
      // TODO: move previous cell under mouse
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
  // io.ConfigResizeWindowsFromEdges = true;
  // if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
  ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
  /* } else {
    log.error("y no docking?");
    // TODO: emit a log message
  } */
  ImGui::End();
}

void Application::drawStatusBar(float width, float height, float pos_x,
                                float pos_y) {
  // Draw status bar (no docking)
  auto &viewport = entt::locator<Viewport>::value();
  ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
  ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Always);
  ImGui::Begin("statusbar", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings |
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_NoResize);

  // Call the derived class to add stuff to the status bar
  // DrawInsideStatusBar(width - 45.0f, height);

  // Draw the common stuff
  ImGui::SameLine(8.0f * GUI_SCALE);
  // Font font(Font::FAMILY_MONOSPACE);
  // font.Normal().Regular().SmallSize();
  // ImGui::PushFont(font.ImGuiFont());
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
                (viewport.tileSet.size.first * viewport.scale * GUI_SCALE));
  auto _y = int(current_pos.y /
                (viewport.tileSet.size.second * viewport.scale * GUI_SCALE));
  ImGui::Text("|  pos: %d.%d", _x, _y);
  auto rx = _x + viewport.view_x;
  auto ry = _y + viewport.view_y;
  ImGui::SameLine();
  ImGui::Text("|  cell: %d.%d", rx, ry);
  ImGui::SameLine();
  if (gm->location && !gm->location->cells.empty()) {
    auto cache_full =
        gm->location->cells.front().size() * gm->location->cells.size();
    if (cache_count / float(cache_full) != 1.f) {
      ImGui::Text("|  cache:");
      ImGui::SameLine();
      const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
      const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

      ImGui::AlignTextToFramePadding();
      ImGui::BufferingBar("##buffer_bar", cache_count / float(cache_full),
                          ImVec2(100 * GUI_SCALE, 3 * GUI_SCALE), bg, col);
    } else {
      ImGui::Text("|  cache: %d/%lu", cache_count, cache_full);
    }
  }
  // ImGui::PopFont();
  ImGui::End();
}

void Application::drawViewportWindow() {
  auto &viewport = entt::locator<Viewport>::value();
  auto &engine = entt::locator<DrawEngine>::value();
  if (!ImGui::Begin("Viewport")) {
    ImGui::End();
    return;
  }

  if (!gm->location || gm->location->cells.empty()) {
    ImGui::Text("No location generated");
    ImGui::End();
    return;
  }
  auto cache_full =
      gm->location->cells.front().size() * gm->location->cells.size();
  ImGui::Text("Cache len: %d/%lu", cache_count, cache_full);
  ImGui::Text("Vis Cache cell count: %zu",
              gm->location->visibilityCache.size());
  ImGui::Text("Redraws: %d", engine.redraws);
  ImGui::Text("Tiles updated: %d", engine.tilesUpdated);
  ImGui::Text("Objects in render: %d\n", engine.layers->size());

  if (ImGui::BeginTable("layers", 3,
                        ImGuiTableFlags_Resizable |
                            ImGuiTableFlags_SizingFixedFit |
                            ImGuiTableFlags_NoSavedSettings)) {
    for (auto [k, l] : engine.layers->layers) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(fmt::format("{}", k).c_str());
      ImGui::TableNextColumn();
      ImGui::Text(fmt::format("{}", l->children.size()).c_str());
      ImGui::TableNextColumn();
      if (ImGui::Checkbox(fmt::format("##{}", k).c_str(), &l->enabled)) {
        engine.invalidate();
      }
    }
    ImGui::EndTable();
  }
  ImGui::Text("\n");
  event_emitter emitter{};
  if (ImGui::SliderInt("width", &viewport.width, 10, 200)) {
    engine.invalidate();
    engine.vW = viewport.width / viewport.scale / GUI_SCALE;
    if (engine.vW > 200)
      engine.vW = 200;
  }
  if (ImGui::SliderInt("height", &viewport.height, 10, 200)) {
    engine.invalidate();
    engine.vH = viewport.height / viewport.scale / GUI_SCALE;
    if (engine.vH > 200)
      engine.vH = 200;
  }
  if (ImGui::SliderInt("x", &viewport.view_x, -viewport.width,
                       gm->location->cells.front().size())) {

    engine.invalidate();
  }
  if (ImGui::SliderInt("y", &viewport.view_y, -viewport.height,

                       gm->location->cells.size())) {
    engine.invalidate();
  }
  if (ImGui::SliderInt("z", &viewport.view_z, -10, 10)) {
    engine.invalidate();
  }

  if (ImGui::SliderFloat("scale", &viewport.scale, 0.3f, 2.f)) {
    updateScale();
  }
  // ImGui::SetItemUsingMouseWheel();
  // if (io.WantCaptureKeyboard) {
  // float wheel = ImGui::GetIO().MouseWheel;
  // if (wheel) {
  //   if (ImGui::IsItemActive()) {
  //     ImGui::ClearActiveID();
  //   } else {
  //     viewport.scale += wheel * 0.025f; // step
  //     updateScale();
  //   }
  // }
  // }

  ImGui::SameLine();
  // if (ImGui::Button(ICON_FA_FORMAT_SIZE)) {
  if (ImGui::Button("size")) {
    viewport.scale = 1;
    emitter.publish(resize_event{});
  }
  ImGui::Checkbox("Room debug", &engine.roomDebug);

  ImGui::End();
}

void Application::updateScale() {
  auto &viewport = entt::locator<Viewport>::value();
  auto &engine = entt::locator<DrawEngine>::value();
  event_emitter emitter{};
  engine.vW = viewport.width / viewport.scale / GUI_SCALE;
  engine.vH = viewport.height / viewport.scale / GUI_SCALE;
  if (engine.vW > 200)
    engine.vW = 200;
  if (engine.vH > 200)
    engine.vH = 200;
  emitter.publish(resize_event{});
}

void Application::drawLocationWindow() {
  if (!ImGui::Begin("Location")) {
    ImGui::End();
    return;
  }
  auto &viewport = entt::locator<Viewport>::value();
  auto &engine = entt::locator<DrawEngine>::value();
  auto &data = entt::locator<GameData>::value();

  if (ImGui::InputInt("Seed", &seed)) {
    genLocation(seed);
    engine.invalidate();
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_DICE_THREE)) {
    event_emitter emitter{};
    emitter.publish(regen_event{-1});
  }
  if (gm->location == nullptr) {
    ImGui::End();
    return;
  }
  ImGui::SameLine();
  if (ImGui::Button("Redraw")) {
    event_emitter emitter{};
    emitter.publish(redraw_event{});
  }
  ImGui::Separator();
  std::vector<std::string> lts;
  auto n = 0;
  auto lts_idx = 0;
  for (auto [key, _] : gm->locationSpecs) {
    lts.push_back(key);
    if (gm->location->type.name == key) {
      lts_idx = n;
    }
    n++;
  }
  if (ImGui::Combo("Location type", &lts_idx, lts)) {
    //   locationType = Location::A_LOCATION_TYPES[lts_idx];
    event_emitter emitter{};
    emitter.publish(regen_event{seed, lts[lts_idx]});
    // genLocation(seed, lts[lt_idx]);
  }

  ImGui::Text("Feature overrides:");
  if (ImGui::Button("Disable all")) {
    for (auto [k, v] : gm->location->type.templateMap) {
      if (k == "")
        continue;
      for (auto [key, value] : v) {
        gm->location->type.templateMap[k][key] = 0;
      }
    }
    genLocation(seed, gm->location->type);
    engine.invalidate();
    ImGui::End();
    return;
  }
  ImGui::SameLine();
  if (ImGui::Button("Enable all")) {
    for (auto [k, v] : gm->location->type.templateMap) {
      if (k == "")
        continue;
      for (auto [key, value] : v) {
        gm->location->type.templateMap[k][key] = 1;
      }
    }
    genLocation(seed, gm->location->type);
    engine.invalidate();
    ImGui::End();
    return;
  }
  for (auto [k, v] : gm->location->type.templateMap) {
    if (k == "")
      continue;
    if (ImGui::CollapsingHeader(k.c_str())) {
      for (auto [key, value] : v) {
        if (ImGui::Button(fmt::format("1##{}", key).c_str())) {
          gm->location->type.templateMap[k][key] = 1;
          genLocation(seed, gm->location->type);
          engine.invalidate();
          ImGui::End();
          return;
        }
        ImGui::SameLine();
        if (ImGui::Button(fmt::format("D##{}", key).c_str())) {
          gm->location->type.templateMap[k][key] = data.mapFeatures[k][key];
          genLocation(seed, gm->location->type);
          engine.invalidate();
          ImGui::End();
          return;
        }
        ImGui::SameLine();
        if (ImGui::Button(fmt::format("0##{}", key).c_str())) {
          gm->location->type.templateMap[k][key] = 0;
          genLocation(seed, gm->location->type);
          engine.invalidate();
          ImGui::End();
          return;
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(80);
        ImGui::InputFloat(fmt::format("##{}", key).c_str(),
                          &gm->location->type.templateMap[k][key]);
        ImGui::SameLine();
        if (gm->templates.find(key) != gm->templates.end()) {
          ImGui::Text(
              fmt::format("{} [{}] t", key, gm->templates[key]->stage).c_str());
        } else if (gm->features.find(key) != gm->features.end()) {
          ImGui::Text(
              fmt::format("{} [{}] f", key, gm->features[key]->stage).c_str());
        } else {
          ImGui::Text(fmt::format("{} [broken]", key).c_str());
        }
      }
    }
  }

  ImGui::End();
}

void Application::genLocation() {
  auto s = rand();
  genLocation(s);
}

void Application::genLocation(int s) {

  //   auto spec = LocationSpec{"Dungeon"};
  //   spec.type = locationType;
  //   spec.threat = 1;
  //   if (locationType == LocationType::CAVERN) {
  //     spec.floor = CellType::GROUND;
  //     spec.cellTags.add("CAVE");
  //   } else if (locationType == LocationType::EXTERIOR) {
  //     spec.floor = CellType::GROUND;
  //   } else if (locationType == LocationType::DUNGEON) {
  //     spec.floor = CellType::FLOOR;
  //   }

  auto lt = lua.get<std::string>("location_type");
  log.info(lt);
  genLocation(s, *gm->locationSpecs.at(lt));
}

bool Application::movePlayer(Direction d) {
  auto s = gm->moveCreature(gm->location->player, d);
  if (!s) {
    return false;
  }
  auto &viewport = entt::locator<Viewport>::value();
  auto &engine = entt::locator<DrawEngine>::value();

  auto p = gm->registry.get<hf::position>(gm->location->player->entity);
  auto margin = lua.get<int>("margin");
  if (p.x < viewport.view_x + margin) {
    viewport.view_x--;
    engine.invalidate();
  } else if (p.x > viewport.view_x + viewport.width - margin) {
    viewport.view_x++;
    engine.invalidate();
  }
  if (p.y < viewport.view_y + margin) {
    viewport.view_y--;
    engine.invalidate();
  } else if (p.y > viewport.view_y + viewport.height - margin) {
    viewport.view_y++;
    engine.invalidate();
  }
  // gm->serve();
  // gm->location->invalidateVisibilityCache(gm->location->player->currentCell,
  //                                         true);
  return true;
}

void Application::genLocation(int s, LocationSpec spec) {

  auto mutex = entt::locator<std::mutex *>::value();
  auto &viewport = entt::locator<Viewport>::value();
  auto &engine = entt::locator<DrawEngine>::value();
  mutex->lock();
  if (cacheThread.joinable()) {
    cacheThread.join();
  }

  event_emitter emitter{};
  gm->setSeed(s);

  auto label = "Generate location";
  log.start(label);

  gm->gen(spec);
  log.stop(label);
  viewport.regions.clear();

  auto location = std::make_shared<Region>();
  location->cells = gm->location->cells;
  location->position = {0, 0};
  location->active = true;
  location->location = gm->location;
  viewport.regions.push_back(location);

  engine.update();
  emitter.publish(resize_event{});

  if (location->location->enterCell != nullptr) {
    emitter.publish(center_event{location->location->enterCell->x, location->location->enterCell->y});
  }

  cache_count = 0;
  engine.tilesCache.clear();
  // cacheThread = std::thread([&]() {
  //   if (!gm->location->cells.empty()) {
  //     for (auto x = 0; x < gm->location->cells.front().size(); x++) {
  //       for (auto y = 0; y < gm->location->cells.size(); y++) {
  //         auto t = engine.cacheTile(x, y, viewport.view_z);
  //         cache_count++;
  //       }
  //     }
  //   }
  //   log.info("cache thread ends");
  // });

  mutex->unlock();
}

int Application::serve() {
  log.start("init");
  lua.script_file(PATH / "scripts" / "init.lua");
  log.stop("init");

  log.info("serve");
  auto &viewport = entt::locator<Viewport>::value();
  auto &engine = entt::locator<DrawEngine>::value();

  sf::ContextSettings settings;
  settings.antialiasingLevel = 4;
  sf::Clock deltaClock;

  // genLocation(seed);

  auto c = Color::fromHexString(viewport.colors["PALETTE"]["BACKGROUND"]);
  auto bgColor = sf::Color(c.r, c.g, c.b, c.a);

  engine.resize(window->getSize());

  window->clear(bgColor);
  rectangle.setPosition(0, 0);
  rectangle.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));

  cursor.setSize(
      sf::Vector2f(viewport.tileSet.size.first, viewport.tileSet.size.second) *
      viewport.scale * GUI_SCALE);
  cursor.setFillColor(sf::Color::Transparent);
  cursor.setOutlineThickness(1);

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigDockingWithShift = false;
  io.MouseDrawCursor = true;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  icons_config.GlyphMinAdvanceX = 16.0f;
  // io.Fonts->Clear();
  io.Fonts->AddFontDefault();
  io.Fonts->AddFontFromFileTTF("Hack-Bold.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config,
  //                              icons_ranges);
  log.info("fonts: {}", io.Fonts->Fonts.Size);
  io.Fonts->Build();
  log.debug("Update font tex: {}", ImGui::SFML::UpdateFontTexture());
  log.info("reg size: {}", gm->registry.storage<entt::entity>().size());

  std::thread t([=]() {
    auto d = lua.get<sol::table>("light")["flick_delay"];
    auto mutex = entt::locator<std::mutex *>::value();
    auto &viewport = entt::locator<Viewport>::value();
    auto &engine = entt::locator<DrawEngine>::value();
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(d));
      // mutex->lock();
      // engine.updateLight();
      // mutex->unlock();
      engine.updateExistingLight();
      // engine.scheduleFastRedraw();
      // engine.invalidate();
    }
  });
  t.detach();

  auto de = true;
  // auto de = lua.get<bool>("darkness");
  engine.layers->layers["darkness"]->enabled = de;

  // gm->serve();
  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      processEvent(event);
    }

    sf::FloatRect visibleArea(0, 0, window->getSize().x, window->getSize().y);
    auto sv = sf::View(visibleArea);
    window->setView(sv);

    float current_fps = ImGui::GetIO().Framerate;

    auto t = engine.draw();
    rectangle.setTexture(&t);

    window->draw(rectangle);
    // window->clear(sf::Color::Cyan);

    auto x = int(current_pos.x /
                 (viewport.tileSet.size.first * viewport.scale * GUI_SCALE));
    auto y = int(current_pos.y /
                 (viewport.tileSet.size.second * viewport.scale * GUI_SCALE));
    auto rx = x + viewport.view_x;
    auto ry = y + viewport.view_y;
    if (lockedPos) {
      rx = (*lockedPos).first;
      ry = (*lockedPos).second;
      x = rx - viewport.view_x;
      y = ry - viewport.view_y;
    }
    auto [cc, rz] = viewport.getCell(rx, ry, viewport.view_z);

    if (!cc || (*cc)->type == CellType::UNKNOWN) {
      cursor.setOutlineColor(sf::Color(70, 70, 70));
    } else {
      cursor.setOutlineColor(sf::Color(200, 124, 70));
    }
    if (lockedPos) {
      cursor.setOutlineColor(sf::Color(70, 70, 255));
    }
    cursor.setPosition(sf::Vector2f(x * viewport.tileSet.size.first,
                                    y * viewport.tileSet.size.second) *
                       viewport.scale * GUI_SCALE);
    window->draw(cursor);

    ImGui::SFML::Update(*window, deltaClock.restart());
    ImGuiViewport *vp = ImGui::GetMainViewport();

    // lua.script_file(PATH / "scripts" / "draw.lua");
    // drawStatusBar(vp->Size.x, 16.0f * GUI_SCALE, 0.0f,
    //               vp->Size.y - 24 * GUI_SCALE);

    if (debug) {
      ImGui::ShowDemoWindow();
      drawDocking(24.0f * GUI_SCALE);

      editor->drawCellInfo(cc);

      drawLocationWindow();
      drawViewportWindow();
      editor->drawTilesetWindow();
      editor->drawSpecWindow();
      editor->drawObjectsWindow();
      editor->drawSelectedInfo();
      if (gm->location) {
        editor->drawLocationWindow(gm->location);
      }

      ImGui::Begin("Timeline");
      Sequentity::EventEditor(gm->registry);
      ImGui::End();

      // console.Draw("Console");
      ide->render(lua);
      ide->renderTodo();
      editor->drawTemplates(lua);
    }
    ImGui::SFML::Render(*window);

    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}

Application::~Application() {
  entt::locator<GameData>::reset();
  entt::locator<Viewport>::reset();
  if (jsThread.joinable()) {
    jsThread.join();
  }
  if (cacheThread.joinable()) {
    cacheThread.join();
  }
}
