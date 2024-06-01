#include "lss/game/light.hpp"
#include "lss/game/wearableType.hpp"
#include <algorithm>
#include <app/ui/drawEngine.hpp>
#include <app/ui/viewport.hpp>
#include <lss/components.hpp>
#include <lss/generator/room.hpp>
#include <memory>
#include <stdexcept>
#include <tuple>

using RandomLocal = effolkronium::random_thread_local;

void DrawEngine::updateVisible() {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto ents = registry->view<hellfrost::visible>();
  for (auto e : ents) {
    auto render = hf::renderable{};
    if (registry->has<hf::renderable>(e))
      render = registry->get<hf::renderable>(e);
    auto [v, m] = registry->get<hellfrost::visible, hf::meta>(e);
    // log.info("Creating renderable for {}", m.id);
    render.spriteKey =
        (v.sign != "" && viewport->tileSet.sprites.find(v.sign) !=
                             viewport->tileSet.sprites.end())
            ? v.sign
            : "UNKNOWN";
    render.fgColor = viewport->colors[v.type].contains(v.sign)
                         ? viewport->getColorString(v.type, v.sign)
                         : "#aa3333ff";
    registry->assign_or_replace<hellfrost::renderable>(e, render);
  }
}

void DrawEngine::update() {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  updateVisible();

  // registry->on_construct<hf::visible>().connect<&create_renderable>();

  visible = std::make_shared<entt::observer>(
      *registry, entt::collector.group<hf::visible>());
  new_visible = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<hf::visible>());
  renderable = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<hf::renderable>());
  new_renderable = std::make_shared<entt::observer>(
      *registry, entt::collector.group<hf::renderable>());
  position = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<hf::position>());
  ineditor = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<hf::ineditor>());
  glow = std::make_shared<entt::observer>(*registry,
                                          entt::collector.group<hf::glow>());

  layers->layers.clear();

  std::map<std::string_view, int> layerMap = {
      {"bottom", 0},        {"cellsBg", 50},     {"cells", 100},
      {"cellsBrd", 150},    {"entitiesBg", 200}, {"entities", 250},
      {"entitiesBrd", 300}, {"heroBg", 400},     {"hero", 425},
      {"heroBrd", 450},     {"light", 500},      {"darkness", 900},
      {"overlay", 1000},    {"debug", 2000},
  };
  for (auto [layer, zIndex] : layerMap) {
    layers->layers[layer] = std::make_shared<Layer>();
    layers->layers[layer]->zIndex = zIndex;
    // log.info("{} > {}", layer, zIndex);
  }
}

void DrawEngine::updateEntity(entt::entity e) {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto render = registry->get<hf::renderable>(e);
  auto brdLayer = layers->layers[render.brdLayer];
  auto bgLayer = layers->layers[render.bgLayer];
  auto layer = layers->layers[render.fgLayer];
  brdLayer->remove((int)registry->entity(e));
  bgLayer->remove((int)registry->entity(e));
  layer->remove((int)registry->entity(e));
  renderEntity(e);
}

sf::Color DrawEngine::getGlowColorForCell(entt::entity e,
                                          std::shared_ptr<Cell> n, int pulse) {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto [pos, glow] = registry->get<hf::position, hf::glow>(e);
  auto [_c, rz] = viewport->getCell(pos.x, pos.y, 0);
  auto c = *_c;
  std::string t(magic_enum::enum_name(glow.type));
  sf::Color l_color;
  if (glow.type == LightType::INHERIT && registry->has<hf::renderable>(e)) {
    l_color = viewport->getColor(registry->get<hf::renderable>(e).fgColor);
  } else {
    l_color = viewport->getColor("LIGHT", t);
  }
  l_color.a = glow.bright + pulse;
  auto d = sqrt(pow(c->x - n->x, 2) + pow(c->y - n->y, 2));
  l_color.a -= (d - 1) * alpha_per_d;
  // l_color.a += Random::get<float>(-1 * glow.flick, glow.flick);
  l_color.a = std::clamp((int)l_color.a, 0, 200);
  return l_color;
}

