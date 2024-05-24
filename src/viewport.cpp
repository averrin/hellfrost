#include "lss/game/cell.hpp"
#include <app/ui/viewport.hpp>

std::string Viewport::getColorString(std::string cat, std::string key) {
  auto color = getColor(cat, key);
  auto _c = LibColor::Color(color.r, color.g, color.b, color.a);
  return _c.hexA();
}
sf::Color Viewport::getColor(std::string cat, std::string key) {
  if (cat == "") {
    throw "cat is empty";
  }
  std::string c = "#eeeeeeff";
  if (tileSet.colorVariations.find(key) != tileSet.colorVariations.end()) {
    key = *Random::get(tileSet.colorVariations[key]);
  }
  if (colors[cat].contains(key)) {
    c = colors[cat][key];
  } else {
    fmt::print("ERROR: missed color: {}.{}\n", cat, key);
  }
  auto color = getColor(c);
  if (tileSet.colorWandering.find(key) != tileSet.colorWandering.end()) {
    auto w = tileSet.colorWandering[key];
    auto keys = {"hue", "saturation", "value", "red", "green", "blue", "alpha"};
    auto _c = LibColor::Color(color.r, color.g, color.b, color.a);
    for (std::string k : keys) {
      if (w.find(k) != w.end()) {
        auto range = w[k];
        auto n = Random::get<float>(range[0], range[1]);
        // fmt::print("{}.{} wandering {} = {}\n", cat, key, k, n);
        if (k == "value") {
          _c.value(_c.value() + n);
        } else if (k == "hue") {
          _c.hue(_c.hue() + n);
        } else if (k == "saturation") {
          _c.saturation(_c.saturation() + n);
        } else if (k == "red") {
          _c.red(_c.red() + n);
        } else if (k == "green") {
          _c.green(_c.green() + n);
        } else if (k == "blue") {
          _c.blue(_c.blue() + n);
        } else if (k == "alpha") {
          _c.alpha(_c.alpha() + n);
        }
      }
    }

    color = sf::Color(_c.r, _c.g, _c.b, _c.a);
  }
  return color;
}

TileSpec Viewport::getWallSpec(std::shared_ptr<Cell> cell) {
  auto coords = getCoords(cell);
  auto [_l, _z1] = getCell(coords.first - 1, coords.second, view_z);
  auto [_r, _z2] = getCell(coords.first + 1, coords.second, view_z);
  auto [_t, _z3] = getCell(coords.first, coords.second - 1, view_z);
  auto [_b, _z4] = getCell(coords.first, coords.second + 1, view_z);

  // auto [_tl, _z10] = getCell(coords.first - 1, coords.second - 1, view_z);
  // auto [_tr, _z20] = getCell(coords.first + 1, coords.second - 1, view_z);
  // auto [_bl, _z30] = getCell(coords.first - 1, coords.second + 1, view_z);
  // auto [_br, _z40] = getCell(coords.first + 1, coords.second + 1, view_z);

  // auto spec = tileSet.sprites["WALL_H"];
  auto spec = tileSet.sprites["WALL"];
  if (_r && _l &&
      ((*_r)->type == CellType::WALL && (*_l)->type != CellType::WALL)) {
    spec = tileSet.sprites["WALL_HRS"];
  } else if (_r && _l &&
             ((*_r)->type != CellType::WALL && (*_l)->type == CellType::WALL)) {
    spec = tileSet.sprites["WALL_HLS"];
  }
  if (_t && _b &&
      ((*_t)->type == CellType::WALL && (*_b)->type == CellType::WALL)) {
    spec = tileSet.sprites["WALL_V"];
  } else if (_t && _b &&
             ((*_t)->type == CellType::WALL && (*_b)->type != CellType::WALL)) {
    spec = tileSet.sprites["WALL_VBS"];
  } else if (_t && _b &&
             ((*_t)->type != CellType::WALL && (*_b)->type == CellType::WALL)) {
    spec = tileSet.sprites["WALL_VTS"];
  }

  if (_t && _b && (*_t)->type == CellType::WALL &&
      (*_b)->type == CellType::WALL && _l && _r &&
      (*_l)->type == CellType::WALL && (*_r)->type == CellType::WALL) {
    spec = tileSet.sprites["WALL_CROSS"];
    return spec;
  }
  if (_t && _b && (*_t)->type != CellType::WALL &&
      (*_b)->type != CellType::WALL && _l && _r &&
      (*_l)->type != CellType::WALL && (*_r)->type != CellType::WALL) {
    spec = tileSet.sprites["WALL"];
    return spec;
  }

  if (_l && _r && (*_l)->type == CellType::WALL &&
      (*_r)->type == CellType::WALL) {
    spec = tileSet.sprites["WALL_H"];
    if (_b && (*_b)->type == CellType::WALL
        // &&
        // (!_bl || (*_bl)->type != CellType::WALL) &&
        // (!_tl || (*_tl)->type != CellType::WALL)
    ) {
      spec = tileSet.sprites["WALL_HB"];
    }
    if (_t && (*_t)->type == CellType::WALL
        // &&
        // (!_bl || (*_bl)->type != CellType::WALL) &&
        // (!_tl || (*_tl)->type != CellType::WALL)
    ) {
      spec = tileSet.sprites["WALL_HT"];
    }
    return spec;
  }
  if (_t && _b && (*_t)->type == CellType::WALL &&
      (*_b)->type == CellType::WALL) {
    spec = tileSet.sprites["WALL_V"];
    if (_l && (*_l)->type == CellType::WALL
        // &&
        // (!_tl || (*_tl)->type != CellType::WALL)
    ) {
      spec = tileSet.sprites["WALL_VL"];
    }
    if (_r && (*_r)->type == CellType::WALL
        // &&
        // (!_tr || (*_tr)->type != CellType::WALL)
    ) {
      spec = tileSet.sprites["WALL_VR"];
    }
    return spec;
  }

  if (_l && _b && (*_l)->type == CellType::WALL &&
      (*_b)->type == CellType::WALL) {
    spec = tileSet.sprites["WALL_LB"];
  } else if (_r && _b && (*_r)->type == CellType::WALL &&
             (*_b)->type == CellType::WALL) {
    spec = tileSet.sprites["WALL_RB"];
  } else if (_r && _t && (*_r)->type == CellType::WALL &&
             (*_t)->type == CellType::WALL) {
    spec = tileSet.sprites["WALL_RT"];
  } else if (_l && _t && (*_l)->type == CellType::WALL &&
             (*_t)->type == CellType::WALL) {
    spec = tileSet.sprites["WALL_LT"];
  }
  return spec;
}
