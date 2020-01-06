#include <application.hpp>

#include <chrono>
#include <iostream>
#include <regex>
#include <thread>
#include <utility>

#include <SFML/Graphics.hpp>
#include <imgui-etc/imgui-stl.hpp>
#include <imgui-sfml/imgui-SFML.h>
#include <imgui/imgui.h>
#include <libcolor/libcolor.hpp>

#include <editor/duk_console.h>
#include <editor/editor.hpp>
#include <fonts/material_design_icons.h>
#include <style/theme.h>

#include <hellfrost/deps.hpp>
#include <hellfrost/ui/drawEngine.hpp>

#ifdef __APPLE__
float GUI_SCALE = 2.0f;
#else
float GUI_SCALE = 1.f;
#endif

namespace hf = hellfrost;

// static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;
static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

// TODO: add config file
auto DEFAULT_TILESET = "ascii";
auto DATA_FILE = fs::path("data/game.bin");
Application::Application(std::string app_name, fs::path path,
                         std::string version, int s)
    : APP_NAME(app_name), VERSION(version), PATH(path) {

  log.start(APP_NAME);
  log.start("App init");
  log.info("Seed: {}", lu::blue("{}", s));
  log.info("Path: {}", lu::gray("{}", PATH.string()));

  entt::monostate<"gui_scale"_hs>{} = GUI_SCALE;
  entt::monostate<"path"_hs>{} = PATH.string().data();

  entt::service_locator<entt::registry>::empty();
  entt::service_locator<hf::event_emitter>::set(
      std::make_shared<hf::event_emitter>());
  entt::service_locator<hf::Viewport>::empty();
  // entt::service_locator<hf::GameData>::empty();

  std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();
  auto viewport = std::make_shared<hf::Viewport>();
  entt::service_locator<hf::Viewport>::set(viewport);
  viewport->loadTileset(PATH / fs::path("data/tilesets") / DEFAULT_TILESET);
  engine = std::make_unique<hf::DrawEngine>(window);
  engine->vW = viewport->width / viewport->scale;
  engine->vH = viewport->height / viewport->scale;

  entt::service_locator<hf::DrawEngine>::set(engine);
  gm = std::make_shared<hf::GameManager>(PATH / DATA_FILE, s);
  entt::service_locator<hf::GameManager>::set(gm);
  editor = std::make_shared<Editor>(PATH);

  setupGui();
  log.stop("App init");
}

void Application::setupGui() {
  log.start("GUI init");

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  Theme::Init();
  ImGui::GetStyle().ScaleAllSizes(GUI_SCALE);

  window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), APP_NAME,
                                sf::Style::Default, settings);
  window->setVerticalSyncEnabled(true);

  ImGui::SFML::Init(*window);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  window->resetGLStates();

  auto emitter = entt::service_locator<hf::event_emitter>::get().lock();

  emitter->on<hf::regen_event>([&](const auto &p, auto &em) {
    auto viewport = entt::service_locator<hf::Viewport>::get().lock();
    auto seed = gm->seed;
    if (p.seed == -1) {
      seed = rand();
    } else if (p.seed >= 0) {
      seed = p.seed;
    }
    viewport->view_x = 0;
    viewport->view_y = 0;
    viewport->view_z = 0;
    gm->start(seed);
    engine->invalidate();
  });
  emitter->on<hf::center_event>([&](const auto &p, auto &em) {
    auto viewport = entt::service_locator<hf::Viewport>::get().lock();
    // lockedPos = {p.x, p.y};
    viewport->view_x = p.x - viewport->width / 2 / GUI_SCALE;
    viewport->view_y = p.y - viewport->height / 2 / GUI_SCALE;
    engine->invalidate();
  });

  emitter->on<hf::redraw_event>(
      [&](const auto &p, auto &em) { engine->invalidate(); });

  emitter->on<hf::location_update_event>([&](const auto &p, auto &em) {
    gm->location->invalidate();
    engine->clearCache();
    engine->invalidate();
  });

  emitter->on<hf::resize_event>([&](const auto &p, auto &em) {
    auto viewport = entt::service_locator<hf::Viewport>::get().lock();
    log.start("resize event");
    sf::FloatRect visibleArea(0, 0, window->getSize().x, window->getSize().y);
    auto sv = sf::View(visibleArea);
    window->setView(sv);
    viewport->width = window->getSize().x / viewport->tileSet.size.first + 1;
    viewport->height = window->getSize().y / viewport->tileSet.size.second + 1;
    log.debug("viewport new size: {}x{}", viewport->width, viewport->height);
    engine->vW = viewport->width / viewport->scale / GUI_SCALE;
    engine->vH = viewport->height / viewport->scale / GUI_SCALE;
    // engine->tilesCache.clear();
    engine->resize(window->getSize());
    rectangle.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
    engine->invalidate();
    log.stop("resize event");
  });

  emitter->on<hf::gm::generation_finish>([&](const auto &p, auto &em) {
    auto viewport = entt::service_locator<hf::Viewport>::get().lock();
    viewport->update(gm->location);

    engine->update();
    auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
    emitter->publish<hf::resize_event>();
  });
  log.stop("GUI init");
}

