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
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB},
    Items{}
};

EnemySpec const SNAKE = {
    "snake", 0,
    1.5, 3, 0, 1,
    ds_1d3,
    {},
    Items{}
};

EnemySpec const VIPER = {
    "viper", 0,
    2, 3, 0, 1,
    ds_1d3,
    {},
    Items{Prototype::POISON_FANG}
};
EnemySpec const BAT = {
    "bat", 0,
    1.5, 3, 0, 1,
    ds_1d3,
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};
EnemySpec const BAT_LARGE = {
    "large bat", 1,
    1.5, 10, 0, 1,
    DamageSpec(0, 2, 3, DamageType::BASIC),
    {Traits::NIGHT_VISION, Traits::SHADOW_RUNNER, Traits::MOB, Traits::FLY},
    Items{}, {} //vampire
};

EnemySpec const ANT = {
    "giant ant", 0,
    1.5, 10, 0, 5,
    DamageSpec(0, 2, 3, DamageType::ACID),
    {Traits::MOB, Traits::FIRE_VULNERABLE},
    Items{}, {} //vampire
};

EnemySpec const ANT_QUEEN = {
    "giant ant", 0,
    1.5, 20, 0, 8,
    DamageSpec(1, 3, 3, DamageType::ACID),
    {Traits::MOB, Traits::FIRE_VULNERABLE},
    Items{}, {} //vampire
};

EnemySpec const GOBLIN_ROCKTHROWER = {
    "goblin rockthrower", 1,
    1, 5, 0, 1,
    DamageSpec(0, 1, 3, DamageType::WEAPON),
    {Traits::CAN_SWIM, Traits::SHADOW_RUNNER},
    Items{Prototype::GOBLIN_DAGGER, Prototype::ROCK->setCount(25)}
};

EnemySpec const GOBLIN = {
    "goblin", 1,
    1, 15, 0, 1,
    DamageSpec(0, 1, 3, DamageType::WEAPON),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER}
};

EnemySpec const GOBLIN_LIEUTENANT = {
    "goblin lieutenant", 1,
    1, 25, 0, 1,
    DamageSpec(1, 1, 5, DamageType::WEAPON),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::GOBLIN_DAGGER, Prototype::GOBLIN_DAGGER, Prototype::LEATHER_CUIRASS,
          Prototype::THROWING_KNIVES->setCount(3)}
};

EnemySpec const GOBLIN_ROGUE = {
    "goblin rogue", 2,
    1.5, 15, 0, 1,
    DamageSpec(1, 1, 3, DamageType::WEAPON),
    {Traits::NIGHT_VISION, Traits::DEADLY_SHADOWS, Traits::CAN_SWIM, Traits::SHADOW_RUNNER, Traits::MOB, Traits::DUAL_WIELD},
    Items{Prototype::POISON_DAGGER, Prototype::GOBLIN_DAGGER, Prototype::THROWING_AXES->setCount(5)}
};


EnemySpec const CULTIST = {
    "cultist", 2,
    1, 50, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    {}, Items{Prototype::CANDLE, Prototype::POISON_DAGGER}
};

EnemySpec const ORK = {
    "ork", 2,
    1, 55, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const ORK_BERSERK = {
    "ork berserk", 3,
    1.5, 40, 0, 0,
    DamageSpec(1, 3, 8, DamageType::WEAPON),
    {Traits::FIRE_VULNERABLE, Traits::BLOOD_THIRST}, Items{}
};

EnemySpec const ORK_BLACK = {
    "black ork", 3,
    1.1, 75, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    {}, Items{Prototype::TORCH, Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD, Prototype::LEATHER_CUIRASS}
};

EnemySpec const PIXI = {
    "pixi", 3,
    3, 25, 30, 0,
    DamageSpec(0, 3, 4, DamageType::WEAPON),
    {Traits::FLY, Traits::MOB}};

EnemySpec const OGRE = {
    "ogre", 4,
    1, 100, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    {Traits::NIGHT_VISION, Traits::FIRE_VULNERABLE},
    Items{Prototype::GREAT_AXE},
};

EnemySpec const I_OGRE = {
    "loooong ogre", 4,
    1, 1000000, 0, 2,
    DamageSpec(0, 0, 0, DamageType::WEAPON),
    {},
    Items{},
};



EnemySpec const SKELETON = {
    "skeleton", 3,
    1, 55, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    {Traits::WEAPON_RESIST, Traits::MAGIC_VULNERABLE}, Items{Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const ZOMBIE = {
    "zombie", 3,
    0.5, 75, 0, 2,
    DamageSpec(1, 1, 6, DamageType::WEAPON),
    {Traits::WEAPON_RESIST}, Items{Prototype::ORK_SWORD, Prototype::LEATHER_SHIELD}
};

EnemySpec const WRAITH = {
    "wraith", 4,
    1.5, 45, 0, 2,
    DamageSpec(1, 3, 6, DamageType::MAGIC),
    {Traits::WEAPON_IMMUNE}, Items{Prototype::WRAITH_TOUCH}
};

//TODO: add glow
EnemySpec const FIRE_ELEMENTAL = {
    "fire elemental", 5,
    1, 80, 0, 4,
    DamageSpec(1, 3, 6, DamageType::FIRE),
    {Traits::FIRE_IMMUNE, Traits::FROST_VULNERABLE}, Items{Prototype::LANTERN}
};

EnemySpec const FROST_ELEMENTAL = {
    "frost elemental", 5,
    1, 80, 0, 4,
    DamageSpec(1, 3, 6, DamageType::FROST),
    {Traits::FROST_IMMUNE, Traits::FIRE_VULNERABLE}, Items{}
};

EnemySpec const ACID_ELEMENTAL = {
    "acid elemental", 5,
    1, 80, 0, 4,
    DamageSpec(1, 3, 6, DamageType::ACID),
    {Traits::ACID_IMMUNE, Traits::MAGIC_VULNERABLE}, Items{}
};

EnemySpec const STONE_GOLEM = {
    "stone golem", 6,
    0.7, 100, 0, 10,
    DamageSpec(3, 3, 6, DamageType::WEAPON),
    {}, Items{}
};

EnemySpec const STEEL_GOLEM = {
    "steel golem", 6,
    0.7, 80, 0, 12,
    DamageSpec(3, 4, 8, DamageType::WEAPON),
    {}, Items{}
};

EnemySpec const FIRE_DRAGON = {
    "fire DRAGON", 7,
    1, 150, 0, 8,
    DamageSpec(5, 5, 8, DamageType::FIRE),
    {Traits::FIRE_IMMUNE, Traits::FROST_VULNERABLE}, Items{}
};

EnemySpec const FROST_DRAGON = {
    "frost dragon", 7,
    1, 150, 0, 8,
    DamageSpec(5, 5, 8, DamageType::FROST),
    {Traits::FROST_IMMUNE, Traits::FIRE_VULNERABLE}, Items{}
};

EnemySpec const ACID_DRAGON = {
    "acid dragon", 7,
    1, 150, 0, 8,
    DamageSpec(5, 5, 8, DamageType::ACID),
    {Traits::ACID_IMMUNE, Traits::MAGIC_VULNERABLE}, Items{}
};
const std::vector<EnemySpec> ALL = {
    RAT,
    SNAKE, VIPER,
    BAT, BAT_LARGE,
    ANT, ANT_QUEEN,
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
