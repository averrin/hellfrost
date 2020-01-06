#include <editor/editor.hpp>

#include <SFML/Graphics.hpp>
#include <imgui-etc/imgui-stl.hpp>
#include <imgui-sfml/imgui-SFML.h>
#include <magic_enum.hpp>
#include <dukglue/dukglue.h>

#include <hellfrost/deps.hpp>
#include <hellfrost/ui/drawEngine.hpp>
#include <hellfrost/ui/viewport.hpp>

// TODO: migrate to hellfrost
#include <lss/game/light.hpp>
#include <lss/game/wearableType.hpp>
// #include <lss/generator/mapUtils.hpp>

#include <fonts/material_design_icons.h>

Editor::Editor(fs::path _path) : PATH(_path) {
  log.setParent(&LibLog::Logger::getInstance());
  gm = entt::service_locator<hf::GameManager>::get().lock();

  auto n = 0;
  for (auto entity : fs::directory_iterator(PATH / fs::path("data/tilesets"))) {
    if (fs::is_directory(entity)) {
      auto tileset = entity.path().filename().string();
      if (tileset == "ascii") { // TODO: add config
        ts_idx = n;
      }
      ts.push_back(tileset);
      n++;
    }
  }
//   auto emitter = entt::service_locator<hf::event_emitter>::get().lock();

//   emitter->on<hf::log_event>([&](const auto &event, auto &em) {
//     log.info(lu::yellow("DUK"), event.msg);
//     console.AddLog(event.msg.data());
//   });

//   emitter->on<hf::clear_markers_event>([&](const auto &event, auto &em) {
//     markers.clear();
//     duk_editor.SetErrorMarkers(markers);
//   });

//   emitter->on<hf::duk_error>([&](auto event, auto &em) {
//     std::string err = std::string(event.msg);
//     std::string line;
//     std::vector<std::string> lines;
//     std::istringstream ss(err);
//     std::regex re("eval:(\\d+)");
//     std::smatch m;
//     while (std::getline(ss, line, '\n')) {
//       lines.push_back(line);
//     }
//     std::regex_search(lines.back(), m, re);

//     markers.insert(std::make_pair<int, std::string>(
//         std::atoi(std::string(m[m.size() - 1]).data()),
//         std::string(lines.front())));
//     duk_editor.SetErrorMarkers(markers);

//     console.AddLog("[error] %s", event.msg.data());
//   });

//   emitter->on<hf::exec_event>(
//       [&](auto event, auto &em) { duk_exec(event.code.data()); });

//   // TODO: move to separate class
//   /*
//   duk_console_init(duk_ctx, 0);
//   dukglue_register_function(duk_ctx, &duk_log_ev, "log");
//   dukglue_register_function(duk_ctx, &duk_sleep, "sleep");
//   dukglue_register_function(duk_ctx, &duk_regen, "regen");

//   dukglue_register_global(duk_ctx, engine, "engine");
//   dukglue_register_method(duk_ctx, &DrawEngine::invalidate, "invalidate");
// */
//   auto fileToEdit = PATH / "data/init.js";

//   {
//     std::ifstream t(fileToEdit);
//     if (t.good()) {
//       std::string str((std::istreambuf_iterator<char>(t)),
//                       std::istreambuf_iterator<char>());
//       duk_editor.SetText(str);
//     }
//   }
}

void duk_log_ev(const std::string msg) {
  auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
  emitter->publish<hf::log_event>(msg);
}

void duk_regen(const int s) {
  auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
  emitter->publish<hf::regen_event>(s);
}

