#include "app/application.hpp"

#include <imgui.h>
#include <imgui-sfml/imgui-SFML.h>

Application::Application(std::string app_name, std::string version) : APP_NAME(app_name), VERSION(version) {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();

    window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), APP_NAME,
                                 sf::Style::Default, settings);
    window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*window);

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
    case sf::Event::Closed:
      window->close();
      break;
    }
}

  void Application::drawMainWindow() {
    ImGui::Begin(APP_NAME.c_str());
    ImGui::Text("\n\nSFML + ImGui starter (%s)\n\n", VERSION.c_str());
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
      sf::Color bgColor = sf::Color(23, 23, 23);
      window->clear(bgColor);
      ImGui::SFML::Update(*window, deltaClock.restart());
      drawMainWindow();
      ImGui::SFML::Render(*window);
      window->display();
    }
    log.info("shutdown");
    ImGui::SFML::Shutdown();
    return 0;
}
