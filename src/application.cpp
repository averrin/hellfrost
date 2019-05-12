#include <thread>   // for access to this thread

#include "app/application.hpp"

#include <imgui-sfml/imgui-SFML.h>
#include <imgui/imgui.h>

#include <libcolor/libcolor.hpp>
#include <app/fonts/material_design_icons.h>
#include <app/style/theme.h>

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

void Application::processEvent(sf::Event event) {
  ImGui::SFML::ProcessEvent(event);
  switch (event.type) {
  case sf::Event::KeyPressed:
    switch (event.key.code) {
    case sf::Keyboard::Escape:
      window->close();
      break;
    }
    break;
  case sf::Event::Closed:
    window->close();
    break;
  }
}

float value = 1.f;
int count = 15;
float size = 50.f;

void Application::DrawStatusBar(float width, float height, float pos_x,
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

void Application::drawMainWindow() {
  ImGui::Begin(APP_NAME.c_str());
  ImGui::Text("\n\nSFML + ImGui starter (%s + %s)\n\n", VERSION.c_str(),
              IMGUI_VERSION);
  ImGui::SliderInt("Count", &count, 1, 200);
  ImGui::SliderFloat("Size", &size, 4.f, 200.f);
  ImGui::SliderFloat("Value", &value, 0.f, 1.f);
  ImGui::End();
}

int Application::serve() {
  log.info("serve");
  sf::Clock deltaClock;
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

    auto bgColor = sf::Color(23, 23, 23);
    window->clear(bgColor);

    float padding = size / 10;
    auto yOffset = 0.f;
    for (auto y = 0; y < count; y++) {
      auto offset = 0.f;
      auto startColor = Color::fromWebName("red");
      startColor.saturation(1 - y / float(count));
      startColor.value(value);
      for (auto i = 0; i < count; i++) {
        auto sfColor =
            sf::Color(startColor.red(), startColor.green(), startColor.blue());

        sf::VertexArray quad(sf::Quads, 4);

        quad[0].position = sf::Vector2f(padding + offset, padding + yOffset);
        quad[0].color = sfColor;
        quad[1].position =
            sf::Vector2f(padding + size + offset, padding + yOffset);
        quad[1].color = sfColor;
        quad[2].position =
            sf::Vector2f(padding + size + offset, padding + size + yOffset);
        quad[2].color = sfColor;
        quad[3].position =
            sf::Vector2f(padding + offset, padding + size + yOffset);
        quad[3].color = sfColor;

        window->draw(quad);
        offset += size + padding;
        startColor.hue(startColor.hue() + 360.f / count);
      }
      yOffset += size + padding;
    }

    ImGui::SFML::Update(*window, deltaClock.restart());

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

    DrawStatusBar(viewport->Size.x, 16.0f, 0.0f, viewport->Size.y - 24);

    drawMainWindow();
    ImGui::SFML::Render(*window);
    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}
