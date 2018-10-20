#include "app/application.hpp"

#include <liblog/liblog.hpp>

#include <imgui-sfml/imgui-SFML.h>
#include <imgui.h>

Application::Application(std::string v) {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    ImGui::CreateContext();
    // L().info("init");

    // ImGuiIO &io = ImGui::GetIO();
    // io.Fonts->AddFontFromFileTTF("./font.ttf", 15.0f);

    // char windowTitle[6] = "SiuiS";
    window = new sf::RenderWindow(sf::VideoMode(800, 600), "SFML + ImGui = <3");
    L().info("init sfml");
    window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*window);

    // window->setTitle(windowTitle);
    window->resetGLStates();
}

void Application::processEvent(sf::Event event) {
    ImGui::SFML::ProcessEvent(event);
    switch (event.type) {
    case sf::Event::Closed:
      window->close();
      break;
    }
}

int Application::serve() {
    L().info("serve");
    sf::Clock deltaClock;
    while (window->isOpen()) {
      sf::Event event;
      while (window->pollEvent(event)) {
        processEvent(event);
      }
      ImGui::SFML::Update(*window, deltaClock.restart());
      ImGui::SFML::Render(*window);
      window->display();
    }
    L().info("shutdown");
    ImGui::SFML::Shutdown();
    return 0;
}
