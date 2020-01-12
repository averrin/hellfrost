#include <lss/game/region.hpp>

void Region::update() {
  const std::lock_guard<std::mutex> lock(mutex);
  auto _x = 0;
  auto _y = 0;
  for (auto r : *cells) {
    for (auto cell : r) {
      cell->x = _x + x;
      cell->y = _y + y;
      x++;
    }
    y++;
  }
}

void Region::place(std::shared_ptr<Region> region, int _x, int _y) {
  fmt::print("place @ {}.{}\n", _x, _y);
  regions.push_back(region);
  region->x = _x;
  region->y = _y;
  region->update();
  invalidate();
}

bool Region::placeInside(std::shared_ptr<Region> region, bool smart) {
  auto start = getRandomCell();
  auto _x = start->x;
  auto _y = start->y;

  if (smart) {
    if (region->width > width - 2 || region->height > height - 2) {
      return false;
    }
    if (_x + region->width > width - 2)
      _x -= _x + region->width - width - 2;
    if (_y + region->height > height - 2)
      _y -= _y + region->height - height - 2;
  }

  place(region, _x, _y);
  return true;
}

std::optional<std::shared_ptr<Cell>> Region::getCell(int _x, int _y, int _z) {
  std::optional<std::shared_ptr<Cell>> cell = std::nullopt;
  if (!isInside(_x, _y, _z))
    return cell;

  auto _cells = getCells();
  if (_cells->size() == 0) {
    fmt::print("WTF?! cells is empty 0_o\n");
    return cell;
  }
  cell = _cells->at(_y - y)[_x - x];
  return cell;
}

std::shared_ptr<Cells> Region::getCells() {
  if (!damaged) {
    return cache;
  }
  return getCells(z);
}

bool Region::isInside(int _x, int _y, int _z) {
  if (_z != z)
    return false;
  // fmt::print("x < _x && _x < x + width) && (y < _y && _y < y + height) ==
  // {}\n", (x < _x && _x < x + width) && (y < _y && _y < y + height));
  // fmt::print("{} < {} && {} < {}) && ({} < {} && {} < {}\n", x, _x, _x, x +
  // width, y, _y, _y, y + height);
  return (x < _x && _x < x + width - 1) && (y < _y && _y < y + height - 1);
}

void Region::updateCell(int _x, int _y, int _z, CellSpec type,
                        std::vector<CellFeature> features) {
  for (auto region : regions) {
    if (region->isInside(_x, _y, _z)) {
      // FIXME: overlapping regions
      return updateCell(_x, _y, _z, type, features);
    }
  }
  if (isInside(_x, _y, _z)) {
    const std::lock_guard<std::mutex> lock(resize_mutex);
    if (_y - y >= cells->size() || _x - x >= cells->front().size()) {
      fmt::print("WTF?! inside check: {}\n", isInside(_x, _y, _z));
      return;
    }
    auto c = cells->at(_y - y)[_x - x];
    mapUtils::updateCell(c, type, features);
    c->x = _x;
    c->y = _y;
    invalidate();
  }
}

std::shared_ptr<Cells> Region::getCells(int _z) {
  const std::lock_guard<std::mutex> lock(resize_mutex);
  // fmt::print("get cells without cache\n");
  auto result = cells;
  int rbx = width;
  int rby = height;
  for (auto r : regions) {
    rbx = std::max(rbx, r->x + r->width);
    rby = std::max(rby, r->y + r->height);
  }
  if (width != rbx || height != rby) {
    result = std::make_shared<Cells>(resize(*result, rbx, rby));
    width = rbx;
    height = rby;
  }
  for (auto r : regions) {
    if (!r->active)
      continue;
    if (r->z != _z)
      continue;
    result =
        std::make_shared<Cells>(paste(*r->getCells(), r->x, r->y, *result));
  }
  cache = result;
  damaged = false;
  return cache;
}
std::optional<std::shared_ptr<Cell>> Region::getRandomCell(CellSpec type) {
  auto _cells = getCells();
  std::vector<std::shared_ptr<Cell>> filtered;
  for (auto r : *_cells) {
    for (auto c : r) {
      if (c->type == type) {
        filtered.push_back(c);
      }
    }
  }
  if (filtered.empty()) {
    return std::nullopt;
  }
  return filtered[rand() % filtered.size()];
}

void Region::resize(int w, int h, CellSpec type) {
  const std::lock_guard<std::mutex> lock(mutex);
  auto c = resize(*cells, w, h, type);
  cells = std::make_shared<Cells>(c);
  width = w;
  height = h;
  damaged = true;
}
Cells Region::resize(Cells src, int w, int h, CellSpec type) {
  auto dst = Cells{};
  for (int _y = 0; _y < h; _y++) {
    if (dst.size() <= _y) {
      dst.push_back(std::vector<std::shared_ptr<Cell>>{});
    }
    for (int _x = 0; _x < w; _x++) {
      if (src.size() <= _y || src[_y].size() <= _x) {
        auto cell = std::make_shared<Cell>(_x, _y, type);
        cell->anchor = {x, y};
        dst[_y].push_back(cell);
      } else {
        dst[_y].push_back(src[_y][_x]);
      }
    }
  }
  return dst;
}

Cells Region::paste(Cells src, int x, int y, Cells dst) {
  int bh = dst.size();
  int bw = bh > 0 ? dst.front().size() : 0;
  for (auto r = y; r < int(src.size() + y) && r < bh; r++) {
    for (auto c = x; c < int(src.front().size() + x) && c < bw; c++) {
      auto cell = src[r - y][c - x];
      if (cell->type == CellType::EMPTY)
        continue;
      dst[r][c] = cell;
      dst[r][c]->x = c;
      dst[r][c]->y = r;
    }
  }
  damaged = true;
  return dst;
}

