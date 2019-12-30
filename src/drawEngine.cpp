#include <app/ui/drawEngine.hpp>
#include <app/ui/viewport.hpp>
#include <lss/components.hpp>

void DrawEngine::update() {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto ents = registry->view<hellfrost::visible>();
  for (auto e : ents) {
    auto v = registry->get<hellfrost::visible>(e);
    registry->assign_or_replace<hellfrost::renderable>(
        e,
        (v.sign != "" && viewport->tileSet.sprites.find(v.sign) !=
                             viewport->tileSet.sprites.end())
            ? v.sign
            : "UNKNOWN",
        viewport->colors[v.type].contains(v.sign)
            ? viewport->colors[v.type][v.sign]
            : "#aa3333ff");
  }
  visible = std::make_shared<entt::observer>(
      *registry, entt::collector.group<hf::visible>());
  renderable = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<hf::renderable>());
  new_renderable = std::make_shared<entt::observer>(
      *registry, entt::collector.group<hf::renderable>());
  position = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<hf::position>());
  ineditor = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<hf::ineditor>());

  layers->layers.clear();

  layers->layers["cellsBg"] = std::make_shared<Layer>();
  layers->layers["cellsBg"]->zIndex = 0;
  layers->layers["cells"] = std::make_shared<Layer>();
  layers->layers["cells"]->zIndex = 100;
  layers->layers["entitiesBg"] = std::make_shared<Layer>();
  layers->layers["entitiesBg"]->zIndex = 200;
  layers->layers["entitiesBrd"] = std::make_shared<Layer>();
  layers->layers["entitiesBrd"]->zIndex = 300;
  layers->layers["entities"] = std::make_shared<Layer>();
  layers->layers["entities"]->zIndex = 400;

  layers->layers["overlay"] = std::make_shared<Layer>();
  layers->layers["overlay"]->zIndex = 1000;
}

void DrawEngine::updateEntity(entt::entity e) {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto brdLayer = layers->layers["entitiesBrd"];
  auto bgLayer = layers->layers["entitiesBg"];
  auto layer = layers->layers["entities"];
  brdLayer->remove((int)registry->entity(e));
  bgLayer->remove((int)registry->entity(e));
  layer->remove((int)registry->entity(e));
  renderEntity(e);
}

