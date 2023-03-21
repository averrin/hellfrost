#include "lss/components.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/content/enemies.hpp"
#include "lss/utils.hpp"
#include <lss/gameManager.hpp>

// TODO: remove it
#include <IconFontCppHeaders/IconsFontAwesome6.h>
#include <app/ui/drawEngine.hpp>
#include <app/ui/viewport.hpp>

GameManager::GameManager(fs::path p) : path(p) {
  loadData();
  generator = std::make_unique<Generator>();
}

void GameManager::gen(LocationSpec spec) {
  srand(seed);
  Random::seed(seed);

  registry->reset();
  auto data = entt::service_locator<GameData>::get().lock();
  // registry
  //     .on_construct<hellfrost::renderable>()
  //     .connect<&entt::registry::assign<hellfrost::position>>(registry);
  if (spec.templateMap.size() == 0) {
    spec.templateMap = data->mapFeatures[spec.getType()];
  }

  generator->templates = templates;
  generator->features = features;
  location = generator->getLocation(spec);
  registry = location->registry;
  entt::service_locator<entt::registry>::set(registry);
  auto view_map = entt::service_locator<Viewport>::get().lock();

  std::map<std::shared_ptr<Room>, entt::entity> res;
  for (auto r : location->rooms) {
    auto e = registry->create();
    registry->assign<hellfrost::ingame>(e);
    registry->assign<hellfrost::room>(e, r);
    registry->assign<hellfrost::size>(e, r->width, r->height);
    registry->assign<hellfrost::children>(e);
    registry->assign<hellfrost::renderable>(e, "EMPTY_CELL");
    auto i = r->getInfo();
    registry->assign<hellfrost::meta>(e, i, "", i);
    registry->assign<hellfrost::position>(e, r->x, r->y, 0, false);
    registry->assign<hellfrost::ineditor>(e, r->getInfo(),
                                          std::vector<std::string>{"Rooms"},
                                          ICON_FA_VECTOR_SQUARE);
    res[r] = e;
  }
  for (auto r : location->cells) {
    for (auto cell : r) {
      auto e = registry->create();
      registry->assign<hellfrost::ingame>(e);
      registry->assign<hellfrost::cell>(e, cell);
      auto i = cell->getSId();
      registry->assign<hellfrost::meta>(e, i, "", i);
      registry->assign<hellfrost::renderable>(e, "EMPTY_CELL");
      registry->assign<hellfrost::position>(e, cell->x, cell->y, cell->z,
                                            false);
      registry->assign<hellfrost::ineditor>(
          e, cell->getSId(), std::vector<std::string>{"Cells"}, ICON_FA_CUBE);

      for (auto room : location->rooms) {
        for (auto c : room->cells) {
          if (c == cell) {
            auto re = res[room];
            auto ch = registry->get<hf::children>(re);
            ch.children.push_back(e);
            registry->assign_or_replace<hf::children>(re, ch);
          }
        }
      }
    }
  }

  // TODO: nest entities in their rooms
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
        std::vector<std::string>{"Items", i->type.name}, ICON_FA_CARROT);
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
        ICON_FA_DRAGON);

    auto key = i->type.name;
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    std::replace(key.begin(), key.end(), ' ', '_');
    registry->assign<hellfrost::visible>(e, "ENEMY", key);
    registry->assign<hellfrost::position>(e, i->currentCell->x,
                                          i->currentCell->y, 0);
  }

  auto ents = registry->group(entt::get<hf::position, hf::meta>);
  for (auto e : ents) {
    if (!registry->valid(e)) {
      continue;
    }
    auto pos = registry->get<hf::position>(e);
    if (!pos.movable)
      continue;

    auto c = location->cells[pos.y][pos.x];
    if (c->hasFeature(CellFeature::WIPE) || c->type == CellType::UNKNOWN ||
        c->type == CellType::WATER || c->type == CellType::WALL ||
        c->type == CellType::UPSTAIRS || c->type == CellType::DOWNSTAIRS) {
      // TODO: allow lights
      registry->destroy(e);
    }
  }

  return;
  // EsToProto(EnemyType::CULTIST, data, 99);
  // auto n = 0;
  // for (auto es : EnemyType::ALL) {
  //   n++;
  // }
  return;

  // data->prototypes->reset();
  // auto n = 0;
  // for (auto c : Prototype::ALL) {
  //   for (auto item : c) {
  //     auto type = data->itemSpecs[item->typeKey];
  //     item->type = type;

  //     item->identified = type.identified;
  //     item->durability = type.durability;
  //     item->unidName = item->unidName;
  //     item->name = item->name != "" ? item->name : type.name;

  //     auto e = data->prototypes->create();
  //     data->prototypes->assign<entt::tag<"proto"_hs>>(e);
  //     auto id = fmt::format("{}_{}", item->type.name, n);
  //     data->prototypes->assign<hf::meta>(e, item->getFullTitle(true), "",
  //     id); data->prototypes->assign<hf::ineditor>(
  //         e, id, std::vector<std::string>{"Items", item->type.name},
  //         u8"\uF4E5");
  //     data->prototypes->assign<hf::visible>(e, "ITEMS", item->type.sign);

  //     data->prototypes->assign<hf::pickable>(e, item->type.category,
  //                                            item->type.identified,
  //                                            item->count, item->unidName);
  //     data->prototypes->assign<hf::wearable>(e, item->type.wearableType,
  //                                            item->type.durability);
  //     n++;
  //   }
  // }

  // for (auto [sid, spec] : data->terrainSpecs) {
  //   auto e = data->prototypes->create();
  //   data->prototypes->assign<entt::tag<"proto"_hs>>(e);
  //   data->prototypes->assign<entt::tag<"terrain"_hs>>(e);

  //   data->prototypes->assign<hf::meta>(e, sid, "", sid);
  //   data->prototypes->assign<hf::ineditor>(
  //       e, sid, std::vector<std::string>{"Terrain", spec.name}, u8"\uF531");
  //   data->prototypes->assign<hf::visible>(e, "TERRAIN", spec.sign);
  //   if (spec.light.distance > 0) {
  //     data->prototypes->assign<hf::glow>(e, spec.light.distance,
  //                                        spec.light.type, spec.light.stable);
  //   }
  //   n++;
  // }
}

