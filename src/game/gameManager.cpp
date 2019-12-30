#include "lss/components.hpp"
#include "lss/utils.hpp"
#include <lss/gameManager.hpp>

// TODO: remove it
#include <app/fonts/material_design_icons.h>
#include <app/ui/drawEngine.hpp>
#include <app/ui/viewport.hpp>

GameManager::GameManager(fs::path p) : path(p) {
  loadData();
  generator = std::make_unique<Generator>();
}

void GameManager::gen(LocationSpec spec) {
  srand(seed);

  registry->reset();
  // registry
  //     .on_construct<hellfrost::renderable>()
  //     .connect<&entt::registry::assign<hellfrost::position>>(registry);

  location = generator->getLocation(spec);
  registry = location->registry;
  entt::service_locator<entt::registry>::set(registry);
  auto view_map = entt::service_locator<Viewport>::get().lock();

  auto data = entt::service_locator<GameData>::get().lock();

  for (auto i : utils::castObjects<Item>(location->objects)) {
    auto e = registry->create();
    registry->assign<hellfrost::ingame>(e);
    auto type = data->itemSpecs[i->typeKey];
    i->type = type;

    i->identified = type.identified;
    i->durability = type.durability;
    i->unidName = type.name;
    i->name = type.name;

    registry->assign<hellfrost::meta>(e, i->getTitle());
    registry->assign<hellfrost::ineditor>(
        e, i->getFullTitle(true),
        std::vector<std::string>{"Items", i->type.name}, ICON_MDI_SWORD);
    registry->assign<hellfrost::visible>(e, "ITEMS", i->type.sign);
    registry->assign<hellfrost::position>(e, i->currentCell->x,
                                          i->currentCell->y, 0);
  }

  for (auto i : utils::castObjects<Enemy>(location->objects)) {
    auto e = registry->create();
    registry->assign<hellfrost::ingame>(e);
    registry->assign<hellfrost::meta>(e, i->type.name);

    registry->assign<hellfrost::ineditor>(
        e, i->type.name, std::vector<std::string>{"Enemies", i->type.name},
        ICON_MDI_SWORD_CROSS);

    auto key = i->type.name;
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    std::replace(key.begin(), key.end(), ' ', '_');
    registry->assign<hellfrost::visible>(e, "ENEMY", key);
    registry->assign<hellfrost::position>(e, i->currentCell->x,
                                          i->currentCell->y, 0);
  }

  return;

  data->prototypes->reset();
  auto n = 0;
  for (auto c : Prototype::ALL) {
    for (auto item : c) {
      auto type = data->itemSpecs[item->typeKey];
      item->type = type;

      item->identified = type.identified;
      item->durability = type.durability;
      item->unidName = item->unidName;
      item->name = item->name != "" ? item->name : type.name;

      auto e = data->prototypes->create();
      data->prototypes->assign<entt::tag<"proto"_hs>>(e);
      auto id = fmt::format("{}_{}", item->type.name, n);
      data->prototypes->assign<hf::meta>(e, item->getFullTitle(true), "", id);
      data->prototypes->assign<hf::ineditor>(
          e, id, std::vector<std::string>{"Items", item->type.name},
          u8"\uF4E5");
      data->prototypes->assign<hf::visible>(e, "ITEMS", item->type.sign);

      data->prototypes->assign<hf::pickable>(e, item->type.category,
                                             item->type.identified, item->count,
                                             item->unidName);
      data->prototypes->assign<hf::wearable>(e, item->type.wearableType,
                                             item->type.durability);
      n++;
    }
  }

  for (auto [sid, spec] : data->terrainSpecs) {
    auto e = data->prototypes->create();
    data->prototypes->assign<entt::tag<"proto"_hs>>(e);
    data->prototypes->assign<entt::tag<"terrain"_hs>>(e);

    data->prototypes->assign<hf::meta>(e, sid, "", sid);
    data->prototypes->assign<hf::ineditor>(
        e, sid, std::vector<std::string>{"Terrain", spec.name}, u8"\uF531");
    data->prototypes->assign<hf::visible>(e, "TERRAIN", spec.sign);
    if (spec.light.distance > 0) {
      data->prototypes->assign<hf::glow>(e, spec.light.distance,
                                         spec.light.type, spec.light.stable);
    }
    n++;
  }
}