void DrawEngine::updateExistingLight() {
  auto mutex = entt::service_locator<std::mutex>::get().lock();
  if (lightUpdating) {
    mutex->unlock();
    return;
  }
  lightUpdating = true;
  mutex->lock();
  // log.start("update existing light", false);
  auto light = layers->layers["light"];
  auto lights = light->children;
  light->clear();
  auto d = 5;
  for (auto [id, t] : lights) {
    auto tile = std::dynamic_pointer_cast<sf::RectangleShape>(t);
    auto c = tile->getFillColor();
    c.a = lightMapAlpha[id] + RandomLocal::get(-d, d);
    c.a = std::clamp((int)c.a, 0, 200);
    tile->setFillColor(c);
    // auto cs = "#005599";
    // auto color = Color::fromHexString(cs);
    // tile->setFillColor(sf::Color(color.r, color.g, color.b, c.a));
  }
  light->children = lights;
  light->invalidate();
  light->update();
  redrawCanvas();
  // log.stop("update existing light");
  mutex->unlock();
  lightUpdating = false;
}

void DrawEngine::updateDark() {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto dark = layers->layers["darkness"];
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto rScale = viewport->scale * GUI_SCALE;
  dark->clear();

  auto location = viewport->regions.front()->location;
  if (location->player == nullptr) {
    log.error("No player exists");
    return;
  }
  log.start("update dark", true);

  auto vis = registry->get<hf::vision>(location->player->entity);
  auto d = vis.distance;
  auto fov = location->getVisible(location->player->currentCell, d, false);
  location->player->viewField = fov;

  // for (auto c : fov) {
  //   c->visibilityState = VisibilityState::VISIBLE;
  // }

  for (auto x = 0; x < location->cells.front().size(); x++) {
    for (auto y = 0; y < location->cells.size(); y++) {
      auto [_c, rz] = viewport->getCell(x, y, 0);
      auto c = *_c;
      if (c->type == CellType::EMPTY || c->type == CellType::UNKNOWN) {
        continue;
      }

      if (c->visibilityState == VisibilityState::VISIBLE) {
        c->visibilityState = VisibilityState::SEEN;
      }

      if (std::find(fov.begin(), fov.end(), c) != fov.end()) {
        c->visibilityState = VisibilityState::VISIBLE;
        continue;
      }

      auto e_size = sf::Vector2f(viewport->tileSet.size.first,
                                 viewport->tileSet.size.second) *
                    rScale;
      auto e_pos = sf::Vector2f(x * viewport->tileSet.size.first,
                                y * viewport->tileSet.size.second) *
                   rScale;
      e_pos.x += -viewport->tileSet.size.first * viewport->view_x * rScale;
      e_pos.y += -viewport->tileSet.size.second * viewport->view_y * rScale;

      auto bg = std::make_shared<sf::RectangleShape>();
      bg->setSize(e_size);
      bg->setPosition(e_pos);
      auto cs = viewport->colors["PALETTE"]["BACKGROUND"];
      if (c->visibilityState == VisibilityState::SEEN) {
        cs = "#1f1f1f";
        if (c->type == CellType::WALL) {
          cs = "#232323cf";
        }
        auto color = Color::fromHexString(cs);
        bg->setFillColor(sf::Color(color.r, color.g, color.b, color.a));
        dark->draw(bg, 100000 + x * 100 + y);
        // if (c->type == CellType::WALL) {
        //   auto sprite = viewport->makeSprite("", "WALL");
        //   auto color = Color::fromHexString("#666666");
        //   sprite->setColor(sf::Color(color.r, color.g, color.b, color.a));
        //   sprite->setPosition(e_pos);

        //   if (registry->has<hf::wall>(e)) {
        //     sprite = viewport->makeSprite(viewport->getWallSpec(cell));
        //   }

        //   auto s = *sprite;
        //   s.setScale(sf::Vector2f(1, 1) * rScale);
        //   dark->draw(std::make_shared<sf::Sprite>(s), 200000 + x * 100 + y);
        // }
      } else {

        auto color = Color::fromHexString(cs);
        bg->setFillColor(sf::Color(color.r, color.g, color.b, color.a));
        dark->draw(bg, 100000 + x * 100 + y);
      }
    }
  }
  log.stop("update dark");
}

