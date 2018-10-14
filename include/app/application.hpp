#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>

class Application {
public:
        Application(std::string v);
        ~Application() = default;
        sf::RenderWindow *window;
        void processEvent(sf::Event event);
        int serve();
};


#endif // __APPLICATION_H_
