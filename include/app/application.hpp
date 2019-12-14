#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>

#include <liblog/liblog.hpp>

class Tile;
class Viewport;
class Generator;
class Object;
class Application {
        bool needRedraw = true;
        std::vector<std::pair<int, int>> damage;
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
        void drawObjectsWindow();
        void drawTilesetWindow();

        void saveTileset();
        void drawCellInfo();
        void drawObjects(std::vector<std::shared_ptr<Object>>);
        void centerObject(std::shared_ptr<Object>);

        void renderTile(std::shared_ptr<sf::RenderTexture>, std::shared_ptr<Tile>);
        LibLog::Logger &log = LibLog::Logger::getInstance();
        int seed;
        int redraws;
        int tilesUpdated;
        void genLocation(int);

        std::shared_ptr<Generator> generator;
};


#endif // __APPLICATION_H_
