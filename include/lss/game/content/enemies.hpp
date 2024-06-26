#ifndef __ENEMIES_H_
#define __ENEMIES_H_
#include "lss/game/content/items.hpp"
#include "lss/game/content/traits.hpp"
#include "lss/game/enemy.hpp"

const auto ds_1d3 = DamageSpec(0, 1, 3, DamageType::BASIC);

// clang-format off
namespace EnemyType {
EnemySpec const RAT = {
    "rat", 0,
    2, 3, 0, 1,
    ds_1d3,
    LootBoxes::ZERO,
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB},
    Items{}
};

EnemySpec const SNAKE = {
    "snake", 0,
    1.5, 3, 0, 1,
    ds_1d3,
    LootBoxes::ZERO,
    {},
    Items{}
};

EnemySpec const VIPER = {
    "viper", 0,
    2, 3, 0, 1,
    ds_1d3,
    LootBoxes::ZERO,
    {},
    Items{Prototype::POISON_FANG}
};
EnemySpec const BAT = {
    "bat", 0,
    1.5, 3, 0, 1,
    ds_1d3,
    LootBoxes::ZERO,
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};
EnemySpec const BAT_LARGE = {
    "large bat", 1,
    1.5, 10, 0, 1,
    DamageSpec(0, 2, 3, DamageType::BASIC),
    LootBoxes::ZERO,
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};

EnemySpec const ANT = {
    "giant ant", 0,
    1.5, 10, 0, 5,
    DamageSpec(0, 2, 3, DamageType::ACID),
    LootBoxes::ZERO,
    {Traits::MOB, Traits::FIRE_VULNERABLE},
    Items{}, {} //vampire
};

EnemySpec const ANT_QUEEN = {
    "giant ant", 0,
    1.5, 20, 0, 8,
    DamageSpec(1, 3, 3, DamageType::ACID),
    LootBoxes::ZERO,
    {Traits::MOB, Traits::FIRE_VULNERABLE},
    Items{}, {} //vampire
};

EnemySpec const GOBLIN_ROCKTHROWER = {
    "goblin rockthrower", 1,
    1, 5, 0, 1,
    DamageSpec(0, 1, 3, DamageType::WEAPON),
    LootBox("rockthrower", 1, {}, {
        LootBox(0.40, LootBoxes::POTIONS),
        LootBox("g10", 0.90, {Prototype::ROCK->setCount(10)})}, false),
    {Traits::CAN_SWIM, Traits::SHADOW_RUNNER},
    Items{Prototype::GOBLIN_DAGGER, Prototype::ROCK->setCount(25)}
};

EnemySpec const GOBLIN = {
    "goblin", 1,
    1, 15, 0, 1,
    DamageSpec(0, 1, 3, DamageType::WEAPON),
    LootBox("goblin", 1, {}, {
        LootBox(0.40, LootBoxes::POTIONS),
        LootBox(0.40, LootBoxes::LOOT_TIER_0),
        LootBox("g50", 0.90, {Prototype::GOLD->setCount(50)})}, false),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER}
};

EnemySpec const GOBLIN_LIEUTENANT = {
    "goblin lieutenant", 1,
    1, 25, 0, 1,
    DamageSpec(1, 1, 5, DamageType::WEAPON),
    LootBox("lieutenent", 1, {}, {
        LootBox("heal_potion_40", 0.40, {Prototype::POTION_HEAL}),
        LootBox(0.20, LootBoxes::POTIONS),
        LootBox(0.40, LootBoxes::SCROLLS),
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox("g50", 0.90, {Prototype::GOLD->setCount(50)})
    }, false),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER, Prototype::LEATHER_CUIRASS,
          Prototype::THROWING_KNIVES->setCount(3)}
};

EnemySpec const GOBLIN_ROGUE = {
    "goblin rogue", 2,
    1.5, 15, 0, 1,
    DamageSpec(1, 1, 3, DamageType::WEAPON),
    LootBox("rogue", 1, {}, {
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox("g50", 0.90, {Prototype::GOLD->setCount(50)})
    }, false),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::POISON_DAGGER, Prototype::GOBLIN_DAGGER, Prototype::THROWING_AXES->setCount(5)}
};


