#ifndef __POTIONS_H_
#define __POTIONS_H_
// #include "lss/game/content/spells.hpp"
#include "lss/game/item.hpp"

namespace Prototype {
// TODO: make colors unique
const auto POTION_HEAL_LESSER = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "lesser heal potion",
    "POTION"/*, Spells::HEAL_LESSER*/);
const auto POTION_HEAL = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "heal potion",
    "POTION"/*, Spells::HEAL*/);

const auto POTION_MANA = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "restore mana potion",
    "POTION"/*, Spells::RESTORE_MANA*/);

const auto POTION_GOD_SPEED = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "god speed potion",
    "POTION"/*, Spells::GOD_SPEED*/);

const auto POTION_HP_BOOST = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "health boost potion",
    "POTION"/*, Spells::HP_BOOST*/);

const auto POTION_VISIBILITY_BOOST = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()),
    "visibility boost potion", "POTION"/*, Spells::VISIBILITY_BOOST*/);

const auto POTION_CRIT_BOOST = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()),
    "crit chance boost potion", "POTION"/*, Spells::CRIT_BOOST*/);

const auto POTION_LEVITATION = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "levitation potion",
    "POTION"/*, Spells::LEVITATION*/);

const auto POTION_REGENERATION = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "regeneration potion",
    "POTION"/*, Spells::EOT_HEAL*/);

const auto POTION_POISON = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "poison",
    "POTION"/*, Spells::EOT_POISON*/);

const auto POTION_CONFUSION = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "potion of confusion",
    "POTION"/*, Spells::CONFUSION*/);

const auto POTION_INTELLIGENCE_BOOST = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()),
    "intelligence boost potion", "POTION"/*, Spells::INTELLIGENCE_BOOST*/);

const auto POTION_STRENGTH_BOOST = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "strength boost potion",
    "POTION"/*, Spells::STRENGTH_BOOST*/);

const auto POTION_INVULNERABILITY = std::make_shared<Item>(
    fmt::format("{} potion", utils::getRandomColor()), "invulnerability potion",
    "POTION"/*, Spells::INVULNERABILITY*/);

} // namespace Prototype

#endif // __POTIONS_H_
