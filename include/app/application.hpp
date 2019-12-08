#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>

#include <liblog/liblog.hpp>
#include <app/ui/tile.hpp>

class Application {
        bool needRedraw = true;
public:
        Application(std::string, std::string, int);
        ~Application() = default;

        void setupGui();

        std::string APP_NAME;
        std::string VERSION;

        sf::RenderWindow *window;
        std::shared_ptr<sf::RenderTexture> cacheTex;
        std::shared_ptr<Viewport> view_map;
        void processEvent(sf::Event event);
        int serve();
        void drawStatusBar(float width, float height, float pos_x, float pos_y);
        void drawDocking();
        void drawMainWindow();
        void renderTile(std::shared_ptr<sf::RenderTexture>, std::shared_ptr<Tile>);
        Logger& log = Logger::getInstance();
        int seed;
        int redraws;
};


#endif // __APPLICATION_H_