void DrawEngine::updateLight() {
  // log.start("update light", true);
  lightMap.clear();
  lightMapAlpha.clear();
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto light = layers->layers["light"];
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto rScale = viewport->scale * GUI_SCALE;
  light->clear();
  auto ents = registry->group(entt::get<hf::position, hf::glow>);
  for (auto e : ents) {
    if (!registry->valid(e)) {
      continue;
    }
    // TODO: fetch only renderable?
    if (registry->has<hf::renderable>(e)) {
      if (registry->get<hf::renderable>(e).hidden) {
        continue;
      }
    }

    auto pos = registry->get<hf::position>(e);

    // if (pos.y - viewport->view_y < 0 || pos.y - viewport->view_y >= vH)
    //   continue;
    // if (pos.x - viewport->view_x < 0 || pos.x - viewport->view_x >= vW)
    //   continue;
    if (pos.z != viewport->view_z)
      continue;

    auto e_size = sf::Vector2f(viewport->tileSet.size.first,
                               viewport->tileSet.size.second) *
                  rScale;
    auto e_pos = sf::Vector2f(pos.x * viewport->tileSet.size.first,
                              pos.y * viewport->tileSet.size.second) *
                 rScale;
    e_pos.x += -viewport->tileSet.size.first * viewport->view_x * rScale;
    e_pos.y += -viewport->tileSet.size.second * viewport->view_y * rScale;

    auto glow = registry->get<hf::glow>(e);
    auto pulse = RandomLocal::get(-1 * glow.pulse, glow.pulse);
    if (glow.distance > 0 && glow.bright > 0) {
      std::vector<int> ids;

      auto [_c, rz] = viewport->getCell(pos.x, pos.y, 0);
      auto c = *_c;
      auto location = viewport->regions.front()->location;
      auto i = 1;

      for (auto n : location->getVisible(c, glow.distance, false)) {
        n->illuminated = true;
        auto lid = 100000 + n->x * 100 + n->y;
        if (std::find(ids.begin(), ids.end(), lid) != ids.end()) {
          continue;
        }
        ids.push_back(lid);

        auto e_pos = sf::Vector2f(n->x * viewport->tileSet.size.first,
                                  n->y * viewport->tileSet.size.second) *
                     rScale;
        e_pos.x += -viewport->tileSet.size.first * viewport->view_x * rScale;
        e_pos.y += -viewport->tileSet.size.second * viewport->view_y * rScale;

        auto bg = std::make_shared<sf::RectangleShape>();
        bg->setSize(e_size);
        bg->setPosition(e_pos);

        auto l_color = getGlowColorForCell(e, n, pulse);
        auto _l = LibColor::Color(l_color.r, l_color.g, l_color.b, l_color.a);
        std::vector<std::tuple<entt::entity, sf::Color>> colors;
        colors.push_back(std::make_tuple(e, l_color));
        if (lightMap[lid].size() != 0) {
          if (glow.passive) {
            continue;
          }
          int alpha = 0;
          for (auto _e : lightMap[lid]) {
            if (!registry->valid(_e)) {
              continue;
            }
            auto g = registry->get<hf::glow>(_e);
            if (g.passive)
              continue;
            std::string _t(magic_enum::enum_name(glow.type));

            auto pulse = RandomLocal::get(-1 * g.pulse, g.pulse);
            auto color = getGlowColorForCell(_e, c, pulse);
            colors.push_back(std::make_tuple(_e, color));
            if (_e == e)
              continue;
            auto _c = LibColor::Color(color.r, color.g, color.b, color.a);
            if (blend_mode == "join") {
              _c.join(_l);
            } else if (blend_mode == "add") {
              _c.add(_l);
            } else if (blend_mode == "sub") {
              _c.sub(_l);
            } else if (blend_mode == "blend") {
              _c.blend(_l);
            }
            // _c.a = std::max(color.a, l_color.a) + alpha_blend_inc;
            // _c.a = std::clamp(_c.a, 0, 255);
            l_color = sf::Color(_c.r, _c.g, _c.b, l_color.a);
          }
        }
        auto alpha = l_color.a;
        auto v = 0;
        auto ag = e;
        for (auto [_e, c] : colors) {
          auto _c = LibColor::Color(c.r, c.g, c.b, c.a);
          if (_c.v > v) {
            v = _c.v;
          }
          if (c.a > alpha) {
            ag = _e;
            alpha = c.a;
          }
        }
        for (auto [_e, c] : colors) {
          if (_e != ag) {
            auto _c = LibColor::Color(c.r, c.g, c.b, c.a);
            alpha += c.a * alpha_blend_inc;
            v += _c.v * alpha_blend_inc;
          }
        }

        l_color.a = alpha;
        // l_color.a = 255;
        if (v > 0) {
          auto _c = LibColor::Color(l_color.r, l_color.g, l_color.b, l_color.a);
          // _c.value(std::clamp(v, 0, 1));
          l_color = sf::Color(_c.r, _c.g, _c.b, _c.a);
        }

        l_color.a += RandomLocal::get<float>(-1 * glow.flick, glow.flick);
        l_color.a = std::clamp((int)l_color.a, 0, max_bright);
        bg->setFillColor(l_color);
        lightMapAlpha[lid] = l_color.a;
        light->draw(bg, lid);
        i++;
        try {
          lightMap[lid].push_back(e);
        } catch (std::out_of_range ex) {
          lightMap[lid] = {e};
        }
      }
    }
  }
  // log.stop("update light");
}