void Application::processEvent(sf::Event event) {
  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  // current_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
  // auto _x = int(current_pos.x /
  //               (viewport->tileSet.size.first * viewport->scale *
  //               GUI_SCALE));
  // auto _y = int(current_pos.y /
  //               (viewport->tileSet.size.second * viewport->scale *
  //               GUI_SCALE));
  // auto rx = _x + viewport->view_x;
  // auto ry = _y + viewport->view_y;

  log.start("app process event", true);
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
      DEBUG = !DEBUG;
      break;
    }
    break;
  case sf::Event::Resized: {
    auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
    emitter->publish<hf::resize_event>();
  }
  // case sf::Event::LostFocus:
  case sf::Event::GainedFocus:
    engine->invalidate();
    break;
  case sf::Event::Closed:
    window->close();
    break;
    // TODO: move to editor
    // case sf::Event::MouseButtonPressed:
    //   if (event.mouseButton.button == sf::Mouse::Right) {
    //     lockedPos = {rx, ry};
    //   } else if (event.mouseButton.button == sf::Mouse::Middle) {
    //     lockedPos = std::nullopt;
    //   }

    //   break;
  }
  log.stop("app process event", 10);
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
  auto viewport = entt::service_locator<hf::Viewport>::get()
                      .lock(); // TODO: use gamemanager instead
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
  if (viewport->scale != 1) {
    ImGui::SameLine();
    ImGui::Text("|  scale: %.3f", viewport->scale);
  }

  /*
  ImGui::SameLine();
  auto cache_full = gm->location->width * gm->location->height;
  const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
  if (cache_full > 0) {
    if (engine->cache_count / float(cache_full) != 1.f) {
      ImGui::Text("|  cache:");
      ImGui::SameLine();
      const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

      ImGui::AlignTextToFramePadding();
      ImGui::BufferingBar("##cache_bar",
                          engine->cache_count / float(cache_full),
                          ImVec2(100 * GUI_SCALE, 3 * GUI_SCALE), bg, col);
    }
  } else {
    ImGui::Text("|  gen:");
    ImGui::SameLine();
    ImGui::Spinner("##generating", 5.f, 1.f, col);
  }
*/
  ImGui::PopFont();
  ImGui::End();
}

/*
auto rw = std::make_shared<RegistryWrapper>(gm->registry);
dukglue_register_global(duk_ctx, gm->location, "location");
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
 */

// cache_count = 0;
// engine->clearCache();
// cacheThread = std::thread([&]() {
//   log.start("bg caching");
//   for (auto x = 0; x < gm->location->width; x++) {
//     for (auto y = 0; y < gm->location->height; y++) {
//       auto t = engine->cacheTile(x, y, viewport->view_z);
//       cache_count++;
//     }
//   }
//   log.stop("bg caching");
// });

int Application::serve() {
  sf::Clock deltaClock;

  gm->start();

  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  auto c = Color::fromHexString(viewport->colors["PALETTE"]["BACKGROUND"]);
  auto bgColor = sf::Color(c.r, c.g, c.b, c.a);

  engine->resize(window->getSize());

  window->clear(bgColor);
  rectangle.setPosition(0, 0);
  rectangle.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));

  log.debug("serve loop started");
  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      processEvent(event);
    }
    // sf::FloatRect visibleArea(0, 0, window->getSize().x,
    // window->getSize().y); auto sv = sf::View(visibleArea);
    // window->setView(sv);

    float current_fps = ImGui::GetIO().Framerate;

    auto t = engine->Draw();
    rectangle.setTexture(&t);

    window->draw(rectangle);
    // window->clear(sf::Color::Cyan);

    // auto x = int(current_pos.x /
    //              (viewport->tileSet.size.first * viewport->scale *
    //              GUI_SCALE));
    // auto y = int(current_pos.y /
    //              (viewport->tileSet.size.second * viewport->scale *
    //              GUI_SCALE));
    // auto rx = x + viewport->view_x;
    // auto ry = y + viewport->view_y;
    // if (lockedPos) {
    //   rx = (*lockedPos).first;
    //   ry = (*lockedPos).second;
    //   x = rx - viewport->view_x;
    //   y = ry - viewport->view_y;
    // }
    // auto [cc, rz] = viewport->getCell(rx, ry, viewport->view_z);

    // if (!cc || (*cc)->type == CellType::UNKNOWN) {
    //   cursor.setOutlineColor(sf::Color(70, 70, 70));
    // } else {
    //   cursor.setOutlineColor(sf::Color(200, 124, 70));
    // }
    // if (lockedPos) {
    //   cursor.setOutlineColor(sf::Color(70, 70, 255));
    // }
    // cursor.setPosition(sf::Vector2f(x * viewport->tileSet.size.first,
    //                                 y * viewport->tileSet.size.second) *
    //                    viewport->scale * GUI_SCALE);
    // window->draw(cursor);

    ImGui::SFML::Update(*window, deltaClock.restart());
    ImGuiViewport *vp = ImGui::GetMainViewport();
    drawStatusBar(vp->Size.x, 16.0f * GUI_SCALE, 0.0f,
                  vp->Size.y - 24 * GUI_SCALE);

    if (DEBUG) {
      drawDocking(24.0f * GUI_SCALE);

      editor->Draw();
    }
    ImGui::SFML::Render(*window);

    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}

Application::~Application() {
  // entt::service_locator<GameData>::reset();
  entt::service_locator<hf::Viewport>::reset();
  /*
  if (jsThread.joinable()) {
    jsThread.join();
  }
  if (cacheThread.joinable()) {
    cacheThread.join();
  }
   */
  log.stop(APP_NAME);
}
