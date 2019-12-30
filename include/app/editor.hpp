#ifndef __EDITOR_H_
#define __EDITOR_H_
#include <lss/components.hpp>
#include <app/ui/drawEngine.hpp>
#include <imgui_entt_entity_editor.hpp>
#include <lss/gameManager.hpp>

typedef std::vector<entt::entity> entity_list;
struct tree_node {
  entity_list entities;
  std::map<std::string, std::shared_ptr<tree_node>> children;
};


namespace hf = hellfrost;
class Editor {
        fs::path PATH;

public:
    Editor(std::shared_ptr<GameManager> _gm, fs::path _path);

    int ts_idx = 0;
    std::vector<std::string> ts;


	void Meta(std::shared_ptr<entt::registry>, entt::entity);
	void Visible(std::shared_ptr<entt::registry>, entt::entity);
	void Position(std::shared_ptr<entt::registry>, entt::entity);
	void Renderable(std::shared_ptr<entt::registry>, entt::entity);
    void InEditor(std::shared_ptr<entt::registry>, entt::entity);

    void Pickable(std::shared_ptr<entt::registry>, entt::entity);
    void Wearable(std::shared_ptr<entt::registry>, entt::entity);

    void Glow(std::shared_ptr<entt::registry>, entt::entity);

    void drawSelectedInfo();
    void drawCellInfo(std::optional<std::shared_ptr<Cell>> cell);
    void drawSpecWindow();
    void drawObjectsWindow();
    void drawTilesetWindow();
    void saveTileset();

template <typename... T>
    void drawEntityTree(std::shared_ptr<entt::registry>);

    void drawObjects(std::vector<std::shared_ptr<Object>> objects);
    void drawEntityInfo(entt::entity e, std::shared_ptr<entt::registry>);
    void drawEntityEditor(std::shared_ptr<entt::registry>);


    std::shared_ptr<GameManager> gm;
    MM::ImGuiEntityEditor<entt::basic_registry<entt::entity>> entityEditor;

    void processRegistry(std::shared_ptr<entt::registry>);
};

#endif // __EDITOR_H_
