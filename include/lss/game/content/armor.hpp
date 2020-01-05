#ifndef __ARMOR_H_
#define __ARMOR_H_

#include "lss/game/item.hpp"

namespace Prototype {

/* Leather armor */
const auto LEATHER_CUIRASS =
    std::make_shared<Item>("leather cuirass", "CUIRASS",
                           Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_HELMET =
    std::make_shared<Item>("leather helm", "HELMET",
                           Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_SHIELD =
    std::make_shared<Item>("leather shield", "SHIELD",
                           Effects{std::make_shared<ArmorValue>(R::I(2, 3))});
const auto LEATHER_GREAVES =
    std::make_shared<Item>("leather greaves", "GREAVES",
                           Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto LEATHER_BOOTS =
    std::make_shared<Item>("leather boots", "BOOTS",
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_RIGHT_PAULDRON =
    std::make_shared<Item>("leather right pauldron", "RIGHT_PAULDRON",
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_LEFT_PAULDRON =
    std::make_shared<Item>("leather left pauldron", "LEFT_PAULDRON",
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_RIGHT_GAUNTLET =
    std::make_shared<Item>("leather right gauntlet", "RIGHT_GAUNTLET",
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});
const auto LEATHER_LEFT_GAUNTLET =
    std::make_shared<Item>("leather left gauntlet", "LEFT_GAUNTLET",
                           Effects{std::make_shared<ArmorValue>(R::I(1, 2))});

/* Iron armor */
const auto IRON_CUIRASS =
    std::make_shared<Item>("iron cuirass", "CUIRASS",
                           Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_HELMET =
    std::make_shared<Item>("iron helm", "HELMET",
                           Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_SHIELD =
    std::make_shared<Item>("iron shield", "SHIELD",
                           Effects{std::make_shared<ArmorValue>(R::I(4, 5))});
const auto IRON_GREAVES =
    std::make_shared<Item>("iron greaves", "GREAVES",
                           Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto IRON_BOOTS =
    std::make_shared<Item>("iron boots", "BOOTS",
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_RIGHT_PAULDRON =
    std::make_shared<Item>("iron right pauldron", "RIGHT_PAULDRON",
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_LEFT_PAULDRON =
    std::make_shared<Item>("iron left pauldron", "LEFT_PAULDRON",
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_RIGHT_GAUNTLET =
    std::make_shared<Item>("iron right gauntlet", "RIGHT_GAUNTLET",
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});
const auto IRON_LEFT_GAUNTLET =
    std::make_shared<Item>("iron left gauntlet", "LEFT_GAUNTLET",
                           Effects{std::make_shared<ArmorValue>(R::I(3, 4))});

/* Steel armor */
const auto STEEL_CUIRASS =
    std::make_shared<Item>("steel cuirass", "CUIRASS",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_HELMET =
    std::make_shared<Item>("steel helm", "HELMET",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_SHIELD =
    std::make_shared<Item>("steel shield", "SHIELD",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_GREAVES =
    std::make_shared<Item>("steel greaves", "GREAVES",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 7))});
const auto STEEL_BOOTS =
    std::make_shared<Item>("steel boots", "BOOTS",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_RIGHT_PAULDRON =
    std::make_shared<Item>("steel right pauldron", "RIGHT_PAULDRON",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_LEFT_PAULDRON =
    std::make_shared<Item>("steel left pauldron", "LEFT_PAULDRON",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_RIGHT_GAUNTLET =
    std::make_shared<Item>("steel right gauntlet", "RIGHT_GAUNTLET",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
const auto STEEL_LEFT_GAUNTLET =
    std::make_shared<Item>("steel left gauntlet", "LEFT_GAUNTLET",
                           Effects{std::make_shared<ArmorValue>(R::I(5, 6))});
} // namespace Prototype

#endif // __ARMOR_H_
