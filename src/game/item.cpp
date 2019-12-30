#include "lss/game/item.hpp"
#include "lss/utils.hpp"
#include "lss/gameData.hpp"

bool Item::interact(std::shared_ptr<Object> actor) { return false; }

std::string Item::getFullTitle(bool force) {
  return fmt::format("{}{}", count <= 0 ? "" : fmt::format("{} ", count),
                     getTitle(force));
}

std::string Item::getTitle(bool force) {
  std::vector<std::string> effectNames;
  std::vector<std::string> specialPostfix;
  std::vector<std::string> specialPrefix;
  for (auto e : effects) {
    if (e->special) {
      specialPostfix.push_back(e->getTitle());
    } else {
      effectNames.push_back(e->getTitle());
    }
  }
  return fmt::format(
      "{}{}{}{}", (identified || force) ? name : unidName,
      specialPostfix.size() <= 0
          ? ""
          : fmt::format(" {}", lu::join(specialPostfix, " ")),
      effectNames.size() == 0 || (!identified && !force)
          ? ""
          : fmt::format(" {{{}}}", lu::join(effectNames, ", ")),
      durability == -1
          ? ""
          : fmt::format(" <{}>", durability != 0
                                           ? fmt::format("{}", durability)
                                           : "broken"));
}

Item::Item(std::string un, std::string n, std::string t, Effects e)
    : Object(), unidName(un), name(n), effects(e),typeKey(t) {
  zIndex = 1;
  // auto data = entt::service_locator<GameData>::get().lock();
  // type = data->itemSpecs[t];
  // identified = type.identified;
  // durability = type.durability;
}
Item::Item(std::string t, int c) : Object(), count(c),typeKey(t) {
  zIndex = 1;
  // auto data = entt::service_locator<GameData>::get().lock();
  // type = data->itemSpecs[t];
  // identified = type.identified;
  // durability = type.durability;
  // unidName = type.name;
  // name = type.name;
}
Item::Item(std::string n, std::string t, int c, Effects e)
    : Object(), count(c), name(n), effects(e),typeKey(t) {
  zIndex = 1;
  // auto data = entt::service_locator<GameData>::get().lock();
  // type = data->itemSpecs[t];
  // identified = type.identified;
  // durability = type.durability;
  // unidName = type.name;
}
Item::Item(std::string t, Effects e) : Object(), effects(e),typeKey(t) {
  zIndex = 1;
  // auto data = entt::service_locator<GameData>::get().lock();
  // type = data->itemSpecs[t];
  // identified = type.identified;
  // durability = type.durability;
  // unidName = type.name;
  // name = type.name;
}
Item::Item(std::string t, int c, Effects e) : Object(), effects(e), count(c),typeKey(t) {
  zIndex = 1;
  // auto data = entt::service_locator<GameData>::get().lock();
  // type = data->itemSpecs[t];
  // identified = type.identified;
  // durability = type.durability;
  // unidName = type.name;
  // name = type.name;
}
Item::Item(std::string n, std::string t, Effects e)
    : Object(), effects(e), name(n),typeKey(t) {
  zIndex = 1;
  // auto data = entt::service_locator<GameData>::get().lock();
  // type = data->itemSpecs[t];
  // identified = type.identified;
  // durability = type.durability;
  // unidName = type.name;
}