EnemySpec const CULTIST = {
    "cultist", 2,
    1, 50, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox("cultist", 1, {Prototype::CANDLE, Prototype::POISON_DAGGER}, {
        LootBox("potion_heal_40", 0.40, {Prototype::POTION_HEAL}),
        LootBox(0.40, LootBoxes::SCROLLS),
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox("g100", 0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {}, Items{Prototype::CANDLE, Prototype::POISON_DAGGER}
};

EnemySpec const ORK = {
    "ork", 2,
    1, 55, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox("ork", 1, {Prototype::TORCH}, {
        LootBox("potion_heal_40", 0.40, {Prototype::POTION_HEAL}),
        LootBox(0.40, LootBoxes::SCROLLS),
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox("g100", 0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const ORK_BERSERK = {
    "ork berserk", 3,
    1.5, 40, 0, 0,
    DamageSpec(1, 3, 8, DamageType::WEAPON),
    LootBox("berserk", 1, {}, {
        LootBox("potion_heal_40", 0.40, {Prototype::POTION_HEAL}),
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox("a1", 0.10, Prototype::ARTIFACTS_1),
        LootBox("g100", 0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {Traits::FIRE_VULNERABLE, Traits::BLOOD_THIRST}, Items{}
};

EnemySpec const ORK_BLACK = {
    "black ork", 3,
    1.1, 75, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox("bork", 1, {Prototype::TORCH}, {
        LootBox("potion_heal_50", 0.50, {Prototype::POTION_HEAL}),
        LootBox(0.20, LootBoxes::POTIONS),
        LootBox(0.80, LootBoxes::LOOT_TIER_2),
        LootBox("g100", 0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD, Prototype::LEATHER_CUIRASS}
};

EnemySpec const PIXI = {
    "pixi", 3,
    3, 25, 30, 0,
    DamageSpec(0, 3, 4, DamageType::WEAPON),
    LootBox("pixi", 0.90, {Prototype::GOLD->setCount(200)}, {
        LootBox(0.40, LootBoxes::SCROLLS),
        LootBox("speed_ring", 0.10, {Prototype::SPEED_RING})}, false),
    {Traits::FLY, Traits::MOB}};

EnemySpec const OGRE = {
    "ogre", 4,
    1, 100, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox("ogre", 0.80, {Prototype::GREAT_AXE}),
    {Traits::NIGHT_VISION, Traits::FIRE_VULNERABLE},
    Items{Prototype::GREAT_AXE},
};

EnemySpec const I_OGRE = {
    "loooong ogre", 4,
    1, 1000000, 0, 2,
    DamageSpec(0, 0, 0, DamageType::WEAPON),
    LootBoxes::ZERO,
    {},
    Items{},
};



EnemySpec const SKELETON = {
    "skeleton", 3,
    1, 55, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox("skeleton", 1, {}, {
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox("g100", 0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {Traits::WEAPON_RESIST, Traits::MAGIC_VULNERABLE}, Items{Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const ZOMBIE = {
    "zombie", 3,
    0.5, 75, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    LootBox("zombie", 1, {}, {
        LootBox(0.80, LootBoxes::LOOT_TIER_1),
        LootBox("g100", 0.90, {Prototype::GOLD->setCount(100)})
    }, false),
    {Traits::WEAPON_RESIST}, Items{Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const WRAITH = {
    "wraith", 4,
    1.5, 45, 0, 2,
    DamageSpec(1, 3, 6, DamageType::MAGIC),
    LootBox(),
    {Traits::WEAPON_IMMUNE}, Items{Prototype::WRAITH_TOUCH}
};

//TODO: add glow
EnemySpec const FIRE_ELEMENTAL = {
    "fire elemental", 5,
    1, 80, 0, 4,
    DamageSpec(1, 3, 6, DamageType::FIRE),
    LootBox(),
    {Traits::FIRE_IMMUNE, Traits::FROST_VULNERABLE}, Items{Prototype::LANTERN}
};

EnemySpec const FROST_ELEMENTAL = {
    "frost elemental", 5,
    1, 80, 0, 4,
    DamageSpec(1, 3, 6, DamageType::FROST),
    LootBox(),
    {Traits::FROST_IMMUNE, Traits::FIRE_VULNERABLE}, Items{}
};

EnemySpec const ACID_ELEMENTAL = {
    "acid elemental", 5,
    1, 80, 0, 4,
    DamageSpec(1, 3, 6, DamageType::ACID),
    LootBox(),
    {Traits::ACID_IMMUNE, Traits::MAGIC_VULNERABLE}, Items{}
};

EnemySpec const STONE_GOLEM = {
    "stone golem", 6,
    0.7, 100, 0, 10,
    DamageSpec(3, 3, 6, DamageType::WEAPON),
    LootBox(),
    {}, Items{}
};

EnemySpec const STEEL_GOLEM = {
    "steel golem", 6,
    0.7, 80, 0, 12,
    DamageSpec(3, 4, 8, DamageType::WEAPON),
    LootBox(),
    {}, Items{}
};

EnemySpec const FIRE_DRAGON = {
    "fire DRAGON", 7,
    1, 150, 0, 8,
    DamageSpec(5, 5, 8, DamageType::FIRE),
    LootBox(),
    {Traits::FIRE_IMMUNE, Traits::FROST_VULNERABLE}, Items{}
};

EnemySpec const FROST_DRAGON = {
    "frost dragon", 7,
    1, 150, 0, 8,
    DamageSpec(5, 5, 8, DamageType::FROST),
    LootBox(),
    {Traits::FROST_IMMUNE, Traits::FIRE_VULNERABLE}, Items{}
};

EnemySpec const ACID_DRAGON = {
    "acid dragon", 7,
    1, 150, 0, 8,
    DamageSpec(5, 5, 8, DamageType::ACID),
    LootBox(),
    {Traits::ACID_IMMUNE, Traits::MAGIC_VULNERABLE}, Items{}
};
const std::vector<EnemySpec> ALL = {
    RAT,
    SNAKE, VIPER,
    BAT, BAT_LARGE,
    ANT, ANT_QUEEN,
    CULTIST,
    GOBLIN_ROCKTHROWER,
    GOBLIN, GOBLIN_ROGUE, GOBLIN_LIEUTENANT,
    ORK, ORK_BLACK, ORK_BERSERK,
    PIXI,
    OGRE,
    I_OGRE,

    SKELETON, ZOMBIE, WRAITH,
    FIRE_ELEMENTAL, FROST_ELEMENTAL, ACID_ELEMENTAL,
    STONE_GOLEM, STEEL_GOLEM,
    
    FIRE_DRAGON, FROST_DRAGON, ACID_DRAGON,
};
} // namespace EnemyType
// clang-format on

#endif // __ENEMIES_H_
