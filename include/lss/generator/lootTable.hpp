#ifndef __LOOTTABLE_H_
#define __LOOTTABLE_H_
#include "lss/game/lootBox.hpp"

namespace LootTable {
const std::map<int, LootBox> DUNGEON = {
    {0, LootBox(0.08, LootBoxes::DUNGEON_0)},
    {1, LootBox(0.10, LootBoxes::DUNGEON_0)},
    {2, LootBox(0.12, LootBoxes::DUNGEON_1)},
    {3, LootBox(0.14, LootBoxes::DUNGEON_1)},
    {4, LootBox(0.16, LootBoxes::DUNGEON_2)},
    {5, LootBox(0.18, LootBoxes::DUNGEON_2)},
    {6, LootBox(0.20, LootBoxes::DUNGEON_3)},
    {7, LootBox(0.22, LootBoxes::DUNGEON_3)},
    {8, LootBox(0.24, LootBoxes::DUNGEON_4)},
    {9, LootBox(0.26, LootBoxes::DUNGEON_4)},
};
}

#endif // __LOOTTABLE_H_