void GameManager::EsToProto(EnemySpec es, std::shared_ptr<GameData> data,
                            int n) {
  auto key = es.name;
  std::transform(key.begin(), key.end(), key.begin(), ::toupper);
  std::replace(key.begin(), key.end(), ' ', '_');

  auto e = data->prototypes->create();
  data->prototypes->assign<entt::tag<"proto"_hs>>(e);
  auto id = fmt::format("{}_{}", es.name, n);
  data->prototypes->assign<hf::meta>(e, es.name, "", id);
  data->prototypes->assign<hf::ineditor>(
      e, id, std::vector<std::string>{"Enemies", es.name}, ICON_FA_DRAGON);
  data->prototypes->assign<hf::visible>(e, "ENEMY", key);
}

std::shared_ptr<EntityWrapper>
GameManager::createInLua(std::shared_ptr<Location> location,
                         const std::string id, const int x, const int y,
                         const int z) {
  auto registry = location->registry;
  auto mutex = entt::service_locator<std::mutex>::get().lock();
  mutex->lock();
  auto data = entt::service_locator<GameData>::get().lock();
  entt::entity e;
  data->prototypes->each([&](auto entity) {
    auto meta = data->prototypes->get<hf::meta>(entity);
    if (meta.id == id) {
      e = entity;
    }
  });

  auto ne = registry->create();
  registry->assign<hf::ingame>(ne);
  auto meta = data->prototypes->get<hf::meta>(e);
  meta.id = fmt::format("{}_{}", meta.id, (int)registry->entity(ne));
  registry->stomp(ne, e, *data->prototypes, entt::exclude<entt::tag<"proto"_hs>>
                  // , hf::meta, hf::position>
  );
  registry->assign<hf::position>(ne, x, y, z);
  registry->assign_or_replace<hf::meta>(ne, meta);
  mutex->unlock();
  auto ew = std::make_shared<EntityWrapper>(ne);
  ew->registry = registry;
  return ew;
}
