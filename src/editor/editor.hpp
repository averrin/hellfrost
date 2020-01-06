#ifndef __EDITOR_H_
#define __EDITOR_H_
#include <thread>

#include <dukglue/dukglue.h>
#include <imgui-etc/imgui_entt_entity_editor.hpp>
#include <imguicolortextedit/TextEditor.h>

#include <hellfrost/deps.hpp>
#include <hellfrost/game/cell.hpp>
#include <hellfrost/game/gameManager.hpp>
#include <hellfrost/ui/drawEngine.hpp>

#include <editor/console.hpp>
// #include <editor/scriptWrappers.hpp>

namespace hf = hellfrost;

typedef std::vector<entt::entity> entity_list;
struct tree_node {
  entity_list entities;
  std::map<std::string, std::shared_ptr<tree_node>> children;
};

class Editor {
public:
  Editor(fs::path);

  void Draw();
  void processEvent(sf::Event event);

private:
  LibLog::Logger log = LibLog::Logger(fmt::color::cadet_blue, "EDIT");

  fs::path PATH;
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
  void drawCellInfo(std::optional<std::shared_ptr<hf::Cell>> cell);
  void drawSpecWindow();
  void drawObjectsWindow();
  void drawTilesetWindow();
  void saveTileset();
  void dukEditorWindow();

  void drawLocationWindow();
  void drawViewportWindow();

  template <typename... T> void drawEntityTree(std::shared_ptr<entt::registry>);

  void drawEntityInfo(entt::entity e, std::shared_ptr<entt::registry>);
  void drawEntityEditor(std::shared_ptr<entt::registry>);

  std::shared_ptr<hf::GameManager> gm;
  MM::ImGuiEntityEditor<entt::basic_registry<entt::entity>> entityEditor;

  Console console;
  TextEditor duk_editor;
  TextEditor::ErrorMarkers markers;

  duk_context *duk_ctx = duk_create_heap_default();
  void duk_log(const std::string msg);
  void duk_exec(const char *code);

  std::optional<std::pair<int, int>> lockedPos = std::nullopt;
  std::thread jsThread;
};

#endif // __EDITOR_H_
