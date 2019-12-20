#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

#include <liblog/liblog.hpp>
#include <lss/gameManager.hpp>
#include <app/ui/tile.hpp>
#include <app/ui/viewport.hpp>
#include <imgui_entt_entity_editor.hpp>
#include "lss/components.hpp"


struct event_emitter: entt::emitter<event_emitter> {};
struct redraw_event {};
struct center_event {int x; int y;};
struct damage_event {int x; int y;};

class Object;
class Application {
        bool needRedraw = true;
        std::vector<std::pair<int, int>> damage;
public:
        Application(std::string, fs::path, std::string, int);
        ~Application() = default;

        void setupGui();

        std::string APP_NAME;
        std::string VERSION;
        fs::path PATH;

        std::unique_ptr<GameManager> gm;
        MM::ImGuiEntityEditor<decltype(gm->registry)> editor;
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

        void drawEntityInfo(entt::entity);

        void saveTileset();
        void drawCellInfo();
        void drawObjects(std::vector<std::shared_ptr<Object>>);
        void centerObject(std::shared_ptr<Object>);
                void genLocation(int);
                int seed;

        void renderTile(std::shared_ptr<sf::RenderTexture>, std::shared_ptr<Tile>);
        void renderEntity(std::shared_ptr<sf::RenderTexture> canvas, entt::entity e) ;

        LibLog::Logger &log = LibLog::Logger::getInstance();
        int redraws;
        int tilesUpdated;
};


#endif // __APPLICATION_H_
