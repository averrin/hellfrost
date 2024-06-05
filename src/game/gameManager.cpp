#include "lss/components.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/content/enemies.hpp"
#include "lss/utils.hpp"
#include <lss/gameManager.hpp>

// TODO: remove it
#include <IconsFontAwesome6.h>
#include <app/ui/drawEngine.hpp>
#include <app/ui/viewport.hpp>
#include <thread>

void convertVisibleToRenderable(std::shared_ptr<entt::registry> registry,
                                entt::entity e) {
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto render = hf::renderable{};
  if (registry->has<hf::renderable>(e))
    render = registry->get<hf::renderable>(e);
  auto [v, m] = registry->get<hellfrost::visible, hf::meta>(e);
  render.spriteKey = (v.sign != "" && viewport->tileSet.sprites.find(v.sign) !=
                                          viewport->tileSet.sprites.end())
                         ? v.sign
                         : "UNKNOWN";
  render.fgColor = fmt::format("{}/{}", v.type, v.sign);
  registry->assign_or_replace<hellfrost::renderable>(e, render);
  registry->remove<hellfrost::visible>(e);
}

GameManager::GameManager(fs::path p) : path(p) {
  loadData();
  generator = std::make_unique<Generator>();
}

void GameManager::gen(LocationSpec spec) {
  srand(seed);
  Random::seed(seed);
  log.info("Seed: {}", seed);

  registry->reset();
  auto data = entt::service_locator<GameData>::get().lock();
  // registry
  //     .on_construct<hellfrost::renderable>()
  //     .connect<&entt::registry::assign<hellfrost::position>>(registry);
  if (spec.templateMap.size() == 0) {
    spec.templateMap = data->mapFeatures;
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
    registry->assign_or_replace<hellfrost::tags>(e, r->tags.tags);
    registry->assign<hellfrost::size>(e, r->width, r->height);
    registry->assign<hellfrost::children>(e);
    auto rend = hf::renderable{"EMPTY_CELL"};
    rend.brdLayer = "debug";
    registry->assign<hellfrost::renderable>(e, rend);
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
      if (cell->type == CellType::UNKNOWN || cell->type == CellType::EMPTY)
        continue;
      auto e = location->addEntity(cell->type.name, cell);
      registry->assign_or_replace<hellfrost::cell>(e, cell);
      auto i = cell->getSId();
      registry->assign_or_replace<hellfrost::meta>(e, i, "", i);
      auto ie = registry->get<hellfrost::ineditor>(e);
      ie.icon = ICON_FA_CUBE;
      registry->assign_or_replace<hellfrost::ineditor>(e, ie);
      registry->assign_or_replace<hellfrost::tags>(e, cell->tags.tags);
      auto render = hf::renderable{};
      if (registry->has<hellfrost::renderable>(e)) {
        render = registry->get<hellfrost::renderable>(e);
      }
      if (cell->tags.has("BLOOD") && cell->type != CellType::WATER) {
        render.bgColor = "ENV/BLOOD";
        render.hasBg = true;
      };
      if (cell->tags.has("FROST")) {
        render.bgColor = "ENV/FROST";
        render.hasBg = true;
      };
      if (cell->tags.has("CORRUPT")) {
        render.bgColor = "ENV/CORRUPT";
        render.hasBg = true;
      };
      if (cell->type == CellType::WALL) {
        if (cell->tags.has("CAVE"))
          render.fgColor = "ENV/WALL_CAVE";
        if (cell->tags.has("BUILDING"))
          render.fgColor = "ENV/WALL";
      }
      registry->assign_or_replace<hellfrost::renderable>(e, render);
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
  // for (auto i : utils::castObjects<Item>(location->objects)) {
  //   auto e = registry->create();
  //   registry->assign<hellfrost::ingame>(e);
  //   auto type = data->itemSpecs[i->typeKey];
  //   i->type = type;

  //   i->identified = type.identified;
  //   i->durability = type.durability;
  //   i->unidName = type.name;
  //   i->name = type.name;

  //   registry->assign<hellfrost::meta>(e, i->getTitle());
  //   registry->assign<hellfrost::ineditor>(
  //       e, i->getFullTitle(true),
  //       std::vector<std::string>{"Items", i->type.name}, ICON_FA_CARROT);
  //   registry->assign<hellfrost::visible>(e, "ITEMS", i->type.sign);
  //   registry->assign<hellfrost::position>(e, i->currentCell->x,
  //                                         i->currentCell->y, 0);
  // }

  // for (auto i : utils::castObjects<Enemy>(location->objects)) {
  //   auto e = registry->create();
  //   registry->assign<hellfrost::ingame>(e);
  //   registry->assign<hellfrost::meta>(e, i->type.name);

  //   registry->assign<hellfrost::ineditor>(
  //       e, i->type.name, std::vector<std::string>{"Enemies", i->type.name},
  //       ICON_FA_DRAGON);

  //   auto key = i->type.name;
  //   std::transform(key.begin(), key.end(), key.begin(), ::toupper);
  //   std::replace(key.begin(), key.end(), ' ', '_');
  //   registry->assign<hellfrost::visible>(e, "ENEMY", key);
  //   registry->assign<hellfrost::position>(e, i->currentCell->x,
  //                                         i->currentCell->y, 0);
  // }

  auto ents = registry->group(entt::get<hf::position, hf::meta>);
  for (auto e : ents) {
    if (!registry->valid(e) || registry->has<hf::cell>(e)) {
      continue;
    }
    auto pos = registry->get<hf::position>(e);
    if (!pos.movable)
      continue;

    if (pos.x >= location->cells.front().size() ||
        pos.y >= location->cells.size()) {
      registry->destroy(e);
      continue;
    }
    auto c = location->cells[pos.y][pos.x];
    if (c->tags.has("WIPE") || c->type == CellType::UNKNOWN ||
        c->type == CellType::WATER || c->type == CellType::WALL ||
        c->type == CellType::UPSTAIRS || c->type == CellType::DOWNSTAIRS) {
      // TODO: allow lights
      registry->destroy(e);
    }
  }

  for (auto p : data->prototypes->view<hellfrost::visible>()) {
    convertVisibleToRenderable(data->prototypes, p);
  }

  // serve();
  return;
  // EsToProto(EnemyType::CULTIST, data, 99);
  for (auto e : data->prototypes->view<hellfrost::ineditor>()) {
    auto v = data->prototypes->get<hellfrost::ineditor>(e);
    if (v.folders.front() == "Items") {
      log.info(v.name);
      data->prototypes->destroy(e);
    }
  }
  // auto n = 0;
  // for (auto es : EnemyType::ALL) {
  //   EsToProto(es, data, n);
  //   n++;
  // }
  // return;

  // data->prototypes->reset();
  // auto n = 0;
  std::map<std::string, int> counter;
  for (auto c : Prototype::ALL) {
    for (auto item : c) {
      auto type = data->itemSpecs[item->typeKey];
      item->type = type;

      item->identified = type.identified;
      item->durability = type.durability;
      item->unidName = item->unidName;
      item->name = item->name != "" ? item->name : type.name;
      // auto n = 0;
      // if (counter.find(item->unidName) != counter.end()) {
      //   counter[item->unidName] += 1;
      //   n = counter[item->unidName];
      // } else {
      //   counter[item->unidName] = 0;
      // }

      auto e = data->prototypes->create();
      data->prototypes->assign<entt::tag<"proto"_hs>>(e);
      // auto key = fmt::format("{}_{}", item->type.name, n);
      auto key = fmt::format("{}", item->name);
      std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      std::replace(key.begin(), key.end(), ' ', '_');
      data->prototypes->assign<hf::meta>(e, item->getFullTitle(true),
                                         item->unidName, key);
      data->prototypes->assign<hf::ineditor>(
          e, key, std::vector<std::string>{"Items", item->type.name},
          ICON_FA_CARROT);
      data->prototypes->assign<hf::visible>(e, "ITEMS", item->type.sign);

      data->prototypes->assign<hf::pickable>(e, item->type.category,
                                             item->type.identified, item->count,
                                             item->unidName);
      data->prototypes->assign<hf::wearable>(e, item->type.wearableType,
                                             item->type.durability);
      // n++;
    }
  }

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

// TODO: call it on init
void GameManager::serve() {
  // fmt::print("GameManager::serve\n");
  log.start("GameManager::serve", true);
  auto vis = registry->get<hf::vision>(location->player->entity);
  auto d = vis.distance;
  auto fov = location->player->viewField;
  for (auto c : fov) {
    c->seeThrough = c->type.seeThrough;
    c->passThrough = c->type.passThrough;
    auto es = location->getEntities(c);
    for (auto e : es) {
      if (registry->has<hf::obstacle>(e)) {
        auto o = registry->get<hf::obstacle>(e);
        if (!o.seeThrough) {
          c->seeThrough = false;
        }
        if (!o.passThrough) {
          c->passThrough = false;
        }
      }
    }
  }
  // fmt::print("GameManager::before creatures\n");
  for (auto [e, c] : location->creatures) {
    if (registry->valid(e) && registry->has<hf::position>(e) &&
        c->currentCell != nullptr) {
      auto p = registry->get<hf::position>(e);
      p.x = c->currentCell->x;
      p.y = c->currentCell->y;
      registry->assign_or_replace<hellfrost::position>(e, p);
    }
  }
  auto visCacheThread = std::thread([&]() {
    auto vis = registry->get<hf::vision>(location->player->entity);
    auto d = vis.distance;
    for (auto n : location->getNeighbors(location->player->currentCell)) {
      if (!n->passThrough)
        continue;
      location->getVisible(n, d, true);
    }
  });
  visCacheThread.detach();
  // fmt::print("GameManager::after creatures\n");
  log.stop("GameManager::serve");
}

// void GameManager::EsToProto(EnemySpec es, std::shared_ptr<GameData> data,
//                             int n) {
//   auto key = es.name;
//   std::transform(key.begin(), key.end(), key.begin(), ::toupper);
//   std::replace(key.begin(), key.end(), ' ', '_');

//   auto e = data->prototypes->create();
//   data->prototypes->assign<entt::tag<"proto"_hs>>(e);
//   // auto id = fmt::format("{}_{}", es.name, n);
//   data->prototypes->assign<hf::meta>(e, es.name, "", key);
//   data->prototypes->assign<hf::ineditor>(
//       e, key, std::vector<std::string>{"Enemies", es.name}, ICON_FA_DRAGON);
//   data->prototypes->assign<hf::visible>(e, "ENEMY", key);
// }

std::shared_ptr<EntityWrapper>
GameManager::createInLua(std::shared_ptr<Location> location,
                         const std::string id, const int x, const int y,
                         const int z) {
  auto ne =
      location->addEntity(id, std::make_shared<Cell>(x, y, CellType::EMPTY));
  auto ew = std::make_shared<EntityWrapper>(ne);
  ew->registry = registry;
  return ew;
}

bool GameManager::moveCreature(std::shared_ptr<Creature> creature,
                               Direction d) {
  if (creature == nullptr || creature->currentCell == nullptr) {
    return false;
  }
  auto nc = location->getCell(creature->currentCell, d);
  if (!(*nc)->passThrough) {
    return false;
  }
  fmt::print("Move from {} {} to: {} {}\n", creature->currentCell->x,
             creature->currentCell->y, (*nc)->x, (*nc)->y);
  location->invalidateVisibilityCache(creature->currentCell, true);
  location->invalidateEntityCache(creature->currentCell);

  creature->currentCell = (*nc);
  location->invalidateEntityCache(creature->currentCell);
  return true;
}

void GameManager::commit(lss::Action action) {
  log.info("Committing action: {}", action.name);
  enum { MyEventType = 0 };
  if (action.entity == entt::null || !registry->valid(action.entity) ||
      !registry->has<hf::meta>(action.entity))
    return;
  auto meta = registry->get<hf::meta>(action.entity);
  if (timeline.find(action.entity) == timeline.end()) {
    timeline[action.entity] = std::vector<std::shared_ptr<lss::Action>>();
    auto entity = registry->create();

    auto &track = registry->assign<Sequentity::Track>(
        action.entity, fmt::format("Entity: {}", meta.name).c_str(),
        ImColor::HSV(0.0f, 0.5f, 0.75f));
    track.title = meta.id;
    Sequentity::PushChannel(track, MyEventType,
                            {"Actions", ImColor::HSV(0.33f, 0.5f, 0.75f)});
    action.start = cursor;
  } else {
    auto lastAction = timeline[action.entity].back();
    action.start = lastAction->start + lastAction->cost;
    if (action.start < cursor) {
      action.start = cursor;
    }
  }
  timeline[action.entity].push_back(std::make_shared<lss::Action>(action));

  auto &track = registry->get<Sequentity::Track>(action.entity);
  auto &channel = track.channels[MyEventType];
  Sequentity::PushEvent(channel, {action.start, action.cost,
                                  ImColor::HSV(0.66f, 0.5f, 0.75f), MyEventType,
                                  action.entity});
}

void GameManager::exec(int points) {
  log.start("GameManager::exec");
  std::vector<std::shared_ptr<lss::Action>> to_exec;
  for (auto [e, actions] : timeline) {
    auto local_cursor = cursor;
    auto elapsed = points;
    for (auto &action : actions) {
      // log.debug("Action: {} start: {} cost: {} executed: {} done: {}",
      //           action.name, action.start, action.cost, action.executed,
      //           action.done);
      if (!action->done && action->start <= local_cursor + elapsed) {
        to_exec.push_back(action);
        log.debug("Hit: {} start: {} cost: {} executed: {} done: {}",
                  action->name, action->start, action->cost, action->executed,
                  action->done);
        if (action->cost > elapsed) {
          action->payed = elapsed;
          action->cost -= action->payed;
          elapsed = 0;
          break;
        } else {
          action->payed = action->cost;
          elapsed -= action->cost;
          action->done = true;
        }
      }
      if (action->start > local_cursor + elapsed) {
        break;
      }
    }
  }

  for (auto action : to_exec) {
    if (action->executed)
      continue;
    auto emitter = entt::service_locator<event_emitter>::get().lock();
    emitter->publish<pre_action_event>();
    log.info("Executing action: {} ({})", action->name, action->executed);
    action->action();
    emitter->publish<post_action_event>();
    action->executed = true;
    auto &track = registry->get<Sequentity::Track>(action->entity);
    auto &channel = track.channels[0];
    auto &event =
        *std::find_if(channel.events.begin(), channel.events.end(),
                      [&](auto e) { return e.time == action->start; });
    event.color = ImColor::HSV(0.75f, 0.25f, 0.75f);
    event.enabled = false;
  }
  cursor += points;
  auto &state = registry->ctx<Sequentity::State>();
  state.current_time = cursor;
  state.range[1] = cursor + 250;

  serve();

  // auto emitter = entt::service_locator<event_emitter>::get().lock();
  // // lastAction = action;
  // emitter->publish<pre_action_event>();
  // // emitter->publish<action_event>(action);
  // action.action();
  // emitter->publish<post_action_event>();
  // // recalc light and dark once per turn, not on every action
  // serve();
  // // timeline.clear();
  log.stop("GameManager::exec");
}