void DrawEngine::renderEntity(entt::entity e) {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  if (!registry->has<hf::position, hf::renderable>(e)) {
    auto m = registry->get<hf::meta>(e);
    log.info("Cancel Rendering: {}", m.id);
    return;
  }
  auto viewport = entt::service_locator<Viewport>::get().lock();
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto rScale = viewport->scale * GUI_SCALE;
  auto [pos, render, meta] =
      registry->get<hf::position, hf::renderable, hf::meta>(e);
  // if (render.spriteKey != "EMPTY_CELL") {
  //   log.info("Rendering: {}", meta.id);
  // }

  if (pos.y - viewport->view_y < 0 || pos.y - viewport->view_y >= vH)
    return;
  if (pos.x - viewport->view_x < 0 || pos.x - viewport->view_x >= vW)
    return;
  if (pos.z != viewport->view_z)
    return;

  auto e_size = sf::Vector2f(viewport->tileSet.size.first,
                             viewport->tileSet.size.second) *
                rScale;
  if (registry->has<hf::size>(e)) {
    auto size = registry->get<hf::size>(e);
    e_size.x *= size.width;
    e_size.y *= size.height;
  }
  auto e_pos = sf::Vector2f(pos.x * viewport->tileSet.size.first,
                            pos.y * viewport->tileSet.size.second) *
               rScale;
  e_pos.x += -viewport->tileSet.size.first * viewport->view_x * rScale;
  e_pos.y += -viewport->tileSet.size.second * viewport->view_y * rScale;
  auto rect = sf::Rect(e_pos, e_size);

  auto brdLayer = layers->layers[render.brdLayer];
  if (brdLayer == nullptr) {
    log.error("Layer {} not found", render.brdLayer);
  }
  auto bgLayer = layers->layers[render.bgLayer];
  if (bgLayer == nullptr) {
    log.error("Layer {} not found", render.bgLayer);
  }
  auto layer = layers->layers[render.fgLayer];
  if (layer == nullptr) {
    log.error("Layer {} not found", render.fgLayer);
  }
  auto light = layers->layers["light"];
  auto eid = (int)registry->entity(e);

  if (render.hidden)
    return;
  if (render.hasBg) {
    auto bg = std::make_shared<sf::RectangleShape>();
    bg->setSize(e_size);
    bg->setPosition(e_pos);
    bg->setFillColor(viewport->getColor(render.bgColor));
    bgLayer->draw(bg, eid);
  }

  if (render.hasBorder) {
    auto bg = std::make_shared<sf::RectangleShape>();
    bg->setSize(e_size);
    bg->setPosition(e_pos);
    bg->setFillColor(sf::Color::Transparent);
    bg->setOutlineColor(viewport->getColor(render.borderColor));
    bg->setOutlineThickness(1);
    brdLayer->draw(bg, eid);
  }

  auto sprite = viewport->makeSprite("", render.spriteKey);
  if (registry->has<hf::wall>(e)) {
    auto [c, rz] = viewport->getCell(pos.x, pos.y, pos.z);
    if (c) {
      auto cell = *c;
      sprite = viewport->makeSprite(viewport->getWallSpec(cell));
    }
  }
  sprite->setColor(viewport->getColor(render.fgColor));
  sprite->setPosition(e_pos);

  auto s = *sprite;
  if (registry->has<hf::size>(e)) {
    auto size = registry->get<hf::size>(e);
    s.setScale(sf::Vector2f(size.width, size.height) * rScale);
  } else {
    s.setScale(sf::Vector2f(1, 1) * rScale);
  }
  layer->draw(std::make_shared<sf::Sprite>(s), (int)registry->entity(e));
}

