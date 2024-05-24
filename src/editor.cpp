#include "lss/game/cell.hpp"
#include "lss/generator/room.hpp"
#include <IconFontCppHeaders/IconsFontAwesome6.h>
#include <SFML/Graphics.hpp>
#include <app/editor.hpp>
#include <app/ui/viewport.hpp>
#include <imgui-stl.hpp>
#include <lss/deps.hpp>
#include <lss/game/light.hpp>
#include <lss/game/wearableType.hpp>
#include <lss/generator/mapUtils.hpp>
#include <magic_enum.hpp>

void Editor::drawEntityEditor(std::shared_ptr<entt::registry> registry) {
  entityEditor.registerTrivial<hf::meta>(*registry, "Meta");
  entityEditor.registerTrivial<hf::position>(*registry, "Position");
  entityEditor.registerTrivial<hf::visible>(*registry, "Visible");
  entityEditor.registerTrivial<hf::renderable>(*registry, "Renderable");
  entityEditor.registerTrivial<hf::ineditor>(*registry, "Editor");
  entityEditor.registerTrivial<hf::pickable>(*registry, "Pickable");
  entityEditor.registerTrivial<hf::wearable>(*registry, "Wearable");
  entityEditor.registerTrivial<hf::glow>(*registry, "Glow");
  entityEditor.registerTrivial<hf::cell>(*registry, "Cell");
  entityEditor.registerTrivial<hf::room>(*registry, "Room");
  entityEditor.registerTrivial<hf::children>(*registry, "Children");
  entityEditor.registerTrivial<hf::size>(*registry, "Size");
  entityEditor.registerTrivial<hf::wall>(*registry, "Wall");
  entityEditor.registerTrivial<hf::tags>(*registry, "Tags");
  entityEditor.registerTrivial<hf::overwrite>(*registry, "Overwrite");

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
      registry->type<hf::size>(),
      [&](entt::registry &reg, auto e) { Size(registry, e); });

  entityEditor.registerComponentWidgetFn(
      registry->type<hf::pickable>(),
      [&](entt::registry &reg, auto e) { Pickable(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::wearable>(),
      [&](entt::registry &reg, auto e) { Wearable(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::glow>(),
      [&](entt::registry &reg, auto e) { Glow(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::cell>(),
      [&](entt::registry &reg, auto e) { CellW(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::room>(),
      [&](entt::registry &reg, auto e) { RoomW(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::children>(),
      [&](entt::registry &reg, auto e) { Children(registry, e); });
  entityEditor.registerComponentWidgetFn(
      registry->type<hf::tags>(),
      [&](entt::registry &reg, auto e) { Tags(registry, e); });

  entityEditor.registerToolbarFn([&](entt::registry &reg, auto e) {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    if (reg.has<entt::tag<"proto"_hs>>(e)) {
      auto data = entt::service_locator<GameData>::get().lock();
      if (ImGui::Button(ICON_FA_PLUS)) {
        auto viewport = entt::service_locator<Viewport>::get().lock();
        auto location = viewport->regions.front()->location;
        auto meta = reg.get<hf::meta>(e);
        if (selectedCell) {
          auto cell = *selectedCell;
          location->addEntity(meta.id, cell);
          emitter->publish<center_event>(cell->x, cell->y);
        } else {
          location->addEntity(meta.id, location->cells[0][0]);
        }
        emitter->publish<redraw_event>();
      }
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_CLONE)) {
        auto data = entt::service_locator<GameData>::get().lock();
        auto ne = reg.create();
        reg.stomp(ne, e, *data->prototypes);
        auto meta = reg.get<hf::meta>(ne);
        meta.id = meta.id + "_copy";
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

      if (ImGui::Button(ICON_FA_ARROWS_TO_DOT)) {
        emitter->publish<center_event>(p.x, p.y);
      }
      ImGui::SameLine();
    }
    if (reg.has<hf::ineditor>(e)) {
      auto &ie = reg.get<hf::ineditor>(e);

      if (ImGui::Button(ICON_FA_OBJECT_GROUP)) {
        ie.selected = !ie.selected;
        reg.assign_or_replace<hellfrost::ineditor>(e, ie);
      }
      ImGui::SameLine();
    }
    if (ImGui::Button(ICON_FA_TRASH)) {
      if (reg.has<hf::renderable>(e)) {
        // auto &p = reg.get<hf::position>(e);
        reg.destroy(e);
        // FIXME: use damage_event
        emitter->publish<redraw_event>();
      } else {
        reg.destroy(e);
      }
    }
  });
}

Editor::Editor(std::shared_ptr<GameManager> _gm, fs::path _path)
    : gm(_gm), PATH(_path) {

  auto n = 0;
  char *DEFAULT_TILESET = entt::monostate<"tileset"_hs>{};
  for (auto entity : fs::directory_iterator(PATH / "tilesets")) {
    if (fs::is_directory(entity)) {
      auto tileset = entity.path().filename().string();
      if (tileset == DEFAULT_TILESET) { // TODO: add config
        ts_idx = n;
      }
      ts.push_back(tileset);
      n++;
    }
  }
}

void Editor::processRegistry(std::shared_ptr<entt::registry>) {}

void Editor::Tags(std::shared_ptr<entt::registry> registry, entt::entity e) {
  auto &t = registry->get<hf::tags>(e);
  if (ImGui::ListEdit("Tags", t.tags)) {
    registry->assign_or_replace<hf::tags>(e, t);
  }
}

void Editor::Size(std::shared_ptr<entt::registry> registry, entt::entity e) {
  auto &s = registry->get<hf::size>(e);
  if (ImGui::InputInt("Width", &s.width)) {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    emitter->publish<redraw_event>();
  }
  if (ImGui::InputInt("Height", &s.height)) {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    emitter->publish<redraw_event>();
  }
}

void Editor::RoomW(std::shared_ptr<entt::registry> registry, entt::entity e) {
  auto &r = registry->get<hf::room>(e);
  ImGui::Text(
      fmt::format("Type: {}", magic_enum::enum_name(r.room->type)).c_str());
}
void Editor::Children(std::shared_ptr<entt::registry> registry,
                      entt::entity e) {
  if (ImGui::TreeNode(
          fmt::format("childs##{}", (int)registry->entity(e)).c_str(),
          "%s Entities", ICON_FA_CUBE)) {
    auto ch = registry->get<hf::children>(e);
    for (auto child : ch.children) {
      drawEntityInfo(child, registry);
    }
    ImGui::TreePop();
  }
}

void Editor::CellW(std::shared_ptr<entt::registry> registry, entt::entity e) {
  auto &p = registry->get<hf::cell>(e);
  auto cell = p.cell;

  auto cti = 0;
  auto n = 0;
  std::vector<std::string> ct_names;
  for (auto ct : Cell::types) {
    ct_names.push_back(ct.name);
    if (cell != nullptr && ct == cell->type) {
      cti = n;
    }
    n++;
  }
  if (ImGui::Combo("Type##cell_type", &cti, ct_names)) {
    if (cell != nullptr) {
      auto emitter = entt::service_locator<event_emitter>::get().lock();
      auto viewport = entt::service_locator<Viewport>::get().lock();
      auto engine = entt::service_locator<DrawEngine>::get().lock();
      mapUtils::updateCell(cell, Cell::types[cti], cell->tags.tags);
      engine->tilesCache.clear();
      emitter->publish<redraw_event>();
    }
  }
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
  if (ImGui::Combo("Light type", &gt, names)) {
    g.type = types[gt];
  }
  ImGui::InputInt("Brightness", &g.bright);
  ImGui::InputInt("Flick", &g.flick);
  ImGui::InputInt("Pulse", &g.pulse);
  ImGui::Checkbox("Passive", &g.passive);
}

void Editor::InEditor(std::shared_ptr<entt::registry> registry,
                      entt::entity e) {
  auto &ie = registry->get<hf::ineditor>(e);
  if (registry->has<hf::ineditor>(e)) {
    ie = registry->get<hf::ineditor>(e);
  } else {
    registry->assign_or_replace<hf::ineditor>(e, hf::ineditor{});
    ie = registry->get<hf::ineditor>(e);
  }

  ImGui::BeginDisabled();
  ImGui::InputText("Name", ie.name);
  ImGui::EndDisabled();
  if (ImGui::Button(ICON_FA_FLOPPY_DISK)) {
    ie.name = new_editor_name;
  }
  ImGui::SameLine();
  ImGui::InputText("New Name", new_editor_name);
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
  if (ImGui::ListEdit("Folders", ie.folders)) {
    registry->assign_or_replace<hf::ineditor>(e, ie);
  }
  if (ie.icon != "") {
    ImGui::BulletText(fmt::format("Icon: {}", ie.icon).c_str());
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
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  auto &p = registry->get<hf::position>(e);
  if (ImGui::Button(ICON_FA_ARROWS_TO_DOT)) {
    emitter->publish<center_event>(p.x, p.y);
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
  auto viewport = entt::service_locator<Viewport>::get().lock();
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
  if (ImGui::InputText("Color", r.fgColor)) {
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  if (r.fgColor.length() > 1 && r.fgColor[0] == '#') {
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
  }

  if (ImGui::Checkbox("Has BG", &r.hasBg)) {
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  if (r.hasBg) {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("BG Color", r.bgColor)) {
      registry->assign_or_replace<hf::renderable>(e, r);
    }
    if (r.bgColor.length() > 1 && r.bgColor[0] == '#') {
      ImGui::SameLine();
      auto color = Color::fromHexString(r.bgColor);
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
  }

  if (ImGui::Checkbox("Has border", &r.hasBorder)) {
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  if (r.hasBorder) {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("Border Color", r.borderColor)) {
      registry->assign_or_replace<hf::renderable>(e, r);
    }
    if (r.borderColor.length() > 1 && r.borderColor[0] == '#') {
      ImGui::SameLine();
      auto color = Color::fromHexString(r.borderColor);
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

  std::vector<const char *> layers = {"cellsBg",     "cells", "cellsBrd",    "entitiesBg",
                                      "entities", "entitiesBrd", "light",
                                      "overlay",     "debug"};
  int fgl_idx = std::distance(
      layers.begin(), std::find(layers.begin(), layers.end(), r.fgLayer));
  int bgl_idx = std::distance(
      layers.begin(), std::find(layers.begin(), layers.end(), r.bgLayer));
  int brl_idx = std::distance(
      layers.begin(), std::find(layers.begin(), layers.end(), r.brdLayer));
  if (ImGui::Combo("Layer", &fgl_idx, layers.data(), layers.size())) {
    r.fgLayer = std::string(layers[fgl_idx]);
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  if (ImGui::Combo("BG Layer", &bgl_idx, layers.data(), layers.size())) {
    r.bgLayer = std::string(layers[bgl_idx]);
    registry->assign_or_replace<hf::renderable>(e, r);
  }
  if (ImGui::Combo("Border Layer", &brl_idx, layers.data(), layers.size())) {
    r.brdLayer = std::string(layers[brl_idx]);
    registry->assign_or_replace<hf::renderable>(e, r);
  }
}

void Editor::drawSpecWindow() {
  if (!ImGui::Begin("Specification")) {
    ImGui::End();
    return;
  }
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto engine = entt::service_locator<DrawEngine>::get().lock();
  auto data = entt::service_locator<GameData>::get().lock();

  if (ImGui::Button("Apply")) {
    emitter->publish<regen_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button("Reload")) {
    gm->loadData();
    emitter->publish<resize_event>();
    emitter->publish<regen_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    gm->saveData();
  }
  ImGui::Separator();

  if (ImGui::CollapsingHeader("Location Features")) {
    for (auto &[k, _] : data->mapFeatures) {
      if (ImGui::CollapsingHeader(k.c_str())) {
        ImGui::Indent();
        for (auto &[f, _] : data->mapFeatures[k]) {
          ImGui::SetNextItemWidth(80);
          ImGui::InputFloat(f.c_str(), &data->mapFeatures[k][f]);
        }

        ImGui::SetNextItemWidth(80);
        ImGui::InputFloat(fmt::format("##New prob lf {}", k).c_str(),
                          &new_prob);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(160);
        ImGui::InputText(fmt::format("##New name lf {}", k).c_str(),
                         new_key_lf);
        ImGui::SameLine();
        if (ImGui::Button(fmt::format("Add##{}", k).c_str())) {
          data->mapFeatures[k][new_key_lf] = new_prob;
        }
        ImGui::Unindent();
      }
    }
    ImGui::SetNextItemWidth(160);
    ImGui::InputText("##New name lt", new_key_lt);
    ImGui::SameLine();
    if (ImGui::Button("Add")) {
      data->mapFeatures[new_key_lt] = {};
    }
  }
  if (ImGui::CollapsingHeader("Probabilities")) {
    for (auto &[k, _] : data->probability) {
      ImGui::Indent();
      ImGui::SetNextItemWidth(80);
      ImGui::InputFloat(k.c_str(), &data->probability[k]);
      ImGui::Unindent();
    }
    ImGui::Indent();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat("##New prob", &new_prob);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160);
    ImGui::InputText("##New name", new_key_prob);
    ImGui::SameLine();
    if (ImGui::Button("Add")) {
      data->probability[new_key_prob] = new_prob;
    }
    ImGui::Unindent();
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

  if (ImGui::TreeNode("ents", "%s Entities", ICON_FA_CUBE)) {
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
        if (ImGui::TreeNode(fmt::format("{} {}", ICON_FA_FOLDER, fn).c_str())) {
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
  auto viewport = entt::service_locator<Viewport>::get().lock();
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
                  : ICON_FA_CUBE;
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

void Editor::drawCellInfo(std::optional<std::shared_ptr<Cell>> cc) {
  selectedCell = cc;
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

  auto engine = entt::service_locator<DrawEngine>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto [region, _] = viewport->getRegion(cell->x, cell->y, cell->z);

  ImGui::Text("%s Position: %d.%d.%d\n", ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT,
              cell->x, cell->y, cell->z);
  ImGui::Text("%s Position local: %d.%d.%d\n",
              ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, cell->_x, cell->_y, cell->_z);
  auto cti = 0;
  auto n = 0;
  std::vector<std::string> ct_names;
  for (auto ct : Cell::types) {
    ct_names.push_back(ct.name);
    if (ct == cell->type) {
      cti = n;
    }
    n++;
  }
  if (ImGui::Combo("Type##cell_type", &cti, ct_names)) {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    auto viewport = entt::service_locator<Viewport>::get().lock();
    mapUtils::updateCell(cell, Cell::types[cti], cell->tags.tags);
    engine->tilesCache.clear();
    emitter->publish<redraw_event>();
  }
  ImGui::Text("Illuminated: %s\n",
              cell->illuminated ? ICON_FA_SQUARE_CHECK : ICON_FA_SQUARE);
  ImGui::Text("PassThrough: %s\n",
              cell->passThrough ? ICON_FA_SQUARE_CHECK : ICON_FA_SQUARE);
  ImGui::Text("SeeThrough: %s\n",
              cell->seeThrough ? ICON_FA_SQUARE_CHECK : ICON_FA_SQUARE);

  for (auto f : cell->tags.tags) {
    ImGui::BulletText("%s", f.c_str());
  }

  auto ents = gm->registry->view<hf::position>();

  if (ents.size() > 0) {
    if (ImGui::TreeNode("ents", "%s Entities", ICON_FA_CUBE)) {
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
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  auto engine = entt::service_locator<DrawEngine>::get().lock();

  if (ImGui::Combo("Tileset", &ts_idx, ts)) {
    auto path = PATH / fs::path("tilesets") / ts[ts_idx];
    viewport->loadTileset(path);
    emitter->publish<regen_event>();
  }
  ImGui::BulletText("Size: %dx%d; gap: %d\n", viewport->tileSet.size.first,
                    viewport->tileSet.size.second, viewport->tileSet.gap);
  ImGui::BulletText("Maps: %lu\n", viewport->tileSet.maps.size());

  if (ImGui::Button("Apply")) {
    emitter->publish<regen_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button("Reload")) {
    auto path = PATH / fs::path("tilesets") / ts[ts_idx];
    viewport->loadTileset(path);
    emitter->publish<regen_event>();
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    saveTileset();
  }
  ImGui::Separator();

  viewport->colors.erase("");
  if (ImGui::TreeNode("Colors")) {
    for (auto &el : viewport->colors.items()) {
      if (el.key() == "VARIATIONS")
        continue;
      if (el.key() == "WANDERING")
        continue;
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
            emitter->publish<regen_event>();
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
            emitter->publish<regen_event>();
          }
          ImGui::SameLine();
          if (ImGui::Button(
                  fmt::format("{}##{}", ICON_FA_TRASH, e.key()).c_str())) {
            to_remove.push_back(e.key());
          }
        }
        for (auto rk : to_remove) {
          viewport->colors[el.key()].erase(rk);
        }
        if (to_remove.size() > 0) {
          emitter->publish<regen_event>();
        }
        // auto new_key =
        // fmt::format("NEW_COLOR_{}", viewport->colors[el.key()].size());
        ImGui::InputText("key", new_key_color);
        ImGui::SameLine();
        if (ImGui::Button(fmt::format("Add##{}", el.key()).c_str())) {
          viewport->colors[el.key()][new_key_color] = "#eeeeeeff";
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
      // s.setOrigin(viewport->tileSet.size.first / 2,
      //             viewport->tileSet.size.second / 2);
      // s.setRotation(90 * v[3]);
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
      // ImGui::SameLine();
      // ImGui::SetNextItemWidth(80);
      // if (ImGui::InputInt(fmt::format("##{}{}", k, 3).c_str(),
      //                     &(viewport->tileSet.sprites[k][3]))) {
      //   engine->tilesCache.clear();
      //   engine->invalidate();
      // }

      ImGui::SameLine();
      if (ImGui::Button(fmt::format("{}##{}", ICON_FA_TRASH, k).c_str())) {
        to_remove.push_back(k);
      }
    }
    for (auto rk : to_remove) {
      viewport->tileSet.sprites.erase(rk);
    }
    if (to_remove.size() > 0) {
      emitter->publish<regen_event>();
    }

    // auto new_key =
    //     fmt::format("NEW_SPRITE_{}", viewport->tileSet.sprites.size());
    ImGui::InputText("key", new_key_sprite);
    ImGui::SameLine();
    if (ImGui::Button("Add")) {
      viewport->tileSet.sprites[new_key_sprite] = {0, 0, 0};
    }
    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Sprite Variations")) {
    for (auto [k, _] : viewport->tileSet.spriteVariations) {
      if (ImGui::CollapsingHeader(k.c_str())) {
        for (auto v : viewport->tileSet.spriteVariations[k]) {
          ImGui::Indent();
          ImGui::Text(v.c_str());
          ImGui::Unindent();
        }
        // TODO: add/del variation
      }
      // TODO: add/del sprite
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Color Variations")) {
    for (auto [k, _] : viewport->tileSet.colorVariations) {
      if (ImGui::CollapsingHeader(k.c_str())) {
        for (auto v : viewport->tileSet.colorVariations[k]) {
          ImGui::Indent();
          ImGui::Text(v.c_str());
          ImGui::Unindent();
        }
        // TODO: add/del variation
      }
      // TODO: add/del sprite
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Color Wandering")) {
    for (auto [k, _] : viewport->tileSet.colorWandering) {
      if (ImGui::CollapsingHeader(k.c_str())) {
        for (auto [v, p] : viewport->tileSet.colorWandering[k]) {
          ImGui::Indent();
          ImGui::Text(v.c_str());
          ImGui::SameLine();
          ImGui::SetNextItemWidth(90 * GUI_SCALE);
          ImGui::InputFloat(fmt::format("##{}{}0", k, v).c_str(),
                            &(viewport->tileSet.colorWandering[k][v][0]));
          ImGui::SameLine();
          ImGui::SetNextItemWidth(90 * GUI_SCALE);
          ImGui::InputFloat(fmt::format("##{}{}1", k, v).c_str(),
                            &(viewport->tileSet.colorWandering[k][v][1]));
          ImGui::Unindent();
        }
        // TODO: add/del variation
      }
      // TODO: add/del sprite
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
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto path = PATH / fs::path("tilesets") / ts[ts_idx];
  std::ofstream o(path / "colors.json");
  viewport->colors["VARIATIONS"] = viewport->tileSet.colorVariations;
  o << std::setw(4) << viewport->colors << std::endl;

  json j;
  j["TILEMAPS"] = viewport->tileSet.maps;
  j["SIZE"] = viewport->tileSet.size;
  j["GAP"] = viewport->tileSet.gap;
  j["SPRITES"] = viewport->tileSet.sprites;
  j["VARIATIONS"] = viewport->tileSet.spriteVariations;

  std::ofstream o2(path / "tiles.json");
  o2 << std::setw(4) << j << std::endl;
}

void Editor::drawLocationWindow(std::shared_ptr<Location> location) {
  auto ents = gm->registry->view<hf::position>();
  auto emitter = entt::service_locator<event_emitter>::get().lock();

  ImGui::Begin("Location info");
  ImGui::Text(fmt::format("Tags [{}]:", location->tags.tags.size()).c_str());
  for (auto tag : location->tags.tags) {
    ImGui::Indent();
    ImGui::BulletText(tag.c_str());
    ImGui::Unindent();
  }
  if (location->enterCell && location->exitCell) {
    ImGui::BulletText("Enter: %s", location->enterCell->getSId().c_str());
    ImGui::SameLine();
    if (ImGui::Button(
            fmt::format("{}##enter", ICON_FA_ARROWS_TO_DOT).c_str())) {
      emitter->publish<center_event>(location->enterCell->x,
                                     location->enterCell->y);
    }
    // ImGui::SameLine();
    ImGui::BulletText("Exit: %s", location->exitCell->getSId().c_str());
    ImGui::SameLine();
    if (ImGui::Button(fmt::format("{}##exit", ICON_FA_ARROWS_TO_DOT).c_str())) {
      emitter->publish<center_event>(location->exitCell->x,
                                     location->exitCell->y);
    }
    ImGui::BulletText("Path: %d", location->path.size());
    ImGui::SameLine();
    if (ImGui::Button(fmt::format("{}##path", ICON_FA_ARROWS_TO_DOT).c_str())) {
      for (auto e : ents) {
        if (!gm->registry->has<hf::cell>(e))
          continue;
        auto c = gm->registry->get<hf::cell>(e);
        for (auto cell : location->path) {
          if (c.cell == cell) {

            auto ie = gm->registry->get<hf::ineditor>(e);
            ie.selected = true;
            gm->registry->assign_or_replace<hellfrost::ineditor>(e, ie);
            emitter->publish<redraw_event>();
          }
        }
      }
    }
  }

  // ImGui::Checkbox("Show unknown", &show_unknown);

  // ImGui::Text("Rooms: %d", location->rooms.size());
  // for (auto room : location->rooms) {
  //   if (ImGui::CollapsingHeader(room->getInfo().c_str())) {
  //     ImGui::Indent();
  //     ImGui::BulletText(
  //         fmt::format("Type: {}",
  //         magic_enum::enum_name(room->type)).c_str());
  //     ImGui::BulletText("Features: %d", room->features.size());
  //     ImGui::BulletText("Entities: %d", room->entities.size());
  //     if (ImGui::CollapsingHeader(
  //             fmt::format("Cells##{}", room->getInfo()).c_str())) {
  //       ImGui::Indent();
  //       for (auto cell : room->cells) {
  //         if (!show_unknown && cell->type == CellType::UNKNOWN)
  //           continue;
  //         std::vector<entt::entity> es;
  //         for (auto e : ents) {
  //           auto p = gm->registry->get<hf::position>(e);
  //           // fmt::print("{}.{} -> {}.{}\n", cell->x, p.x, cell->y, p.y);
  //           if (cell->x == p.x && cell->y == p.y) {
  //             es.push_back(e);
  //           }
  //         }

  //         if (ImGui::CollapsingHeader(
  //                 fmt::format("Place: {}.{}.{} / {}.{}.{} | {} | f:{} |
  //                 e:{}",
  //                             cell->x, cell->y, cell->z, cell->_x, cell->_y,
  //                             cell->_z, cell->type.name,
  //                             cell->features.size(), es.size())
  //                     .c_str())) {
  //           ImGui::Indent();
  //           ImGui::BulletText(fmt::format("Type: {}",
  //           cell->type.name).c_str()); ImGui::BulletText(
  //               fmt::format("Features: {}", cell->features.size()).c_str());

  //           if (es.size() > 0) {
  //             if (ImGui::TreeNode(
  //                     fmt::format("ents##{}.{}.{}", cell->x, cell->y,
  //                     cell->z)
  //                         .c_str(),
  //                     fmt::format("{} Entities", ICON_FA_CUBE).c_str())) {
  //               for (auto e : es) {
  //                 drawEntityInfo(e, gm->registry);
  //               }
  //               ImGui::TreePop();
  //             }
  //           }
  //           ImGui::Unindent();
  //         }
  //       }

  //       ImGui::Unindent();
  //     }
  //     ImGui::Unindent();
  //   }
  // }
  if (location->cells.size() > 0) {
  ImGui::Text("Cells: %d x %d", location->cells.size(),
              location->cells.front().size());
}
  ImGui::End();
}

void Editor::drawTemplates(sol::state &lua) {}
