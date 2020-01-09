#include <hellfrost/deps.hpp>
#include <hellfrost/game/components.hpp>

#include <hellfrost/ui/drawEngine.hpp>
#include <hellfrost/ui/viewport.hpp>

#include <hellfrost/config.hpp>

namespace hellfrost {

DrawEngine::DrawEngine(std::shared_ptr<sf::RenderWindow> w, fs::path PATH)
    : window(w) {
  log.setParent(&LibLog::Logger::getInstance());
  entt::service_locator<Viewport>::empty();
  layers = std::make_shared<LayersManager>();
  auto viewport = std::make_shared<Viewport>();
  entt::service_locator<Viewport>::set(viewport);

  auto config = entt::service_locator<Config>::get().lock();
  loadTileset(PATH / fs::path("data/tilesets") / config->tileset);

  vW = viewport->width / viewport->scale;
  vH = viewport->height / viewport->scale;

  auto c = Color::fromHexString(tileSet->colors["PALETTE"]["BACKGROUND"]);
  bgColor = sf::Color(c.r, c.g, c.b, c.a);

  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto emitter = entt::service_locator<event_emitter>::get().lock();
  emitter->on<center_event>([&](const auto &p, auto &em) {
    auto viewport = entt::service_locator<Viewport>::get().lock();
    // lockedPos = {p.x, p.y};
    viewport->view_x = p.x - viewport->width / 2 / GUI_SCALE;
    viewport->view_y = p.y - viewport->height / 2 / GUI_SCALE;
    invalidate();
  });

  emitter->on<gm::generation_start>([&](const auto &event, auto &em) {
    auto viewport = entt::service_locator<Viewport>::get().lock();
    viewport->update(event.location);
    update();
    resize();
  });

  emitter->on<gm::generation_finish>([&](const auto &event, auto &em) {
    auto viewport = entt::service_locator<Viewport>::get().lock();
    viewport->update(event.location);
    update();
    resize();
  });

  emitter->on<resize_event>([&](const auto &p, auto &em) { resize(); });
  emitter->on<redraw_event>([&](const auto &p, auto &em) { invalidate(); });
  emitter->on<location_update_event>([&](const auto &p, auto &em) { invalidate(); });
}

void DrawEngine::loadTileset(fs::path path) {
  log.debug("Loading tileset from: {}", lu::green(path.string()));
  log.start("Loading");
  std::ifstream file(path / "tileset.json");
  cereal::JSONInputArchive iarchive(file);
  TileSet ts;
  ts.serialize(iarchive);
  tileSet = std::make_shared<TileSet>(ts);

  // TODO: move to drawEngine
  tilesTextures.clear();
  for (auto t_path : tileSet->maps) {
    sf::Texture t;
    t.loadFromFile(path / t_path);
    tilesTextures.push_back(t);
  }
  log.debug("tileset size: {}x{}", tileSet->size.first, tileSet->size.second);
  log.debug("sprite categories: {}", tileSet->sprites.size());

  log.stop("Loading");
}

void DrawEngine::update() {
  auto registry = entt::service_locator<entt::registry>::get().lock();
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto ents = registry->view<visible>();
  for (auto e : ents) {
    auto v = registry->get<visible>(e);
    registry->assign_or_replace<renderable>(
        e,
        (v.sign != "" && tileSet->sprites.find(v.sign) != tileSet->sprites.end())
            ? v.sign
            : "UNKNOWN",
        tileSet->colors[v.type].find(v.sign) != tileSet->colors[v.type].end() ? tileSet->colors[v.type][v.sign]
                                                : "#aa3333ff");
  }
  ob_visible = std::make_shared<entt::observer>(
      *registry, entt::collector.group<visible>());
  ob_renderable = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<renderable>());
  ob_new_renderable = std::make_shared<entt::observer>(
      *registry, entt::collector.group<renderable>());
  ob_position = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<position>());
  ob_ineditor = std::make_shared<entt::observer>(
      *registry, entt::collector.replace<ineditor>());

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
  if (!registry->has<position, renderable>(e))
    return;
  auto viewport = entt::service_locator<Viewport>::get().lock();
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto rScale = viewport->scale * GUI_SCALE;
  auto [pos, render] = registry->get<position, renderable>(e);

  if (pos.y - viewport->view_y < 0 || pos.y - viewport->view_y >= vH)
    return;
  if (pos.x - viewport->view_x < 0 || pos.x - viewport->view_x >= vW)
    return;
  if (pos.z != viewport->view_z)
    return;

  auto e_size = sf::Vector2f(tileSet->size.first, tileSet->size.second) * rScale;
  auto e_pos =
      sf::Vector2f(pos.x * tileSet->size.first, pos.y * tileSet->size.second) *
      rScale;
  e_pos.x += -tileSet->size.first * viewport->view_x * rScale;
  e_pos.y += -tileSet->size.second * viewport->view_y * rScale;
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
    bg->setFillColor(getColor(render.bgColor));
    bgLayer->draw(bg, (int)registry->entity(e));
  }

  if (render.hasBorder) {
    auto bg = std::make_shared<sf::RectangleShape>();
    bg->setSize(e_size);
    bg->setPosition(e_pos);
    bg->setFillColor(sf::Color::Transparent);
    bg->setOutlineColor(getColor(render.borderColor));
    bg->setOutlineThickness(1);
    brdLayer->draw(bg, (int)registry->entity(e));
  }

  auto sprite = makeSprite("", render.spriteKey);
  sprite->setColor(getColor(render.fgColor));
  sprite->setPosition(e_pos);
  auto s = *sprite;
  s.setScale(sf::Vector2f(1, 1) * rScale);
  layer->draw(std::make_shared<sf::Sprite>(s), (int)registry->entity(e));
}

