#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>

#include <liblog/liblog.hpp>
#include <app/ui/tile.hpp>

class Application {
public:
        Application(std::string, std::string);
        ~Application() = default;

        std::string APP_NAME;
        std::string VERSION;

        sf::RenderWindow *window;
        void processEvent(sf::Event event);
        int serve();
        void drawStatusBar(float width, float height, float pos_x, float pos_y);
        void drawDocking();
        void drawMainWindow(std::shared_ptr<Viewport>);
        Logger& log = Logger::getInstance();
};


#endif // __APPLICATION_H_