std::shared_ptr<sf::RectangleShape> makeBg(sf::Vector2f pos) {
  auto viewport = entt::service_locator<Viewport>::get().lock();
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto rScale = viewport->scale * GUI_SCALE;
  auto bg = std::make_shared<sf::RectangleShape>();

  auto e_size = sf::Vector2f(viewport->tileSet.size.first,
                             viewport->tileSet.size.second) *
                rScale;
  auto e_pos = sf::Vector2f(pos.x * viewport->tileSet.size.first,
                            pos.y * viewport->tileSet.size.second) *
               rScale;
  e_pos.x += -viewport->tileSet.size.first * viewport->view_x * rScale;
  e_pos.y += -viewport->tileSet.size.second * viewport->view_y * rScale;

  bg->setSize(e_size);
  bg->setPosition(e_pos);
  return bg;
}

void DrawEngine::resize(sf::Vector2u size) {
  layers->resize(size);
  canvas->create(size.x, size.y);
  sf::FloatRect visibleArea(0, 0, size.x, size.y);
  auto sv = sf::View(visibleArea);
  canvas->setView(sv);

  canvas->clear(bgColor);
}

void DrawEngine::observe() {
  // log.info("OBSERVE");
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto registry = entt::service_locator<entt::registry>::get().lock();

  auto mutex = entt::service_locator<std::mutex>::get().lock();
  mutex->lock();

  // if (new_visible && new_visible->size() > 0) {
  //   log.info("+VISIBLE");
  //   updateVisible();
  //   new_visible->clear();
  // }
  if (visible && visible->size() > 0) {
    log.info("+VISIBLE");
    updateVisible();
    visible->clear();
  }

  if (ineditor && ineditor->size() > 0) {
    log.info("+SELECTED");
    for (auto e : *ineditor) {
      auto ie = registry->get<hf::ineditor>(e);
      if (!registry->has<hf::renderable>(e))
        continue;
      auto r = registry->get<hf::renderable>(e);
      if (ie.selected) {
        r.hasBorder = true;
        r.borderColor = "#ff2";
      } else {
        r.hasBorder = false;
        r.borderColor = "#00000000";
      }
      registry->assign_or_replace<hellfrost::renderable>(e, r);
    }
    ineditor->clear();
  }
  if (position && position->size() > 0) {
    for (auto e : *position) {
      log.info("+MOVED");
      if (registry->has<hf::player>(e)) {
        auto pos = registry->get<hf::position>(e);
        auto [_c, rz] = viewport->getCell(pos.x, pos.y, 0);
        auto c = *_c;
        auto l = viewport->regions[0]->location;
        l->player->currentCell = c;
      }
      updateLight();
      updateDark();
      // position->clear();
    }
  }

  // if (glow && glow->size() > 0) {
  //   for (auto e : *glow) {
  //   }
  //   glow->clear();
  // }
  mutex->unlock();
}