std::shared_ptr<sf::RectangleShape> DrawEngine::makeBg(sf::Vector2f pos) {
  auto viewport = entt::service_locator<Viewport>::get().lock();
  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto rScale = viewport->scale * GUI_SCALE;
  auto bg = std::make_shared<sf::RectangleShape>();

  auto e_size = sf::Vector2f(tileSet->size.first, tileSet->size.second) * rScale;
  auto e_pos =
      sf::Vector2f(pos.x * tileSet->size.first, pos.y * tileSet->size.second) *
      rScale;
  e_pos.x += -tileSet->size.first * viewport->view_x * rScale;
  e_pos.y += -tileSet->size.second * viewport->view_y * rScale;

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
    auto sprite = makeSprite("ENV", "OVERLAY");
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

void DrawEngine::resize() {
  auto size = window->getSize();
  layers->resize(size);
  canvas->create(size.x, size.y);
  sf::FloatRect visibleArea(0, 0, size.x, size.y);
  auto sv = sf::View(visibleArea);
  canvas->setView(sv);

  canvas->clear(bgColor);

  float GUI_SCALE = entt::monostate<"gui_scale"_hs>{};
  auto viewport = entt::service_locator<Viewport>::get().lock();
  viewport->width = size.x / tileSet->size.first + 1;
  viewport->height = size.y / tileSet->size.second + 1;
  vW = viewport->width / viewport->scale / GUI_SCALE;
  vH = viewport->height / viewport->scale / GUI_SCALE;
  // tilesCache.clear();
  invalidate();
}

void DrawEngine::observe() {
  const std::lock_guard<std::mutex> lock(cache_mutex);
  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto registry = entt::service_locator<entt::registry>::get().lock();

  if (ob_visible->size() > 0) {
    for (auto e : *ob_visible) {
      // log.info(lu::cyan("DRAW"), "+visible");
      auto v = registry->get<visible>(e);
      if (!registry->has<position>(e))
        continue;
      auto p = registry->get<position>(e);
      registry->assign_or_replace<renderable>(
          e,
          (v.sign != "" &&
           tileSet->sprites.find(v.sign) != tileSet->sprites.end())
              ? v.sign
              : "UNKNOWN",
          tileSet->colors[v.type].find(v.sign) != tileSet->colors[v.type].end()
              ? tileSet->colors[v.type][v.sign]
              : "#aa3333ff");
    }
    ob_visible->clear();
  }

  if (ob_ineditor->size() > 0) {
    for (auto e : *ob_ineditor) {
      auto ie = registry->get<ineditor>(e);
      if (!registry->has<renderable>(e))
        continue;
      auto r = registry->get<renderable>(e);
      if (ie.selected) {
        r.hasBorder = true;
        r.borderColor = "#ff2";
      } else {
        r.hasBorder = false;
        r.borderColor = "#00000000";
      }
      registry->assign_or_replace<renderable>(e, r);
    }
    ob_ineditor->clear();
  }
}

sf::Texture DrawEngine::Draw() {
  auto label = "frame";
  // log.debug("start frame draw");
  log.start(label, true);
  observe();

  auto viewport = entt::service_locator<Viewport>::get().lock();
  auto registry = entt::service_locator<entt::registry>::get().lock();

  std::vector<std::shared_ptr<entt::observer>> observers = {
      ob_position, ob_renderable, ob_new_renderable};
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
    log.start("full redraw", true);
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

    auto ents = registry->group(entt::get<position, renderable>);
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
    log.start("partial redraw", true);
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
  const std::lock_guard<std::mutex> lock(cache_mutex);
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

  auto tile = getTile(x, y, z);
  if (tile) {
    tilesCache[coords] = *tile;
  }
  return tile;
}

std::shared_ptr<sf::Sprite> DrawEngine::makeSprite(std::string cat,
                                                   std::string key) {
  auto spec = tileSet->sprites["ENV"]["UNKNOWN"];
  if (tileSet->sprites[cat].find(key) != tileSet->sprites[cat].end()) {
    spec = tileSet->sprites[cat][key];
  } else {
    fmt::print("Missed sprite spec: {}: {}\n", cat, key);
  }
  auto s = std::make_shared<sf::Sprite>();
  s->setTexture(tilesTextures[spec[0]]);
  s->setTextureRect(getTileRect(spec[1], spec[2]));
  if (tileSet->colors[cat].find(key) != tileSet->colors[cat].end()) {
    s->setColor(getColor(tileSet->colors[cat][key]));
  } else {
    // fmt::print("Missed color: {}: {}\n", cat, key);
  }
  return s;
}

std::array<int, 3> DrawEngine::getWallSpec(std::shared_ptr<Cell> cell) {
  return {0, 0, 0};
  /*
  auto coords = getCoords(cell);
  auto [_l, _z1] = getCell(coords.first - 1, coords.second, view_z);
  auto [_r, _z2] = getCell(coords.first + 1, coords.second, view_z);
  auto [_t, _z3] = getCell(coords.first, coords.second - 1, view_z);
  auto [_b, _z4] = getCell(coords.first, coords.second + 1, view_z);

  auto spec = tileSet->sprites["WALL_H"];
  if (_r && _l &&
      ((*_r)->type == CellType::WALL && (*_l)->type != CellType::WALL)) {
    spec = tileSet->sprites["WALL_HRS"];
  } else if (_r && _l &&
             ((*_r)->type != CellType::WALL &&
              (*_l)->type == CellType::WALL)) {
    spec = tileSet->sprites["WALL_HLS"];
  }
  if (_t && _b &&
      ((*_t)->type == CellType::WALL && (*_b)->type == CellType::WALL)) {
    spec = tileSet->sprites["WALL_V"];
  } else if (_t && _b &&
             ((*_t)->type == CellType::WALL &&
              (*_b)->type != CellType::WALL)) {
    spec = tileSet->sprites["WALL_VBS"];
  } else if (_t && _b &&
             ((*_t)->type != CellType::WALL &&
              (*_b)->type == CellType::WALL)) {
    spec = tileSet->sprites["WALL_VTS"];
  }

  if (_t && _b && (*_t)->type == CellType::WALL &&
      (*_b)->type == CellType::WALL && _l && _r &&
      (*_l)->type == CellType::WALL && (*_r)->type == CellType::WALL) {
    spec = tileSet->sprites["WALL_CROSS"];
    return spec;
  }
  if (_t && _b && (*_t)->type != CellType::WALL &&
      (*_b)->type != CellType::WALL && _l && _r &&
      (*_l)->type != CellType::WALL && (*_r)->type != CellType::WALL) {
    spec = tileSet->sprites["WALL_C"];
    return spec;
  }

  if (_l && _r && (*_l)->type == CellType::WALL &&
      (*_r)->type == CellType::WALL) {
    spec = tileSet->sprites["WALL_H"];
    if (_b && (*_b)->type == CellType::WALL) {
      spec = tileSet->sprites["WALL_HB"];
    }
    if (_t && (*_t)->type == CellType::WALL) {
      spec = tileSet->sprites["WALL_HT"];
    }
    return spec;
  }
  if (_t && _b && (*_t)->type == CellType::WALL &&
      (*_b)->type == CellType::WALL) {
    spec = tileSet->sprites["WALL_V"];
    if (_l && (*_l)->type == CellType::WALL) {
      spec = tileSet->sprites["WALL_VL"];
    }
    if (_r && (*_r)->type == CellType::WALL) {
      spec = tileSet->sprites["WALL_VR"];
    }
    return spec;
  }

  if (_l && _b && (*_l)->type == CellType::WALL &&
      (*_b)->type == CellType::WALL) {
    spec = tileSet->sprites["WALL_LB"];
  } else if (_r && _b && (*_r)->type == CellType::WALL &&
             (*_b)->type == CellType::WALL) {
    spec = tileSet->sprites["WALL_RB"];
  } else if (_r && _t && (*_r)->type == CellType::WALL &&
             (*_t)->type == CellType::WALL) {
    spec = tileSet->sprites["WALL_RT"];
  } else if (_l && _t && (*_l)->type == CellType::WALL &&
             (*_t)->type == CellType::WALL) {
    spec = tileSet->sprites["WALL_LT"];
  }
  return spec;
*/
}

std::optional<std::shared_ptr<Tile>> DrawEngine::getTile(int x, int y, int z) {
  return std::nullopt;

  /*
auto [c, rz] = getCell(x, y, z);
if (!c) {
  fmt::print("nullopt @ {}.{}.{}\n", x, y, z);
  return std::nullopt;
}
auto cell = *c;

if (cell->type == CellType::UNKNOWN) {
  return std::nullopt;
}

// mutex.lock();
auto fgColor = sf::Color(220, 220, 220);
auto sprite = std::make_shared<sf::Sprite>();
auto tile = std::make_shared<Tile>();
tile->sprites.push_back(sprite);
auto sprite_spec = tileSet->sprites["UNKNOWN"];
if (cell->type == CellType::UPSTAIRS) {
  sprite_spec = tileSet->sprites["UPSTAIRS"];
} else if (cell->type == CellType::DOWNSTAIRS) {
  sprite_spec = tileSet->sprites["DOWNSTAIRS"];
} else if (cell->type == CellType::WATER) {
  sprite_spec = tileSet->sprites["WATER"];
  auto c = Color::fromHexString(colors["ENV"]["WATER"]);
  c.value(c.value() + gen->R(0, 30) / 100.f);
  fgColor = sf::Color(c.r, c.g, c.b, c.a);
  tile->bgColor = getColor(colors["ENV"]["WATER_BG"]);
  tile->hasBackground = true;
} else if (cell->type == CellType::VOID) {
  sprite_spec = tileSet->sprites["VOID"];
  fgColor = getColor(colors["ENV"]["VOID"]);
  tile->bgColor = getColor(colors["ENV"]["VOID"]);
  tile->hasBackground = true;
} else if (cell->passThrough) {

  if (cell->type == CellType::GROUND) {
    sprite_spec = tileSet->sprites["GRASS"];
    auto brown = Color::fromHexString(colors["PALETTE"]["BROWN"]);
    brown.g += gen->R(0, 60);
    fgColor = sf::Color(brown.r, brown.g, brown.b, brown.a);
    tile->bgColor = getColor(colors["ENV"]["GROUND_BG"]);
    tile->hasBackground = true;
    if (cell->hasFeature(CellFeature::CAVE)) {
      tile->bgColor = getColor(colors["ENV"]["CAVE_BG"]);
      tile->hasBackground = true;
    }
  } else if (cell->type == CellType::FLOOR) {
    sprite_spec = tileSet->sprites["FLOOR"];
    fgColor = getColor(colors["ENV"]["FLOOR"]);
  }

  // if (region) {
  //   auto doors =
  //       utils::castObjects<Door>((*region)->location->getObjects(cell));
  //   if (doors.size() != 0) {
  //     if (doors.front()->hidden) {
  //       sprite_spec = getWallSpec(cell);
  //       fgColor = getColor(colors["ENV"]["WALL"]);
  //       if (cell->hasFeature(CellFeature::CAVE)) {
  //         fgColor = getColor(colors["ENV"]["WALL_CAVE"]);
  //         tile->bgColor = getColor(colors["ENV"]["CAVE_BG"]);
  //         tile->hasBackground = true;
  //       }
  //     } else {
  //       sprite_spec = tileSet->sprites["DOOR"];
  //       fgColor = getColor(colors["ENV"]["DOOR"]);
  //     }
  //   }
  // }
} else if (cell->type == CellType::WALL) {
  sprite_spec = getWallSpec(cell);
  fgColor = getColor(colors["ENV"]["WALL"]);
  if (cell->hasFeature(CellFeature::CAVE)) {
    fgColor = getColor(colors["ENV"]["WALL_CAVE"]);
    tile->bgColor = getColor(colors["ENV"]["CAVE_BG"]);
    tile->hasBackground = true;
  }
} else if (cell->type == CellType::ROOF) {
  sprite_spec = {0, 1, 17};
}

sprite->setTexture(tilesTextures[sprite_spec[0]]);

auto src = getTileRect(sprite_spec[1], sprite_spec[2]);
sprite->setTextureRect(src);
sprite->setColor(fgColor);

// tile->bgColor = sf::Color(255, 255, 255, rand()%256);

// if (cell->type != CellType::UNKNOWN) {
for (auto f : cell->features) {
  if (f == CellFeature::BLOOD) {
    tile->bgColor = getColor(colors["ENV"]["BLOOD"]);
    tile->hasBackground = true;
  }
  if (f == CellFeature::FROST) {
    tile->bgColor = getColor(colors["ENV"]["FROST"]);
    tile->hasBackground = true;
  }
  if (f == CellFeature::CORRUPT) {
    tile->bgColor = getColor(colors["ENV"]["CORRUPT"]);
    tile->hasBackground = true;
  }
}
// }

tile->pos = sf::Vector3f((cell->anchor.first + cell->x),
                         (cell->anchor.second + cell->y), rz);
for (auto s : tile->sprites) {
  s->setPosition(sf::Vector2f(tile->pos.x, tile->pos.y));
}
tile->cell = cell;

return tile;
*/
}

} // namespace hellfrost

// cache_count = 0;
// engine->clearCache();
// cacheThread = std::thread([&]() {
//   log.start("bg caching");
//   for (auto x = 0; x < gm->location->width; x++) {
//     for (auto y = 0; y < gm->location->height; y++) {
//       auto t = engine->cacheTile(x, y, viewport->view_z);
//       cache_count++;
//     }
//   }
//   log.stop("bg caching");
// });