void Region::fill(int h, int w, CellSpec type) {
  // cells->clear();
  // for (auto r = 0; r < h; r++) {
  //   std::vector<std::shared_ptr<Cell>> row;
  //   for (auto c = 0; c < w; c++) {
  //     auto cell = std::make_shared<Cell>(c, r, type);
  //     cell->visibilityState = VisibilityState::UNKNOWN;
  //     cell->anchor = {x, y};
  //     row.push_back(cell);
  //   }

  //   cells->push_back(row);
  // }
  // damaged = true;
}

void Region::walls(CellSpec type) {
  // TODO: add walls as region
  // auto i = 0;
  // for (auto row : *cells) {
  //   if (i == 0 || i == cells->size() - 1) {
  //     for (auto c : row) {
  //       mapUtils::updateCell(c, type);
  //     }
  //   } else {
  //     auto n = 0;
  //     for (auto c : row) {
  //       if (n == 0 || n == row.size() - 1) {
  //         mapUtils::updateCell(c, type);
  //       }
  //       n++;
  //     }
  //   }
  //   i++;
  // }
  // damaged = true;
}

void Region::flatten() {
  cells = getCells();
  auto _x = 0;
  auto _y = 0;
  for (auto r : *cells) {
    _x = 0;
    for (auto cell : r) {
      cell->x = _x + x;
      cell->y = _y + y;
      _x++;
    }
    _y++;
  }
  regions.clear();
}

int Region::countNeighbors(std::shared_ptr<Cell> cell,
                           std::function<bool(std::shared_ptr<Cell>)> test) {
  auto c = 0;
  auto _cells = getCells();
  auto _x = cell->x - x;
  auto _y = cell->y - y;
  // fmt::print("{}.{} -> {}.{}\n", cell->x, cell->y, _x, _y);
  if (_x > 0) {
    if (_y > 0) {
      if (test(_cells->at(_y - 1)[_x - 1]))
        c++;
      if (test(_cells->at(_y - 1)[_x]))
        c++;
      if (test(_cells->at(_y)[_x - 1]))
        c++;

      if (_x < width - 1) {
        if (test(_cells->at(_y - 1)[_x + 1]))
          c++;
        if (test(_cells->at(_y)[_x + 1]))
          c++;
      }
    }
    if (_y < height - 1) {
      if (_x < width - 1) {
        if (test(_cells->at(_y + 1)[_x + 1]))
          c++;
        if (test(_cells->at(_y + 1)[_x - 1]))
          c++;
      }
      if (test(_cells->at(_y + 1)[_x]))
        c++;
    }
  }
  return c;
}

std::vector<std::shared_ptr<Cell>>
Region::getNeighbors(std::shared_ptr<Cell> cell) {
  auto _cells = getCells();
  std::vector<std::shared_ptr<Cell>> nbrs;
  if (cell->x > 0) {
    if (cell->y > 0) {
      nbrs.push_back(_cells->at(cell->y - 1)[cell->x - 1]);
      nbrs.push_back(_cells->at(cell->y - 1)[cell->x]);
      nbrs.push_back(_cells->at(cell->y)[cell->x - 1]);

      if (cell->x < width - 1) {
        nbrs.push_back(_cells->at(cell->y - 1)[cell->x + 1]);
        nbrs.push_back(_cells->at(cell->y)[cell->x + 1]);
      }
    }
    if (cell->y < height - 1) {
      if (cell->x < width - 1) {
        nbrs.push_back(_cells->at(cell->y + 1)[cell->x + 1]);
        nbrs.push_back(_cells->at(cell->y + 1)[cell->x - 1]);
      }
      nbrs.push_back(_cells->at(cell->y + 1)[cell->x]);
    }
  }
  return nbrs;
}

std::optional<std::shared_ptr<Cell>> Region::getCell(std::shared_ptr<Cell> cc,
                                                     Direction d) {
  auto _cells = getCells();
  std::optional<std::shared_ptr<Cell>> cell;
  switch (d) {
  case N:
    if (cc->y == 0)
      break;
    cell = _cells->at(cc->y - 1)[cc->x];
    break;
  case E:
    if (cc->x == _cells->front().size())
      break;
    cell = _cells->at(cc->y)[cc->x + 1];
    break;
  case S:
    if (cc->y == _cells->size())
      break;
    cell = _cells->at(cc->y + 1)[cc->x];
    break;
  case W:
    if (cc->x == 0)
      break;
    cell = _cells->at(cc->y)[cc->x - 1];
    break;
  case NW:
    if (cc->y == 0)
      break;
    if (cc->x == 0)
      break;
    cell = _cells->at(cc->y - 1)[cc->x - 1];
    break;
  case NE:
    if (cc->y == 0)
      break;
    if (cc->x == _cells->front().size())
      break;
    cell = _cells->at(cc->y - 1)[cc->x + 1];
    break;
  case SW:
    if (cc->y == _cells->size())
      break;
    if (cc->x == 0)
      break;
    cell = _cells->at(cc->y + 1)[cc->x - 1];
    break;
  case SE:
    if (cc->y == _cells->size())
      break;
    if (cc->x == _cells->front().size())
      break;
    cell = _cells->at(cc->y + 1)[cc->x + 1];
    break;
  }
  return cell;
}