void DrawEngine::renderEntity(entt::entity e) {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  if (!registry->has<hf::position, hf::renderable>(e))
    return;
  auto viewport = entt::service_locator<Viewport>::get().lock();
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto rScale = viewport->scale * GUI_SCALE;
  auto [pos, render] = registry->get<hf::position, hf::renderable>(e);

  if (pos.y - viewport->view_y < 0 || pos.y - viewport->view_y >= vH)
    return;
  if (pos.x - viewport->view_x < 0 || pos.x - viewport->view_x >= vW)
    return;
  if (pos.z != viewport->view_z)
    return;

  auto e_size = sf::Vector2f(viewport->tileSet.size.first,
                             viewport->tileSet.size.second) *
                rScale;
  auto e_pos = sf::Vector2f(pos.x * viewport->tileSet.size.first,
                            pos.y * viewport->tileSet.size.second) *
               rScale;
  e_pos.x += -viewport->tileSet.size.first * viewport->view_x * rScale;
  e_pos.y += -viewport->tileSet.size.second * viewport->view_y * rScale;
  auto rect = sf::Rect(e_pos, e_size);

  auto bgLayer = layers->layers["entitiesBg"];
  auto layer = layers->layers["entities"];
  auto brdLayer = layers->layers["entitiesBrd"];

  if (render.hidden)
    return;
  if (render.hasBg) {
    auto bg = std::make_shared<sf::RectangleShape>();
    bg->setSize(e_size);
    bg->setPosition(e_pos);
    bg->setFillColor(viewport->getColor(render.bgColor));
    bgLayer->draw(bg, (int)registry->entity(e));
  }

  if (render.hasBorder) {
    auto bg = std::make_shared<sf::RectangleShape>();
    bg->setSize(e_size);
    bg->setPosition(e_pos);
    bg->setFillColor(sf::Color::Transparent);
    bg->setOutlineColor(viewport->getColor(render.borderColor));
    bg->setOutlineThickness(1);
    brdLayer->draw(bg, (int)registry->entity(e));
  }

  auto sprite = viewport->makeSprite("", render.spriteKey);
  sprite->setColor(viewport->getColor(render.fgColor));
  sprite->setPosition(e_pos);
  auto s = *sprite;
  s.setScale(sf::Vector2f(1, 1) * rScale);
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

void DrawEngine::updateTile(std::shared_ptr<Tile> t) {
  auto bgLayer = layers->layers["cellsBg"];
  auto layer = layers->layers["cells"];

  bgLayer->remove(t->id());
  layer->remove(t->id());
  renderTile(t);
}

void DrawEngine::renderTile(std::shared_ptr<Tile> t) {
  auto viewport = entt::service_locator<Viewport>::get().lock();
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto rScale = viewport->scale * GUI_SCALE;

  auto bgLayer = layers->layers["cellsBg"];
  auto layer = layers->layers["cells"];
  auto overlay = layers->layers["overlay"];

  if (t->pos.z < viewport->view_z) {
    auto ol = makeBg({t->pos.x, t->pos.y});
    auto sprite = viewport->makeSprite("ENV", "OVERLAY");
    sprite->setPosition(ol->getPosition());
    auto s = *sprite;
    s.setScale(sf::Vector2f(1, 1) * rScale);
    overlay->draw(std::make_shared<sf::Sprite>(s), t->id());
  }

  auto bg = makeBg({t->pos.x, t->pos.y});
  if (t->hasBackground) {
    bg->setFillColor(t->bgColor);
    bgLayer->draw(bg, t->id());
  }
  for (auto sprite : t->sprites) {
    sprite->setPosition(bg->getPosition());
    auto s = *sprite;
    s.setScale(sf::Vector2f(1, 1) * rScale);
    layer->draw(std::make_shared<sf::Sprite>(s), t->id());
  }
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
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto registry = entt::service_locator<entt::registry>::get().lock();

  auto mutex = entt::service_locator<std::mutex>::get().lock();
  mutex->lock();

  if (visible->size() > 0) {
    for (auto e : *visible) {
      log.info(lu::cyan("DRAW"), "+visible");
      auto v = registry->get<hellfrost::visible>(e);
      if (!registry->has<hf::position>(e))
        continue;
      auto p = registry->get<hf::position>(e);
      registry->assign_or_replace<hellfrost::renderable>(
          e,
          (v.sign != "" && viewport->tileSet.sprites.find(v.sign) !=
                               viewport->tileSet.sprites.end())
              ? v.sign
              : "UNKNOWN",
          viewport->colors[v.type].contains(v.sign)
              ? viewport->colors[v.type][v.sign]
              : "#aa3333ff");
    }
    visible->clear();
  }

  if (ineditor->size() > 0) {
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
  mutex->unlock();
}

sf::Texture DrawEngine::draw() {
  auto label = "frame";
  log.start(lu::cyan("DRAW"), label, true);
  observe();

  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto registry = entt::service_locator<entt::registry>::get().lock();

  std::vector<std::shared_ptr<entt::observer>> observers = {
      position, renderable, new_renderable};
  auto dirty = false;
  for (auto observer : observers) {
    if (observer->size() > 0) {
      dirty = true;
      break;
    }
  }

  if (needRedraw) {
    for (auto [k, l] : layers->layers) {
      l->clear();
    }

    canvas->clear(bgColor);
    log.start(lu::cyan("DRAW"), "full redraw", true);
    std::list<int> h(vH);
    std::iota(h.begin(), h.end(), 0);
    std::list<int> w(vW);
    std::iota(w.begin(), w.end(), 0);
    // #ifdef __APPLE__
    for (auto ly : h) {
      for (auto lx : w) {
        auto t = cacheTile(lx, ly, viewport->view_z);
        if (t) {
          renderTile(*t);
        }
      }
    }

    auto ents = registry->group(entt::get<hf::position, hf::renderable>);
    for (auto e : ents) {
      if (registry->valid(e)) {
        renderEntity(e);
      }
    }
    // #else
    //       #include <execution>
    //       #include <algorithm>
    //       std::for_each(std::execution::par, h.begin(), h.end(), [&](auto
    //       ly){
    //         std::for_each(std::execution::par, w.begin(), w.end(), [&,
    //         =ly](auto lx){
    //           auto t = viewport->getTile(lx, ly, viewport->view_z);
    //           if (t) {
    //             renderTile(canvas, *t);
    //           }
    //         });
    //       });
    // #endif
    needRedraw = false;
    dirty = false;
    canvas->draw(*layers);
    canvas->display();
    redraws++;
    log.stop("full redraw", 50);
  } else if (damage.size() > 0 || dirty) {
    log.start(lu::cyan("DRAW"), "partial redraw", true);
    canvas->clear(bgColor);
    for (auto d : damage) {
      auto t = cacheTile(d.first, d.second, viewport->view_z);
      if (t) {
        updateTile(*t);
        tilesUpdated++;
      }
    }

    for (auto observer : observers) {
      for (auto e : *observer) {
        if (!registry->valid(e))
          continue;
        updateEntity(e);
        tilesUpdated++;
      }
      observer->clear();
    }

    damage.clear();
    canvas->draw(*layers);
    canvas->display();
    log.stop("partial redraw", 50);
  }
  log.stop(label, 50);
  return canvas->getTexture();
}

std::optional<std::shared_ptr<Tile>> DrawEngine::cacheTile(int x, int y,
                                                           int z) {
  auto viewport = entt::service_locator<Viewport>::get().lock();
  x += viewport->view_x;
  y += viewport->view_y;
  auto [c, rz] = viewport->getCell(x, y, z);
  if (!c) {
    return std::nullopt;
  }
  auto cell = *c;
  auto coords = fmt::format("{}.{}.{}", x, y, rz);
  auto t = tilesCache.find(coords);
  if (t != tilesCache.end())
    return t->second;

  auto tile = viewport->getTile(x,y, z);
  if (tile) {
    tilesCache[coords] = *tile;
  }
  return tile;
}
