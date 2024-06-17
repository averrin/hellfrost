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

public:
  Editor(std::shared_ptr<GameManager> _gm, fs::path _path);

  int ts_idx = 0;
  std::vector<std::string> ts;

  void drawSelectedInfo();
  void drawCellInfo(std::optional<std::shared_ptr<Cell>> cell);
  std::optional<std::shared_ptr<Cell>> selectedCell;
  void drawSpecWindow();
  void drawObjectsWindow();
  void drawTilesetWindow();
  void drawLocationWindow(std::shared_ptr<Location>);
  void saveTileset();
  void drawTemplates(sol::state &lua);


  template <typename... T> void drawEntityTree();

  static void drawEntityInfo(entt::entity e);
  static void drawEntityEditor();

  std::shared_ptr<GameManager> gm;
  //MM::EntityEditor<entt::basic_registry<entt::entity>> entityEditor;
  // static MM::EntityEditor<entt::entity> entityEditor;

  void processRegistry(entt::registry &registry);
};

#endif // __EDITOR_H_
