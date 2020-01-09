#include <hellfrost/game/components.hpp>
#include <hellfrost/game/gameManager.hpp>

// TODO: remove it
#include <fonts/material_design_icons.h>
// #include <hellfrost/ui/viewport.hpp>

namespace hellfrost {
GameManager::GameManager(fs::path p, int s) : path(p), seed(s) {
  log.setParent(&LibLog::Logger::getInstance());
  loadData();
  generator = std::make_unique<Generator>();
}

void GameManager::start() {
  start(seed);
}
void GameManager::start(int s) {
  status = gm::status::GENERATING;
  auto eventLog = entt::service_locator<LibLog::Logger>::get().lock();
  eventLog->setParent(&log);
  auto label = "GM start";
  log.start(label, true);

  seed = s;
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  srand(seed);
  registry->reset();

  //TODO: make thread here
  location = generator->getLocation();
  // registry = location->registry;
  entt::service_locator<entt::registry>::set(registry);
  emitter->publish<gm::generation_start>(location);

  // generator->log.setParent(&log);
  generator->log.setAsync(true);
  if (genThread.joinable()) {
    genThread.join();
  }
  genThread = std::thread([this]() {
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    generator->start();
    emitter->publish<gm::generation_finish>();
    // generator->log.setParent(&LibLog::Logger::getInstance());
    generator->log.setAsync(false);
    status = gm::status::DONE;
  });


  eventLog->setParent(&LibLog::Logger::getInstance());
  log.stop(label);
  return;

  /*
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

  // return;

  data->prototypes->reset();
  auto n = 0;
  std::map<std::string, int> ids;
  for (auto c : Prototype::ALL) {
    for (auto item : c) {
      if (ids.find(item->typeKey) == ids.end()) {
        ids[item->typeKey] = 0;
      }

      auto type = data->itemSpecs[item->typeKey];
      item->type = type;

      item->identified = type.identified;
      item->durability = type.durability;
      item->unidName = item->unidName;
      item->name = item->name != "" ? item->name : type.name;

      auto e = data->prototypes->create();
      data->prototypes->assign<entt::tag<"proto"_hs>>(e);
      auto id = fmt::format("{}_{}", item->type.name, ids[item->typeKey]);
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
      ids[item->typeKey]++;
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
*/
}
}