void duk_sleep(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Editor::drawEntityEditor(std::shared_ptr<entt::registry> registry) {
  entityEditor.registerTrivial<hf::meta>(*registry, "Meta");
  entityEditor.registerTrivial<hf::position>(*registry, "Position");
  entityEditor.registerTrivial<hf::visible>(*registry, "Visible");
  entityEditor.registerTrivial<hf::renderable>(*registry, "Renderable");
  entityEditor.registerTrivial<hf::ineditor>(*registry, "Editor");
  entityEditor.registerTrivial<hf::pickable>(*registry, "Pickable");
  entityEditor.registerTrivial<hf::wearable>(*registry, "Wearable");
  entityEditor.registerTrivial<hf::glow>(*registry, "Glow");

  entityEditor.registerComponentWidgetFn(
      registry->type<hf::meta>(),
      [&](entt::registry &reg, auto e) { Meta(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::ineditor>(),
      [&](entt::registry &reg, auto e) { InEditor(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::position>(),
      [&](entt::registry &reg, auto e) { Position(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::visible>(),
      [&](entt::registry &reg, auto e) { Visible(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::renderable>(),
      [&](entt::registry &reg, auto e) { Renderable(registry, e); });

  entityEditor.registerComponentWidgetFn(
      registry->type<hf::pickable>(),
      [&](entt::registry &reg, auto e) { Pickable(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::wearable>(),
      [&](entt::registry &reg, auto e) { Wearable(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::glow>(),
      [&](entt::registry &reg, auto e) { Glow(registry, e); });

  entityEditor.registerToolbarFn([&](entt::registry &reg, auto e) {
    auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
    if (reg.has<entt::tag<"proto"_hs>>(e)) {
      if (ImGui::Button(ICON_MDI_CONTENT_DUPLICATE)) {
        // auto data = entt::service_locator<GameData>::get().lock();
        // auto ne = gm->registry->create();
        // gm->registry->stomp(ne, e, *data->prototypes,
        //                     entt::exclude<entt::tag<"proto"_hs>>);
        // gm->registry->assign<hf::ingame>(ne);
        // gm->registry->assign<hf::position>(ne, 0, 0, 0);
      }
      ImGui::SameLine();
    }
    if (reg.has<hf::position>(e)) {
      auto &p = reg.get<hf::position>(e);

      if (ImGui::Button(ICON_MDI_TARGET)) {
        emitter->publish<hf::center_event>(p.x, p.y);
      }
      ImGui::SameLine();
    }
    if (reg.has<hf::ineditor>(e)) {
      auto &ie = reg.get<hf::ineditor>(e);

      if (ImGui::Button(ICON_MDI_SELECT)) {
        ie.selected = !ie.selected;
        reg.assign_or_replace<hellfrost::ineditor>(e, ie);
      }
      ImGui::SameLine();
    }
    if (ImGui::Button(ICON_MDI_DELETE)) {
      if (reg.has<hf::renderable>(e)) {
        // auto &p = reg.get<hf::position>(e);
        reg.destroy(e);
        // FIXME: use damage_event
        emitter->publish<hf::redraw_event>();
      } else {
        reg.destroy(e);
      }
    }
  });
}

void Editor::Pickable(std::shared_ptr<entt::registry> registry,
                      entt::entity e) {
  auto &p = registry->get<hf::pickable>(e);
  ImGui::InputText("Category", p.category.name);
  ImGui::InputText("Unidentified name", p.unidName);
  ImGui::InputInt("Count", &p.count);
  ImGui::Checkbox("Identfied", &p.identified);
}
void Editor::Wearable(std::shared_ptr<entt::registry> registry,
                      entt::entity e) {
  auto &w = registry->get<hf::wearable>(e);

  constexpr auto types = magic_enum::enum_values<WearableType>();
  constexpr auto types_names = magic_enum::enum_names<WearableType>();
  std::vector<std::string> names;
  for (auto n : types_names) {
    names.push_back(std::string(n));
  }
  auto wt = static_cast<int>(w.wearableType);
  if (ImGui::Combo("Wearable type", &wt, names)) {
    w.wearableType = types[wt];
  }
  ImGui::InputInt("Durability", &w.durability);
}

void Editor::Visible(std::shared_ptr<entt::registry> registry, entt::entity e) {
  auto &v = registry->get<hf::visible>(e);
  ImGui::InputText("Type", v.type);
  ImGui::InputText("Sign", v.sign);
}

void Editor::Glow(std::shared_ptr<entt::registry> registry, entt::entity e) {
  auto &g = registry->get<hf::glow>(e);
  ImGui::InputFloat("Distance", &g.distance);
  constexpr auto types = magic_enum::enum_values<LightType>();
  constexpr auto types_names = magic_enum::enum_names<LightType>();
  std::vector<std::string> names;
  for (auto n : types_names) {
    names.push_back(std::string(n));
  }
  auto gt = static_cast<int>(g.type);
  if (ImGui::Combo("Wearable type", &gt, names)) {
    g.type = types[gt];
  }
  ImGui::Checkbox("Stable", &g.stable);
}

void Editor::InEditor(std::shared_ptr<entt::registry> registry,
                      entt::entity e) {
  auto ie = hf::ineditor{};
  if (registry->has<hf::ineditor>(e)) {
    ie = registry->get<hf::ineditor>(e);
  } else {
    registry->assign_or_replace<hf::ineditor>(e, ie);
    ie = registry->get<hf::ineditor>(e);
  }

  ImGui::InputText("Name", ie.name);
  if (ImGui::Checkbox("Selected", &ie.selected)) {
    registry->assign_or_replace<hf::ineditor>(e, ie);
  }
  if (ie.folders.size() > 0) {
    std::string path = "";
    for (auto f : ie.folders) {
      path = fmt::format("{} > {}", path, f);
    }
    ImGui::BulletText("Folders: %s", path.c_str());
  }
  ImGui::ListEdit("Folders", ie.folders);
  if (ie.icon != "") {
    ImGui::BulletText("%s", fmt::format("Icon: {}", ie.icon).c_str());
  }
}

void Editor::Meta(std::shared_ptr<entt::registry> registry, entt::entity e) {
  auto &m = registry->get<hf::meta>(e);
  ImGui::InputText("Name", m.name);
  ImGui::InputText("Description", m.description);
  ImGui::InputText("ID", m.id);
}
void Editor::Position(std::shared_ptr<entt::registry> registry,
                      entt::entity e) {
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
  auto &p = registry->get<hf::position>(e);
  if (ImGui::Button(ICON_MDI_TARGET)) {
    emitter->publish<hf::center_event>(p.x, p.y);
  }
  ImGui::SameLine();
  ImGui::SetNextItemWidth(90 * GUI_SCALE);
  if (ImGui::InputInt("x##Position", &p.x)) {
    registry->assign_or_replace<hf::position>(e, p);
  }
  ImGui::SameLine();
  ImGui::SetNextItemWidth(90 * GUI_SCALE);
  if (ImGui::InputInt("y##Position", &p.y)) {
    registry->assign_or_replace<hf::position>(e, p);
  }
  ImGui::SameLine();
  ImGui::SetNextItemWidth(90 * GUI_SCALE);
  if (ImGui::InputInt("z##Position", &p.z)) {
    registry->assign_or_replace<hf::position>(e, p);
  }
}
void Editor::Renderable(std::shared_ptr<entt::registry> registry,
                        entt::entity e) {
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  auto r = hf::renderable{};
  if (registry->has<hf::renderable>(e)) {
    r = registry->get<hf::renderable>(e);
  } else {
    registry->assign_or_replace<hf::renderable>(e, r);
    r = registry->get<hf::renderable>(e);
  }

  if (ImGui::Checkbox("Hidden", &r.hidden)) {
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  ImGui::SameLine();
  ImGui::SetNextItemWidth(90 * GUI_SCALE);
  if (ImGui::InputInt("zIndex", &r.zIndex)) {
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  auto k = r.spriteKey;
  auto v = viewport->tileSet.sprites[k];
  sf::Sprite s;
  s.setTexture(viewport->tilesTextures[v[0]]);
  s.setTextureRect(viewport->getTileRect(v[1], v[2]));
  ImGui::Image(s,
               sf::Vector2f(viewport->tileSet.size.first * GUI_SCALE,
                            viewport->tileSet.size.second * GUI_SCALE),
               sf::Color::White, sf::Color::Transparent);
  ImGui::SameLine();
  std::vector<const char *> _ts;
  int s_idx = 0;
  auto n = 0;
  std::transform(viewport->tileSet.sprites.begin(),
                 viewport->tileSet.sprites.end(), std::back_inserter(_ts),
                 [&](auto sk) {
                   if (sk.first == r.spriteKey) {
                     s_idx = n;
                   }
                   char *r = new char[sk.first.size() + 1];
                   std::strcpy(r, sk.first.c_str());
                   n++;
                   return r;
                 });
  if (ImGui::Combo("Sprite", &s_idx, _ts.data(), _ts.size())) {
    r.spriteKey = std::string(_ts[s_idx]);
    registry->assign_or_replace<hf::renderable>(e, r);
  }

  ImGui::SetNextItemWidth(150);
  if (ImGui::InputText("Color", r.fgColor, 10)) {
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  ImGui::SameLine();
  auto color = Color::fromHexString(r.fgColor);
  float col[4] = {
      color.r / 255.f,
      color.g / 255.f,
      color.b / 255.f,
      color.a / 255.f,
  };
  if (ImGui::ColorEdit4(r.fgColor.c_str(), col,
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoLabel |
                            ImGuiColorEditFlags_AlphaPreview |
                            ImGuiColorEditFlags_AlphaBar)) {
    auto c = Color(col[0] * 255, col[1] * 255, col[2] * 255, col[3] * 255);
    r.fgColor = c.hexA();
    registry->assign_or_replace<hf::renderable>(e, r);
  }

  if (ImGui::Checkbox("Has BG", &r.hasBg)) {
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  if (r.hasBg) {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("BG Color", r.bgColor, 10)) {
      registry->assign_or_replace<hf::renderable>(e, r);
    }
    ImGui::SameLine();
    color = Color::fromHexString(r.bgColor);
    float bcol[4] = {
        color.r / 255.f,
        color.g / 255.f,
        color.b / 255.f,
        color.a / 255.f,
    };
    if (ImGui::ColorEdit4(r.bgColor.c_str(), bcol,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaPreview |
                              ImGuiColorEditFlags_AlphaBar)) {
      auto c =
          Color(bcol[0] * 255, bcol[1] * 255, bcol[2] * 255, bcol[3] * 255);
      r.bgColor = c.hexA();
      registry->assign_or_replace<hf::renderable>(e, r);
    }
  }

  if (ImGui::Checkbox("Has border", &r.hasBorder)) {
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  if (r.hasBorder) {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("Border Color", r.borderColor, 10)) {
      registry->assign_or_replace<hf::renderable>(e, r);
    }
    ImGui::SameLine();
    color = Color::fromHexString(r.borderColor);
    float bcol[4] = {
        color.r / 255.f,
        color.g / 255.f,
        color.b / 255.f,
        color.a / 255.f,
    };
    if (ImGui::ColorEdit4(r.borderColor.c_str(), bcol,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaPreview |
                              ImGuiColorEditFlags_AlphaBar)) {
      auto c =
          Color(bcol[0] * 255, bcol[1] * 255, bcol[2] * 255, bcol[3] * 255);
      r.borderColor = c.hexA();
      registry->assign_or_replace<hf::renderable>(e, r);
    }
  }
}

void Editor::drawSpecWindow() {
  if (!ImGui::Begin("Specification")) {
    ImGui::End();
    return;
  }
  auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  auto engine = entt::service_locator<hf::DrawEngine>::get().lock();
  // auto data = entt::service_locator<GameData>::get().lock();

  if (ImGui::Button("Apply")) {
    emitter->publish<hf::regen_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button("Reload")) {
    gm->loadData();
    emitter->publish<hf::resize_event>();
    emitter->publish<hf::regen_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    gm->saveData();
  }
  ImGui::Separator();

  /*
  if (ImGui::CollapsingHeader("Probabilities")) {
    for (auto &[k, _] : data->probability) {
      ImGui::SetNextItemWidth(80);
      ImGui::InputFloat(k.c_str(), &data->probability[k]);
    }
  }
  if (ImGui::CollapsingHeader("Item specs")) {
    for (auto &[k, spec] : data->itemSpecs) {
      char *name = new char[spec.name.size() + 1];
      std::strcpy(name, spec.name.c_str());
      if (ImGui::TreeNode(k.c_str())) {
        ImGui::Button("Delete");

        ImGui::InputText("Name", spec.name);
        ImGui::InputText("Sign", spec.sign);
        ImGui::Text("Category: %s", spec.category.name.c_str());
        ImGui::InputInt("Durability", &spec.durability);
        ImGui::Checkbox("Identified", &spec.identified);
        ImGui::TreePop();
      }
    }
  }
  if (ImGui::CollapsingHeader("Terrain specs")) {
    for (auto &[k, spec] : data->terrainSpecs) {
      if (ImGui::TreeNode(k.c_str())) {
        ImGui::Button("Delete");
        ImGui::InputText("Name", spec.name);
        ImGui::InputText("Sign", spec.sign);
        ImGui::Checkbox("seeThrough", &spec.seeThrough);
        ImGui::SameLine();
        ImGui::Checkbox("passThrough", &spec.passThrough);
        ImGui::InputInt("action points left", &spec.apLeft);
        ImGui::Checkbox("destructable", &spec.destructable);
        if (spec.light.distance > 0) {
          if (ImGui::CollapsingHeader("Light specs")) {
            ImGui::BulletText("Light distance: %f", spec.light.distance);
            ImGui::BulletText("Light type: %d", spec.light.type);
            ImGui::BulletText("Light stable: %s",
                              spec.light.stable ? "true" : "false");
          }
        }
        ImGui::TreePop();
      }
    }
  }

  auto n = 0;
  std::function<void(LootBox lb)> it;
  it = [&](LootBox lb) {
    n++;
    if (ImGui::TreeNode(
            fmt::format("{}##{}", lb.name != "" ? lb.name : "LootBox", n)
                .c_str())) {
      ImGui::InputText("Name", lb.name);
      ImGui::Checkbox("Exclusive", &lb.exclusive);
      ImGui::SameLine();
      ImGui::SetNextItemWidth(150);
      ImGui::InputFloat("Probability", &lb.probability);
      ImGui::ListEdit("Items", lb.itemKeys);
      if (lb.children.size() > 0) {
        ImGui::Text("Children:");
        for (auto ch : lb.children) {
          it(ch);
        }
      }
      ImGui::TreePop();
    }
  };

  if (ImGui::CollapsingHeader("Loot boxes")) {
    for (auto lb : data->lootBoxes) {
      it(lb);
    }
  }

  if (ImGui::CollapsingHeader("Prototypes")) {
    drawEntityTree<entt::tag<"proto"_hs>>(data->prototypes);
  }
  */
  ImGui::End();
}

void Editor::drawSelectedInfo() {
  if (!ImGui::Begin("Selected objects")) {
    ImGui::End();
    return;
  }

  auto ents = gm->registry->view<hf::ineditor>();

  auto n = 0;
  if (ents.size() > 0) {
    for (auto e : ents) {
      auto p = gm->registry->get<hf::ineditor>(e);
      if (p.selected) {
        drawEntityInfo(e, gm->registry);
        n++;
      }
    }
  }
  if (n == 0) {
    ImGui::Text("No selected entities.");
  }

  ImGui::End();
}

template <typename... T>
void Editor::drawEntityTree(std::shared_ptr<entt::registry> registry) {
  auto ents = registry->view<T...>();
  auto entityTree = std::make_shared<tree_node>();

  for (auto e : ents) {
    if (!registry->has<hf::ineditor>(e) ||
        registry->get<hf::ineditor>(e).folders.size() == 0 ||
        registry->get<hf::ineditor>(e).folders.front() == "") {
      entityTree->entities.push_back(e);
    } else {
      auto ie = registry->get<hf::ineditor>(e);
      if (ie.folders.size() == 0) {
        entityTree->entities.push_back(e);
      } else {
        auto current_node = entityTree;
        for (auto f : ie.folders) {
          if (current_node->children.find(f) == current_node->children.end()) {
            current_node->children[f] = std::make_shared<tree_node>();
          }
          current_node = current_node->children[f];
        }
        current_node->entities.push_back(e);
      }
    }
  }

  if (ImGui::TreeNode("ents", "%s Entities", ICON_MDI_CUBE_OUTLINE)) {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Count: %lu", ents.size());
    ImGui::SameLine();
    if (ImGui::Button("New Entity")) {
      auto e = registry->create();
      registry->assign<T...>(e);
    }

    std::function<void(std::shared_ptr<tree_node> v)> it;
    it = [&](std::shared_ptr<tree_node> v) {
      for (auto e : v->entities) {
        drawEntityInfo(e, registry);
      }
      for (auto [fn, f] : v->children) {
        if (ImGui::TreeNode(
                fmt::format("{} {}", ICON_MDI_FOLDER, fn).c_str())) {
          it(f);
          ImGui::TreePop();
        }
      }
    };

    it(entityTree);

    ImGui::TreePop();
  }
}

void Editor::drawObjectsWindow() {
  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  if (!ImGui::Begin("Objects")) {
    ImGui::End();
    return;
  }
  // drawObjects(viewport->regions.front()->location->objects);
  drawEntityTree<hf::ingame>(gm->registry);
  ImGui::End();
}

void Editor::drawEntityInfo(entt::entity e,
                            std::shared_ptr<entt::registry> registry) {
  auto t = registry->has<hf::meta>(e) ? registry->get<hf::meta>(e).name.c_str()
                                      : "Entity";
  t = (registry->has<hf::ineditor>(e) &&
       registry->get<hf::ineditor>(e).name != "")
          ? registry->get<hf::ineditor>(e).name.c_str()
          : t;
  auto icon = (registry->has<hf::ineditor>(e) &&
               registry->get<hf::ineditor>(e).icon != "")
                  ? registry->get<hf::ineditor>(e).icon.c_str()
                  : ICON_MDI_CUBE_OUTLINE;
  auto selected = (registry->has<hf::ineditor>(e) &&
                   registry->get<hf::ineditor>(e).selected);
  auto col = selected ? (ImVec4)ImColor::HSV(1.f / 6.f, 0.86f, 1.0f)
                      : (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.99f);
  ImGui::PushStyleColor(ImGuiCol_Text, col);
  auto title = fmt::format("{} {}: {}", icon, t, (int)registry->entity(e));
  drawEntityEditor(registry);
  if (ImGui::TreeNode(title.c_str())) {
    ImGui::PopStyleColor(1);
    entityEditor.renderImGui(*registry, e);
    ImGui::TreePop();
  } else {
    ImGui::PopStyleColor(1);
  }
}

void Editor::drawCellInfo(std::optional<std::shared_ptr<hf::Cell>> cc) {
  if (!ImGui::Begin("Current cell info")) {
    ImGui::End();
    return;
  }
  if (!cc) {
    ImGui::Text("No cell here");
    ImGui::End();
    return;
  }
  auto cell = *cc;

  auto engine = entt::service_locator<hf::DrawEngine>::get().lock();
  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  // auto [region, _] = viewport->getRegion(cell->x, cell->y, cell->z);

  ImGui::Text("%s Position: %d.%d.%d\n", ICON_MDI_ARROW_ALL, cell->x, cell->y,
              cell->z);
  auto cti = 0;
  auto n = 0;
  std::vector<std::string> ct_names;
  for (auto ct : hf::Cell::types) {
    ct_names.push_back(ct.name);
    if (ct == cell->type) {
      cti = n;
    }
    n++;
  }
  if (ImGui::Combo("Type##cell_type", &cti, ct_names)) {
    auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
    auto viewport = entt::service_locator<hf::Viewport>::get().lock();
    // mapUtils::updateCell(cell, hf::Cell::types[cti], cell->features);
    engine->tilesCache.clear();
    emitter->publish<hf::redraw_event>();
  }
  ImGui::Text("PassThrough: %s\n", cell->passThrough
                                       ? ICON_MDI_CHECKBOX_MARKED_OUTLINE
                                       : ICON_MDI_CHECKBOX_BLANK_OUTLINE);
  ImGui::Text("SeeThrough: %s\n", cell->seeThrough
                                      ? ICON_MDI_CHECKBOX_MARKED_OUTLINE
                                      : ICON_MDI_CHECKBOX_BLANK_OUTLINE);
  ImGui::Text("Features: %lu\n", cell->features.size());

  // add feature selector with magic_enum
  std::map<CellFeature, std::string> features = {
      {CellFeature::BLOOD, "blood"},
      {CellFeature::CAVE, "cave"},
      {CellFeature::FROST, "frost"},
      {CellFeature::ACID, "acid"},
      {CellFeature::CORRUPT, "corrupt"}};

  for (auto f : cell->features) {
    ImGui::BulletText("%s", features[f].c_str());
  }
  // if (region) {
  //   auto objects = (*region)->location->getObjects(cell);
  //   drawObjects(objects);
  // }

  auto ents = gm->registry->view<hf::position>();

  if (ents.size() > 0) {
    if (ImGui::TreeNode("ents", "%s Entities", ICON_MDI_CUBE_OUTLINE)) {
      for (auto e : ents) {
        auto p = gm->registry->get<hf::position>(e);
        if (cell->x == p.x && cell->y == p.y) {
          drawEntityInfo(e, gm->registry);
        }
      }
      ImGui::TreePop();
    }
  }

  ImGui::End();
}

void Editor::drawTilesetWindow() {
  if (!ImGui::Begin("Tileset")) {
    ImGui::End();
    return;
  }
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
  auto engine = entt::service_locator<hf::DrawEngine>::get().lock();

  if (ImGui::Combo("Tileset", &ts_idx, ts)) {
    auto path = PATH / fs::path("tilesets") / ts[ts_idx];
    viewport->loadTileset(path);
    emitter->publish<hf::resize_event>();
  }
  ImGui::BulletText("Size: %dx%d; gap: %d\n", viewport->tileSet.size.first,
                    viewport->tileSet.size.second, viewport->tileSet.gap);
  ImGui::BulletText("Maps: %lu\n", viewport->tileSet.maps.size());

  if (ImGui::Button("Apply")) {
    emitter->publish<hf::regen_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button("Reload")) {
    auto path = PATH / fs::path("tilesets") / ts[ts_idx];
    viewport->loadTileset(path);
    emitter->publish<hf::regen_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    saveTileset();
  }
  ImGui::Separator();

  viewport->colors.erase("");
  if (ImGui::TreeNode("Colors")) {
    for (auto &el : viewport->colors.items()) {
      if (ImGui::TreeNode(el.key().c_str())) {
        std::vector<std::string> to_remove;
        for (auto &e : el.value().items()) {
          auto color = Color::fromHexString(e.value());
          float col[4] = {
              color.r / 255.f,
              color.g / 255.f,
              color.b / 255.f,
              color.a / 255.f,
          };
          ImGui::Button(e.key().c_str());
          ImGui::SameLine(220);

          ImGui::SetNextItemWidth(150);
          auto cs = e.value().get<std::string>();
          if (ImGui::InputText(fmt::format("##{}", e.key()).c_str(), cs)) {
            viewport->colors[el.key()][e.key()] = cs;
            emitter->publish<hf::regen_event>();
          }
          ImGui::SameLine();
          if (ImGui::ColorEdit4(e.key().c_str(), col,
                                ImGuiColorEditFlags_NoInputs |
                                    ImGuiColorEditFlags_NoLabel |
                                    ImGuiColorEditFlags_AlphaPreview |
                                    ImGuiColorEditFlags_AlphaBar)) {
            auto c =
                Color(col[0] * 255, col[1] * 255, col[2] * 255, col[3] * 255);
            viewport->colors[el.key()][e.key()] = c.hexA();
            emitter->publish<hf::regen_event>();
          }
          ImGui::SameLine();
          if (ImGui::Button(
                  fmt::format("{}##{}", ICON_MDI_DELETE, e.key()).c_str())) {
            to_remove.push_back(e.key());
          }
        }
        for (auto rk : to_remove) {
          viewport->colors[el.key()].erase(rk);
        }
        if (to_remove.size() > 0) {
          emitter->publish<hf::regen_event>();
        }
        auto new_key =
            fmt::format("NEW_COLOR_{}", viewport->colors[el.key()].size());
        ImGui::InputText("key", new_key);
        ImGui::SameLine();
        if (ImGui::Button(fmt::format("Add##{}", el.key()).c_str())) {
          viewport->colors[el.key()][new_key] = "#eeeeeeff";
        }
        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }

  viewport->tileSet.sprites.erase("");
  if (ImGui::TreeNode("Sprites")) {
    std::vector<std::string> to_remove;
    for (auto [k, v] : viewport->tileSet.sprites) {
      sf::Sprite s;
      s.setTexture(viewport->tilesTextures[v[0]]);
      s.setTextureRect(viewport->getTileRect(v[1], v[2]));
      ImGui::Image(s,
                   sf::Vector2f(viewport->tileSet.size.first,
                                viewport->tileSet.size.second) *
                       GUI_SCALE,
                   sf::Color::White, sf::Color::Transparent);
      ImGui::SameLine();
      ImGui::Button(k.c_str());
      ImGui::SameLine(220);
      ImGui::SetNextItemWidth(80);
      if (ImGui::InputInt(fmt::format("##{}{}", k, 0).c_str(),
                          &(viewport->tileSet.sprites[k][0]))) {
        engine->tilesCache.clear();
        engine->invalidate();
      }
      ImGui::SameLine();
      ImGui::SetNextItemWidth(80);
      if (ImGui::InputInt(fmt::format("##{}{}", k, 1).c_str(),
                          &(viewport->tileSet.sprites[k][1]))) {
        engine->tilesCache.clear();
        engine->invalidate();
      }
      ImGui::SameLine();
      ImGui::SetNextItemWidth(80);
      if (ImGui::InputInt(fmt::format("##{}{}", k, 2).c_str(),
                          &(viewport->tileSet.sprites[k][2]))) {
        engine->tilesCache.clear();
        engine->invalidate();
      }
      ImGui::SameLine();
      if (ImGui::Button(fmt::format("{}##{}", ICON_MDI_DELETE, k).c_str())) {
        to_remove.push_back(k);
      }
    }
    for (auto rk : to_remove) {
      viewport->tileSet.sprites.erase(rk);
    }
    if (to_remove.size() > 0) {
      emitter->publish<hf::regen_event>();
    }

    auto new_key =
        fmt::format("NEW_SPRITE_{}", viewport->tileSet.sprites.size());
    ImGui::InputText("key", new_key);
    ImGui::SameLine();
    if (ImGui::Button("Add")) {
      viewport->tileSet.sprites[new_key] = {0, 0, 0};
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Preview")) {
    auto n = 0;
    for (auto t : viewport->tilesTextures) {
      auto size = t.getSize();
      sf::Sprite s;
      s.setTexture(viewport->tilesTextures[n]);
      ImGui::Text("%s", viewport->tileSet.maps[n].c_str());
      ImGui::Image(s, sf::Vector2f(size.x, size.y), sf::Color::White,
                   sf::Color::Transparent);
      ImGui::Text("\n");
      n++;
    }
    ImGui::TreePop();
  }

  ImGui::End();
}

void Editor::saveTileset() {
  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  auto path = PATH / fs::path("tilesets") / ts[ts_idx];
  std::ofstream o(path / "colors.json");
  o << std::setw(4) << viewport->colors << std::endl;

  json j;
  j["TILEMAPS"] = viewport->tileSet.maps;
  j["SIZE"] = viewport->tileSet.size;
  j["GAP"] = viewport->tileSet.gap;
  j["SPRITES"] = viewport->tileSet.sprites;

  std::ofstream o2(path / "tiles.json");
  o2 << std::setw(4) << j << std::endl;
}

void Editor::duk_exec(const char *code) {
  if (duk_peval_string(duk_ctx, code) != 0) {
    auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
    duk_get_prop_string(duk_ctx, -1, "stack");
    auto err = duk_safe_to_string(duk_ctx, -1);
    log.error(lu::red("DUK"),
              fmt::format("Error running '{}':\n{}", code, err));
    duk_pop(duk_ctx);
    emitter->publish<hf::duk_error>(err);
  }
}

void Editor::dukEditorWindow() {
  if (!ImGui::Begin("Script")) {
    ImGui::End();
    return;
  }

  if (ImGui::Button(ICON_MDI_CONTENT_SAVE)) {
    auto fileToSave = PATH / "init.js";
    std::ofstream o(fileToSave);

    o << std::setw(4) << duk_editor.GetText().data() << std::endl;
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_PLAY)) {
    markers.clear();
    duk_editor.SetErrorMarkers(markers);

    if (jsThread.joinable()) {
      jsThread.join();
    }
    jsThread = std::thread([&]() { duk_exec(duk_editor.GetText().data()); });
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_STOP)) {
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_UNDO)) {
    duk_editor.Undo();
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_REDO)) {
    duk_editor.Redo();
  }

  duk_editor.Render("Init script");
  ImGui::End();
}

void Editor::duk_log(const std::string msg) { duk_log_ev(msg); }

void Editor::drawViewportWindow() {
  if (!ImGui::Begin("Viewport")) {
    ImGui::End();
    return;
  }

  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto engine = entt::service_locator<hf::DrawEngine>::get().lock();
  auto viewport = entt::service_locator<hf::Viewport>::get().lock();
  // log.debug("viewport size: {}x{}", viewport->width, viewport->height);
  // log.debug("viewport pos size: {}.{}.{}", viewport->view_x, viewport->view_y, viewport->view_z);
  // auto cache_full = gm->location->width * gm->location->height;
  // ImGui::Text("Cache len: %d/%d", engine->cache_count, cache_full);
  ImGui::Text("Redraws: %d", engine->redraws);
  ImGui::Text("Tiles updated: %d", engine->tilesUpdated);
  ImGui::Text("Objects in render: %d\n", engine->layers->size());
  for (auto [k, l] : engine->layers->layers) {
    ImGui::AlignTextToFramePadding();
    ImGui::BulletText(fmt::format("{}: {}", k, l->children.size()).c_str());
    ImGui::SameLine();
    if (ImGui::Checkbox(fmt::format("##{}", k).c_str(), &l->enabled)) {
      engine->invalidate();
    }
  }
  ImGui::Text("\n");
  auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
  if (ImGui::SliderInt("width", &viewport->width, 10, 200)) {
    engine->invalidate();
    engine->vW = viewport->width / viewport->scale / GUI_SCALE;
    if (engine->vW > 200)
      engine->vW = 200;
  }
  if (ImGui::SliderInt("height", &viewport->height, 10, 200)) {
    engine->invalidate();
    engine->vH = viewport->height / viewport->scale / GUI_SCALE;
    if (engine->vH > 200)
      engine->vH = 200;
  }
  ImGui::End();
  return;
  if (ImGui::SliderInt("x", &viewport->view_x, -viewport->width,
                       gm->location->width)) {
    engine->invalidate();
  }
  if (ImGui::SliderInt("y", &viewport->view_y, -viewport->height,
                       gm->location->height)) {
    engine->invalidate();
  }
  if (ImGui::SliderInt("z", &viewport->view_z, -10, 10)) {
    engine->invalidate();
  }
  if (ImGui::SliderFloat("scale", &viewport->scale, 0.3f, 2.f)) {
    engine->vW = viewport->width / viewport->scale / GUI_SCALE;
    engine->vH = viewport->height / viewport->scale / GUI_SCALE;
    if (engine->vW > 200)
      engine->vW = 200;
    if (engine->vH > 200)
      engine->vH = 200;
    emitter->publish<hf::resize_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_FORMAT_SIZE)) {
    viewport->scale = 1;
    emitter->publish<hf::resize_event>();
  }

  ImGui::End();
}

void Editor::drawLocationWindow() {
  if (!ImGui::Begin("Location")) {
    ImGui::End();
    return;
  }

  auto engine = entt::service_locator<hf::DrawEngine>::get().lock();
  if (ImGui::InputInt("Seed", &gm->seed)) {

    gm->start();
    engine->invalidate();
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_MDI_DICE_3)) {
    auto emitter = entt::service_locator<hf::event_emitter>::get().lock();
    emitter->publish<hf::regen_event>(-1);
  }
  ImGui::Separator();
  ImGui::End();
}

void Editor::Draw() {

  // editor->drawCellInfo(cc);
  drawLocationWindow();
  drawViewportWindow();
  drawTilesetWindow();
  drawSpecWindow();
  drawObjectsWindow();
  drawSelectedInfo();
  console.Draw("Console");

  dukEditorWindow();
}
