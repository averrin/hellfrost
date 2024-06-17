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

void convertVisibleToRenderable(entt::entity e) {
  auto &registry = entt::locator<entt::registry>::value();
  auto viewport = entt::locator<Viewport>::value();
  auto render = hf::renderable{};
  if (registry.all_of<hf::renderable>(e))
    render = registry.get<hf::renderable>(e);
  auto [v, m] = registry.get<hellfrost::visible, hf::meta>(e);
  render.spriteKey = (v.sign != "" && viewport.tileSet.sprites.find(v.sign) !=
                                          viewport.tileSet.sprites.end())
                         ? v.sign
                         : "UNKNOWN";
  render.fgColor = fmt::format("{}/{}", v.type, v.sign);
  registry.emplace_or_replace<hellfrost::renderable>(e, render);
  registry.remove<hellfrost::visible>(e);
}

GameManager::GameManager(fs::path p) : path(p) {
  loadData();
  generator = std::make_unique<Generator>();
}

void GameManager::gen(LocationSpec spec) {
  srand(seed);
  Random::seed(seed);
  log.info("Seed: {}", seed);

  // registry.reset();
  auto &registry = entt::locator<entt::registry>::value();
  auto data = entt::locator<GameData>::value();
  // registry
  //     .on_construct<hellfrost::renderable>()
  //     .connect<&entt::registry::emplace<hellfrost::position>>(registry);
  if (spec.templateMap.size() == 0) {
    spec.templateMap = data.mapFeatures;
    fmt::print("Map features size: {} in gen\n", data.mapFeatures.size());
  }

  generator->templates = templates;
  generator->features = features;
  location = generator->getLocation(spec);
  // registry = location->registry.emplace();
  // entt::locator<entt::registry>::set(registry);
  // auto view_map = entt::locator<Viewport>::value();

  std::map<std::shared_ptr<Room>, entt::entity> res;
  for (auto r : location->rooms) {
    auto e = registry.create();
    registry.emplace<hellfrost::ingame>(e);
    registry.emplace<hellfrost::room>(e, r);
    registry.emplace_or_replace<hellfrost::tags>(e, r->tags.tags);
    registry.emplace<hellfrost::size>(e, r->width, r->height);
    registry.emplace<hellfrost::children>(e);
    auto rend = hf::renderable{"EMPTY_CELL"};
    rend.brdLayer = "debug";
    registry.emplace<hellfrost::renderable>(e, rend);
    auto i = r->getInfo();
    registry.emplace<hellfrost::meta>(e, i, "", i);
    registry.emplace<hellfrost::position>(e, r->x, r->y, 0, false);
    registry.emplace<hellfrost::ineditor>(e, r->getInfo(),
                                          std::vector<std::string>{"Rooms"},
                                          ICON_FA_VECTOR_SQUARE);
    res[r] = e;
  }
  for (auto r : location->cells) {
    for (auto cell : r) {
      if (cell->type == CellType::UNKNOWN || cell->type == CellType::EMPTY)
        continue;
      auto e = location->addEntity(cell->type.name, cell);
      if (!registry.valid(e) || !registry.all_of<hf::ineditor>(e)) {
        log.error("Invalid entity");
        continue;
      }
      registry.emplace_or_replace<hellfrost::cell>(e, cell);
      auto i = cell->getSId();
      registry.emplace_or_replace<hellfrost::meta>(e, i, "", i);
      auto ie = registry.get<hellfrost::ineditor>(e);
      ie.icon = ICON_FA_CUBE;
      registry.emplace_or_replace<hellfrost::ineditor>(e, ie);
      registry.emplace_or_replace<hellfrost::tags>(e, cell->tags.tags);
      auto render = hf::renderable{};
      if (registry.all_of<hellfrost::renderable>(e)) {
        render = registry.get<hellfrost::renderable>(e);
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
      registry.emplace_or_replace<hellfrost::renderable>(e, render);
      for (auto room : location->rooms) {
        for (auto c : room->cells) {
          if (c == cell) {
            auto re = res[room];
            auto ch = registry.get<hf::children>(re);
            ch.children.push_back(e);
            registry.emplace_or_replace<hf::children>(re, ch);
          }
        }
      }
    }
  }

  // TODO: nest entities in their rooms
  // for (auto i : utils::castObjects<Item>(location->objects)) {
  //   auto e = registry.create();
  //   registry.emplace<hellfrost::ingame>(e);
  //   auto type = data.itemSpecs[i->typeKey];
  //   i->type = type;

  //   i->identified = type.identified;
  //   i->durability = type.durability;
  //   i->unidName = type.name;
  //   i->name = type.name;

  //   registry.emplace<hellfrost::meta>(e, i->getTitle());
  //   registry.emplace<hellfrost::ineditor>(
  //       e, i->getFullTitle(true),
  //       std::vector<std::string>{"Items", i->type.name}, ICON_FA_CARROT);
  //   registry.emplace<hellfrost::visible>(e, "ITEMS", i->type.sign);
  //   registry.emplace<hellfrost::position>(e, i->currentCell->x,
  //                                         i->currentCell->y, 0);
  // }

  // for (auto i : utils::castObjects<Enemy>(location->objects)) {
  //   auto e = registry.create();
  //   registry.emplace<hellfrost::ingame>(e);
  //   registry.emplace<hellfrost::meta>(e, i->type.name);

  //   registry.emplace<hellfrost::ineditor>(
  //       e, i->type.name, std::vector<std::string>{"Enemies", i->type.name},
  //       ICON_FA_DRAGON);

  //   auto key = i->type.name;
  //   std::transform(key.begin(), key.end(), key.begin(), ::toupper);
  //   std::replace(key.begin(), key.end(), ' ', '_');
  //   registry.emplace<hellfrost::visible>(e, "ENEMY", key);
  //   registry.emplace<hellfrost::position>(e, i->currentCell->x,
  //                                         i->currentCell->y, 0);
  // }

  auto ents = registry.group(entt::get<hf::position, hf::meta>);
  for (auto e : ents) {
    if (!registry.valid(e) || registry.all_of<hf::cell>(e)) {
      continue;
    }
    auto pos = registry.get<hf::position>(e);
    if (!pos.movable)
      continue;

    if (pos.x >= location->cells.front().size() ||
        pos.y >= location->cells.size()) {
      registry.destroy(e);
      continue;
    }
    auto c = location->cells[pos.y][pos.x];
    if (c->tags.has("WIPE") || c->type == CellType::UNKNOWN ||
        c->type == CellType::WATER || c->type == CellType::WALL ||
        c->type == CellType::UPSTAIRS || c->type == CellType::DOWNSTAIRS) {
      // TODO: allow lights
      registry.destroy(e);
    }
  }

  if (!registry.view<entt::tag<"proto"_hs>>().size() ) {
    for (auto p : registry.view<entt::tag<"proto"_hs>, hellfrost::visible>()) {
      convertVisibleToRenderable(p);
    }
    location->cellEntities.clear();
    serve();
  }

  location->cellEntities.clear();
  serve();
  return;
  /*
  // EsToProto(EnemyType::CULTIST, data, 99);
  for (auto e : registry.view<entt::tag<"proto"_hs>, hellfrost::ineditor>()) {
    auto v = registry.get<hellfrost::ineditor>(e);
    if (v.folders.front() == "Items") {
      log.info(v.name);
      data.prototypes.destroy(e);
    }
  }
  // auto n = 0;
  // for (auto es : EnemyType::ALL) {
  //   EsToProto(es, data, n);
  //   n++;
  // }
  // return;

  // data.prototypes.reset();
  // auto n = 0;
  std::map<std::string, int> counter;
  for (auto c : Prototype::ALL) {
    for (auto item : c) {
      auto type = data.itemSpecs[item->typeKey];
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

      auto e = data.prototypes.create();
      data.prototypes.emplace<entt::tag<"proto"_hs>>(e);
      // auto key = fmt::format("{}_{}", item->type.name, n);
      auto key = fmt::format("{}", item->name);
      std::transform(key.begin(), key.end(), key.begin(), ::toupper);
      std::replace(key.begin(), key.end(), ' ', '_');
      data.prototypes.emplace<hf::meta>(e, item->getFullTitle(true),
                                        item->unidName, key);
      data.prototypes.emplace<hf::ineditor>(
          e, key, std::vector<std::string>{"Items", item->type.name},
          ICON_FA_CARROT);
      data.prototypes.emplace<hf::visible>(e, "ITEMS", item->type.sign);

      data.prototypes.emplace<hf::pickable>(e, item->type.category,
                                            item->type.identified, item->count,
                                            item->unidName);
      data.prototypes.emplace<hf::wearable>(e, item->type.wearableType,
                                            item->type.durability);
      // n++;
    }
  }

  // for (auto [sid, spec] : data.terrainSpecs) {
  //   auto e = data.prototypes.create();
  //   data.prototypes.emplace<entt::tag<"proto"_hs>>(e);
  //   data.prototypes.emplace<entt::tag<"terrain"_hs>>(e);

  //   data.prototypes.emplace<hf::meta>(e, sid, "", sid);
  //   data.prototypes.emplace<hf::ineditor>(
  //       e, sid, std::vector<std::string>{"Terrain", spec.name}, u8"\uF531");
  //   data.prototypes.emplace<hf::visible>(e, "TERRAIN", spec.sign);
  //   if (spec.light.distance > 0) {
  //     data.prototypes.emplace<hf::glow>(e, spec.light.distance,
  //                                        spec.light.type, spec.light.stable);
  //   }
  //   n++;
  // }
  */
}

// TODO: call it on init
void GameManager::serve() {
  // fmt::print("GameManager::serve\n");
  log.start("GameManager::serve", true);
  auto &registry = entt::locator<entt::registry>::value();
  for (auto [e, c] : location->creatures) {
    if (registry.valid(e) && registry.any_of<hf::position>(e) &&
        c->currentCell != nullptr) {
      auto p = registry.get<hf::position>(e);
      p.x = c->currentCell->x;
      p.y = c->currentCell->y;
      registry.emplace_or_replace<hellfrost::position>(e, p);
    }
  }
  if (registry.valid(location->player->entity) &&
      registry.all_of<hf::position>(location->player->entity)) {
    auto vis = registry.get<hf::vision>(location->player->entity);
    auto d = vis.distance;
    auto fov = location->player->viewField;
    if (fov.size() == 0) {
      fov = location->getVisible(location->player->currentCell, d, true);
      if (fov.size() == 0) {
        log.error("No visible cells");
        return;
      }
    }
    for (auto c : fov) {
      c->seeThrough = c->type.seeThrough;
      c->passThrough = c->type.passThrough;
      auto es = location->getEntities(c);
      for (auto e : es) {
        if (registry.all_of<hf::obstacle>(e)) {
          auto o = registry.get<hf::obstacle>(e);
          if (!o.seeThrough) {
            c->seeThrough = false;
          }
          if (!o.passThrough) {
            c->passThrough = false;
          }
          auto visCacheThread = std::thread([&]() {
            auto vis = registry.get<hf::vision>(location->player->entity);
            auto d = vis.distance;
            for (auto n :
                 location->getNeighbors(location->player->currentCell)) {
              if (!n->passThrough)
                continue;
              location->getVisible(n, d, true);
              location->getEntities(n);
            }
          });
          visCacheThread.detach();
          // fmt::print("GameManager::after creatures\n");
          //
        }
      }
    }
  }
  log.stop("GameManager::serve");
}

// void GameManager::EsToProto(EnemySpec es, std::shared_ptr<GameData> data,
//                             int n) {
//   auto key = es.name;
//   std::transform(key.begin(), key.end(), key.begin(), ::toupper);
//   std::replace(key.begin(), key.end(), ' ', '_');

//   auto e = data.prototypes.create();
//   data.prototypes.emplace<entt::tag<"proto"_hs>>(e);
//   // auto id = fmt::format("{}_{}", es.name, n);
//   data.prototypes.emplace<hf::meta>(e, es.name, "", key);
//   data.prototypes.emplace<hf::ineditor>(
//       e, key, std::vector<std::string>{"Enemies", es.name}, ICON_FA_DRAGON);
//   data.prototypes.emplace<hf::visible>(e, "ENEMY", key);
// }

std::shared_ptr<EntityWrapper>
GameManager::createInLua(std::shared_ptr<Location> location,
                         const std::string id, const int x, const int y,
                         const int z) {
  auto ne =
      location->addEntity(id, std::make_shared<Cell>(x, y, CellType::EMPTY));
  auto ew = std::make_shared<EntityWrapper>(ne);
  return ew;
}

bool GameManager::moveCreature(std::shared_ptr<Creature> creature,
                               Direction d) {
  if (creature == nullptr || creature->currentCell == nullptr) {
    return false;
  }
  auto nc = location->getCell(creature->currentCell, d);
  if (!(*nc)->canPass({})) {
    return false;
  }
  fmt::print("Move from {} {} to: {} {}\n", creature->currentCell->x,
             creature->currentCell->y, (*nc)->x, (*nc)->y);
  location->invalidateVisibilityCache(creature->currentCell);
  location->invalidateEntityCache(creature->currentCell);

  creature->currentCell = (*nc);
  location->invalidateEntityCache(creature->currentCell);
  return true;
}

void GameManager::commit(lss::Action action) {
  log.info("Committing action: {}", action.name);
  auto &registry = entt::locator<entt::registry>::value();
  enum { MyEventType = 0 };
  if (action.entity == entt::null || !registry.valid(action.entity) ||
      !registry.all_of<hf::meta>(action.entity))
    return;
  auto meta = registry.get<hf::meta>(action.entity);
  if (timeline.find(action.entity) == timeline.end()) {
    timeline[action.entity] = std::vector<std::shared_ptr<lss::Action>>();
    auto entity = registry.create();

    auto &track = registry.emplace<Sequentity::Track>(
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

  auto &track = registry.get<Sequentity::Track>(action.entity);
  auto &channel = track.channels[MyEventType];
  Sequentity::PushEvent(channel, {action.start, action.cost,
                                  ImColor::HSV(0.66f, 0.5f, 0.75f), MyEventType,
                                  action.entity});
}

void GameManager::exec(int points) {
  log.start("GameManager::exec");
  auto &registry = entt::locator<entt::registry>::value();
  auto &emitter = entt::locator<event_emitter>::value();
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
    emitter.publish<pre_action_event>(pre_action_event{*action});
    log.info("Executing action: {} ({})", action->name, action->executed);
    action->action();
    emitter.publish<post_action_event>(post_action_event{*action});
    action->executed = true;
    auto &track = registry.get<Sequentity::Track>(action->entity);
    auto &channel = track.channels[0];
    auto &event =
        *std::find_if(channel.events.begin(), channel.events.end(),
                      [&](auto e) { return e.time == action->start; });
    event.color = ImColor::HSV(0.75f, 0.25f, 0.75f);
    event.enabled = false;
  }
  cursor += points;
  auto &state = registry.ctx().template get<Sequentity::State>();
  state.current_time = cursor;
  state.range[1] = cursor + 250;

  serve();

  // auto emitter = entt::locator<event_emitter>::get().lock();
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

void GameManager::destroyEntity(entt::entity e) {
  log.debug("Destroy: {}", (int)e);
  auto &emitter = entt::locator<event_emitter>::value();
  auto &registry = entt::locator<entt::registry>::value();
  if (registry.any_of<hf::position>(e)) {
    auto p = registry.get<hf::position>(e);
    location->invalidateVisibilityCache(location->cells[p.y][p.x]);
    location->invalidateEntityCache(location->cells[p.y][p.x]);
  }
  if (registry.any_of<hf::script>(e)) {
    auto script = registry.get<hf::script>(e);
    emitter.publish(script_event{e, script.path, "destroy"});
  }
  registry.destroy(e);
  // FIXME: use damage_event
  emitter.publish(redraw_event{});
}
