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

#include <editor/editor.hpp>
#include <fonts/material_design_icons.h>
#include <style/theme.h>

#include <hellfrost/deps.hpp>
#include <hellfrost/config.hpp>
#include <hellfrost/ui/drawEngine.hpp>

#ifdef __APPLE__
float GUI_SCALE = 2.0f;
#else
float GUI_SCALE = 1.f;
#endif

namespace hf = hellfrost;


Application::Application(std::string app_name, fs::path path,
                         std::string version, int s)
    : APP_NAME(app_name), VERSION(version), PATH(path) {

  log.start(APP_NAME);
  auto init_label = lu::color(fmt::color::light_blue, "App init");
  log.setLabelColor(init_label, fmt::color::light_blue);
  log.start(init_label);
  log.info("Seed:\t{}", lu::blue("{}", s));
  log.info("Path:\t{}", lu::gray("{}", PATH.string()));

  //TODO: inject with other class
  auto eventLog = LibLog::Logger(fmt::color::lime_green, "EVENT");
  eventLog.setParent(&log);
  entt::service_locator<LibLog::Logger>::set(eventLog);

  entt::monostate<"gui_scale"_hs>{} = GUI_SCALE;

  entt::service_locator<entt::registry>::empty();
  entt::service_locator<hf::event_emitter>::set(
      std::make_shared<hf::event_emitter>());
  // entt::service_locator<hf::GameData>::empty();

  auto config_path = PATH / fs::path("data/config.json");
  loadConfig(config_path);
  // saveConfig(config_path);

  setupGui();

  log.start("Init game systems");
  // std::shared_ptr<R::Generator> gen = std::make_shared<R::Generator>();
  engine = std::make_unique<hf::DrawEngine>(window, PATH);
  entt::service_locator<hf::DrawEngine>::set(engine);

  auto config = entt::service_locator<hf::Config>::get().lock();
  gm = std::make_shared<hf::GameManager>(PATH / config->data_file, s);
  entt::service_locator<hf::GameManager>::set(gm);
  log.stop("Init game systems");

  editor = std::make_shared<Editor>(PATH);

  setupHandlers();
  log.stop(init_label);
}

void Application::loadConfig(fs::path path) {
  log.debug("Config file: {}", lu::gray(path.string()));
  entt::service_locator<hf::Config>::empty();
  std::ifstream file(path, std::ios::in | std::ios::binary);
  cereal::JSONInputArchive iarchive(file);
  hf::Config conf;
  conf.serialize(iarchive);
  entt::service_locator<hf::Config>::set(conf);
}

void Application::saveConfig(fs::path path) {
  log.debug("Saving config to file: {}", lu::gray(path.string()));
  std::ofstream file(path, std::ios::out | std::ios::binary);
  cereal::JSONOutputArchive oarchive(file);
  auto conf = entt::service_locator<hf::Config>::ref();
  conf.serialize(oarchive);
}

void Application::setupHandlers() {
  auto eventLog = entt::service_locator<LibLog::Logger>::get().lock();
  eventLog->start("Setup handlers", true);
  auto emitter = entt::service_locator<hf::event_emitter>::get().lock();

  emitter->on<hf::regen_event>([&](const auto &p, auto &em) {
    auto seed = gm->seed;
    if (p.seed == -1) {
      seed = rand();
    } else if (p.seed >= 0) {
      seed = p.seed;
    }
    gm->start(seed);
    engine->invalidate();
  });

  // emitter->on<hf::location_update_event>([&](const auto &p, auto &em) {
  //   auto eventLog = entt::service_locator<LibLog::Logger>::get().lock();
  // //   gm->location->invalidate();
  // //   engine->clearCache();
  //   engine->invalidate();
  //   eventLog->debug("location update event");
  // });
  // emitter->on<hf::gm::generation_start>([&](const auto &p, auto &em) {
  // });

  emitter->on<hf::resize_event>([&](const auto &p, auto &em) {
    auto eventLog = entt::service_locator<LibLog::Logger>::get().lock();
    eventLog->start("resize event");

    sf::FloatRect visibleArea(0, 0, window->getSize().x, window->getSize().y);
    auto sv = sf::View(visibleArea);
    window->setView(sv);
    rectangle.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
    eventLog->stop("resize event");
  });

  eventLog->stop("Setup handlers");
}

void Application::setupGui() {
  log.start("GUI init");

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  Theme::Init();
  ImGui::GetStyle().ScaleAllSizes(GUI_SCALE);

  window = std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), APP_NAME,
                                sf::Style::Default, settings);
  window->setVerticalSyncEnabled(true);

  ImGui::SFML::Init(*window);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  window->resetGLStates();

  log.stop("GUI init");
}

void Application::processEvent(sf::Event event) {
  // log.setLabelColor("app process event", fmt::color::black);
  // log.start("app process event", true);
  ImGui::SFML::ProcessEvent(event);
  switch (event.type) {
  case sf::Event::KeyPressed:
    switch (event.key.code) {
    case sf::Keyboard::Escape:
      window->close();
      break;
    case sf::Keyboard::F1:
      DEBUG = !DEBUG;
      break;
    }
    break;
  case sf::Event::Resized: {
    auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
    emitter->publish<hf::resize_event>();
    break;
  }
  case sf::Event::GainedFocus:
    engine->invalidate();
    break;
  case sf::Event::Closed:
    window->close();
    break;
  }
  // log.stop("app process event", 10);
}

void Application::drawDocking(float padding) {
  ImGuiViewport *vp = ImGui::GetMainViewport();
  ImGuiWindowFlags window_flags =
      /*ImGuiWindowFlags_MenuBar | */ ImGuiWindowFlags_NoDocking;

  auto dockSpaceSize = vp->Size;
  dockSpaceSize.y -= padding;
  ImGui::SetNextWindowPos(vp->Pos);
  ImGui::SetNextWindowSize(dockSpaceSize);
  ImGui::SetNextWindowViewport(vp->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

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

  if (DEBUG) {
    editor->DrawStatusBar(window);
  } else {
    ImGui::SameLine();
    ImGui::Text("|  press F1 for Editor UI");
  }

  ImGui::PopFont();
  ImGui::End();
}

int Application::serve() {
  sf::Clock deltaClock;

  window->clear(sf::Color::Transparent);
  rectangle.setPosition(0, 0);
  rectangle.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));

  gm->start();
  engine->update();

  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      processEvent(event);
      if (DEBUG) {
        editor->processEvent(event);
      } else {
        //TODO: add class instead GameManager to avoid SFML dependency in hellfrost/game namespace
        // input_controller->processEvent(event);
      }
    }
    // sf::FloatRect visibleArea(0, 0, window->getSize().x,
    // window->getSize().y); auto sv = sf::View(visibleArea);
    // window->setView(sv);

    auto t = engine->Draw();
    rectangle.setTexture(&t);

    window->draw(rectangle);

    ImGui::SFML::Update(*window, deltaClock.restart());
    ImGuiViewport *vp = ImGui::GetMainViewport();
    drawStatusBar(vp->Size.x, 16.0f * GUI_SCALE, 0.0f,
                  vp->Size.y - 24 * GUI_SCALE);

    if (DEBUG) {
      drawDocking(24.0f * GUI_SCALE);
      editor->Draw(window);
    }
    ImGui::SFML::Render(*window);

    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}

Application::~Application() {
  entt::service_locator<hf::DrawEngine>::reset();
  log.stop(APP_NAME);
}
