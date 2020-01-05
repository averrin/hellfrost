#ifndef __SCROLLS_H_
#define __SCROLLS_H_
// #include "lss/game/content/spells.hpp"
#include "lss/game/item.hpp"

namespace Prototype {
const auto SCROLL_IDENTIFICATION = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of identification", "SCROLL"/*, Spells::IDENTIFY*/);
const auto SCROLL_REVEAL = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of reveal", "SCROLL"/*, Spells::REVEAL*/);
const auto SCROLL_TELEPORT = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of teleport", "SCROLL"/*, Spells::TELEPORT_RANDOM*/);

const auto SCROLL_WEAPON_RESIST = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of weapon resistance", "SCROLL"/*, Spells::WEAPON_RESIST*/);
const auto SCROLL_WEAPON_IMMUNE = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of weapon immunity", "SCROLL"/*, Spells::WEAPON_IMMUNE*/);
const auto SCROLL_MAGIC_RESIST = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of magic resistance", "SCROLL"/*, Spells::MAGIC_RESIST*/);
const auto SCROLL_MAGIC_IMMUNE = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of magic immunity", "SCROLL"/*, Spells::MAGIC_IMMUNE*/);

const auto SCROLL_FIRE_RESIST = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of fire resistance", "SCROLL"/*, Spells::FIRE_RESIST*/);
const auto SCROLL_FIRE_IMMUNE = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of fire immunity", "SCROLL"/*, Spells::FIRE_IMMUNE*/);
const auto SCROLL_ACID_RESIST = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of acid resistance", "SCROLL"/*, Spells::ACID_RESIST*/);
const auto SCROLL_ACID_IMMUNE = std::make_shared<Item>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of acid immunity", "SCROLL"/*, Spells::ACID_IMMUNE*/);
} // namespace Prototype

#endif // __SCROLLS_H_
