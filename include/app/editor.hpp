#ifndef __EDITOR_H_
#define __EDITOR_H_
#include <app/ui/drawEngine.hpp>
#include <imgui-SFML.h>
#include <imgui_entt_entity_editor.hpp>
#include <lss/components.hpp>
#include <lss/gameManager.hpp>

typedef std::vector<entt::entity> entity_list;
struct tree_node {
  entity_list entities;
  std::map<std::string, std::shared_ptr<tree_node>> children;
};

namespace hf = hellfrost;
class Editor {
  fs::path PATH;
  std::string new_key_sprite;
  std::string new_key_color;
  std::string new_editor_name;
  std::string new_key_prob;
  std::string new_key_lt;
  std::string new_key_lf;
  bool show_unknown = true;
  float new_prob = 0;

public:
  Editor(std::shared_ptr<GameManager> _gm, fs::path _path);

  int ts_idx = 0;
  std::vector<std::string> ts;

  void Meta(entt::registry &registry, entt::entity);
  void Tags(entt::registry &registry, entt::entity);
  void Visible(entt::registry &registry, entt::entity);
  void Position(entt::registry &registry, entt::entity);
  void Renderable(entt::registry &registry, entt::entity);
  void PlayerComponent(entt::registry &registry, entt::entity);
  void CreatureComponent(entt::registry &registry, entt::entity);
  void Obstacle(entt::registry &registry, entt::entity);
  void Script(entt::registry &registry, entt::entity);
  void Vision(entt::registry &registry, entt::entity);
  void InEditor(entt::registry &registry, entt::entity);

  void Pickable(entt::registry &registry, entt::entity);
  void Wearable(entt::registry &registry, entt::entity);

  void Glow(entt::registry &registry, entt::entity);
  void CellW(entt::registry &registry, entt::entity);
  void RoomW(entt::registry &registry, entt::entity);
  void Children(entt::registry &registry, entt::entity);
  void Size(entt::registry &registry, entt::entity);

  void drawSelectedInfo();
  void drawCellInfo(std::optional<std::shared_ptr<Cell>> cell);
  std::optional<std::shared_ptr<Cell>> selectedCell;
  void drawSpecWindow();
  void drawObjectsWindow();
  void drawTilesetWindow();
  void drawLocationWindow(std::shared_ptr<Location>);
  void saveTileset();
  void drawTemplates(sol::state &lua);


  template <typename... T> void drawEntityTree(entt::registry &registry);

  void drawEntityInfo(entt::entity e, entt::registry &registry);
  void drawEntityEditor(entt::registry &registry);

  std::shared_ptr<GameManager> gm;
  //MM::EntityEditor<entt::basic_registry<entt::entity>> entityEditor;
  MM::EntityEditor<entt::entity> entityEditor;

  void processRegistry(entt::registry &registry);
};

#endif // __EDITOR_H_