sf::Texture DrawEngine::draw() {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  if (registry == nullptr || registry->size() == 0) {
    return canvas->getTexture();
  }

  // if (!visible || visible->size() == 0) {
  //   return canvas->getTexture();
  // }

  auto label = "frame";
  // log.start(lu::cyan("DRAW"), label, true);
  log.start(label, true);
  auto dirty = false;
  if (fastRedraw && !needRedraw) {
    // for (auto [k, l] : layers->layers) {
    //   l->clear();
    // }

    // canvas->clear(bgColor);
    // canvas->draw(*layers);
    // canvas->display();
    dirty = true;
    fastRedraw = false;
    // return canvas->getTexture();
  }
  observe();
  auto mutex = entt::service_locator<std::mutex>::get().lock();
  mutex->lock();

  auto viewport = entt::service_locator<Viewport>::get().lock();

  std::vector<std::shared_ptr<entt::observer>> observers = {
      position, renderable, new_renderable};
  for (auto observer : observers) {
    if (observer && observer->size() > 0) {
      dirty = true;
      break;
    }
  }

  if (needRedraw) {
    // updateVisible();
    for (auto [k, l] : layers->layers) {
      l->clear();
    }

    canvas->clear(bgColor);
    log.start("full redraw", true);
    std::list<int> h(vH);
    std::iota(h.begin(), h.end(), 0);
    std::list<int> w(vW);
    std::iota(w.begin(), w.end(), 0);
    if (roomDebug) {
      auto l = viewport->regions[0]->location;
      float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
      auto rScale = viewport->scale * GUI_SCALE;
      auto i = 1;
      auto debug = layers->layers["debug"];
      for (auto r : l->rooms) {

        auto e_size = sf::Vector2f(r->width * viewport->tileSet.size.first,
                                   r->height * viewport->tileSet.size.second) *
                      rScale;
        auto e_pos = sf::Vector2f(r->x * viewport->tileSet.size.first,
                                  r->y * viewport->tileSet.size.second) *
                     rScale;
        e_pos.x += -viewport->tileSet.size.first * viewport->view_x * rScale;
        e_pos.y += -viewport->tileSet.size.second * viewport->view_y * rScale;

        auto bg = std::make_shared<sf::RectangleShape>();
        bg->setSize(e_size);
        bg->setPosition(e_pos);
        if (r->color == "") {
          r->color = "white";
        }
        auto c = Color::fromWebName(r->color);
        bg->setFillColor(sf::Color(c.r, c.g, c.b, 100));
        debug->draw(bg, 10000000 + i);
        i++;
      }
    }
    auto ents = registry->group(entt::get<hf::position, hf::renderable>);
    for (auto e : ents) {
      if (registry->valid(e)) {
        renderEntity(e);
      }
    }
    updateLight();
    updateDark();
    needRedraw = false;
    dirty = false;
    canvas->draw(*layers);
    canvas->display();
    redraws++;
    log.stop("full redraw", 50);
  } else if (damage.size() > 0 || dirty) {
    log.debug("prd");
    log.start("partial redraw", true);
    canvas->clear(bgColor);
    // for (auto d : damage) {
    //   auto t = cacheTile(d.first, d.second, viewport->view_z);
    //   if (t) {
    //     updateTile(*t);
    //     tilesUpdated++;
    //   }
    // }

    for (auto observer : observers) {
      for (auto e : *observer) {
        if (!registry->valid(e))
          continue;
        updateEntity(e);
        tilesUpdated++;
      }
      observer->clear();
    }
    // updateExistingLight();
    // updateLight();
    // updateDark();

    damage.clear();
    canvas->draw(*layers);
    canvas->display();
    log.stop("partial redraw", 50);
  }
  mutex->unlock();
  log.stop(label, 50);
  return canvas->getTexture();
}

void DrawEngine::redrawCanvas() {
  canvas->clear(bgColor);
  canvas->draw(*layers);
  canvas->display();
}
