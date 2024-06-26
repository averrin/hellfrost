#ifndef __UTILS_H_
#define __UTILS_H_
#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <array>
#include <vector>

#include <fmt/format.h>
#include <liblog/liblog.hpp>

#include <lss/game/direction.hpp>

namespace ll = LibLog;
namespace lp = LibPrint;
using lu = lp::utils;
using namespace std::string_literals;

class utils {
public:

  template <typename rT, typename iT>
  static std::vector<std::shared_ptr<rT>>
  castObjects(std::vector<std::shared_ptr<iT>> input, bool unique = false) {
    std::vector<std::shared_ptr<rT>> result;
    for (auto input_object : input) {
      if (auto casted_object = std::dynamic_pointer_cast<rT>(input_object)) {
        result.push_back(casted_object);
      }
    }
    if (unique) {
      auto it = std::unique(result.begin(), result.end());
      result.resize(std::distance(result.begin(), it));
    }
    return result;
  }

  static std::optional<Direction>
  getDirectionByName(const std::string &dirString) {
    if (dirString == "n"s) {
      return Direction::N;
    } else if (dirString == "e"s) {
      return Direction::E;
    } else if (dirString == "s"s) {
      return Direction::S;
    } else if (dirString == "w"s) {
      return Direction::W;
    } else if (dirString == "nw"s) {
      return Direction::NW;
    } else if (dirString == "ne"s) {
      return Direction::NE;
    } else if (dirString == "sw"s) {
      return Direction::SW;
    } else if (dirString == "se"s) {
      return Direction::SE;
    }
    return std::nullopt;
  }

  static std::string getDirectionName(Direction dir) {
    switch (dir) {
    case N:
      return "n"s;
      break;
    case E:
      return "e"s;
      break;
    case S:
      return "s"s;
      break;
    case W:
      return "w"s;
      break;
    case NW:
      return "nw"s;
      break;
    case NE:
      return "ne"s;
      break;
    case SW:
      return "sw"s;
      break;
    case SE:
      return "se"s;
      break;
    }
    return "";
  }

  static std::string getScrollName() {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    auto length = 12;
    auto randchar = []() -> char {
      const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                             "abcdefghijklmnopqrstuvwxyz";
      const size_t max_index = (sizeof(charset) - 1);
      return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return std::string{str};
  }

  static std::string getRandomColor() {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    static std::array<std::string, 28> colors = {
        "crimson",  "scarlet",   "orange",     "yellow",  "green",
        "blue",     "indigo",    "violet",     "puce",    "mauve",
        "burgundy", "turquoise", "aquamarine", "gray",    "pink",
        "white",    "lavender",  "tan",        "brown",   "cyan",
        "black",    "clear",     "milky",      "rainbow", "murky",
        "bubbling", "sparkling", "magenta"};

    return colors[rand() % colors.size()];
  }
};

#endif // __UTILS_H_
