#include <lss/generator/generator.hpp>
#include <lss/gameManager.hpp>
#include "lss/game/content/specs.hpp"

GameManager::GameManager(fs::path p) : path(p) {
    data = std::make_shared<GameData>();
    loadData();
    data->itemSpecs = {
    ItemType::EMPTY,
    ItemType::GRASS,
    ItemType::SCROLL,
    ItemType::POTION,
    ItemType::ROCK,
    ItemType::BONES,
    ItemType::CLUTTER,
    ItemType::ENEMY,
    ItemType::PICK_AXE,
    ItemType::TWO_HAND,
    ItemType::ONE_HAND,
    ItemType::LIGHT_WEAPON,
    ItemType::RING,
    ItemType::AMULET,
    ItemType::TORCH,
    ItemType::CANDLE,
    ItemType::LANTERN,
    ItemType::GOLD_COINS,
    ItemType::PLATE,
    ItemType::CUIRASS,
    ItemType::HELMET,
    ItemType::SHIELD,
    ItemType::GREAVES,
    ItemType::BOOTS,
    ItemType::CLOAK,
    ItemType::RIGHT_PAULDRON,
    ItemType::LEFT_PAULDRON,
    ItemType::RIGHT_GAUNTLET,
    ItemType::LEFT_GAUNTLET,
    ItemType::CORPSE,
    ItemType::THROWING_KNIVES
};
    reset();
}

void GameManager::reset() {
    generator = std::make_unique<Generator>(data->probability);
}

void GameManager::gen(LocationSpec spec) {
  srand(seed);

  registry.reset();
  location = generator->getLocation(spec);
  for (auto i = 0; i < 100; i++) {
      auto e = registry.create();
      registry.assign<ingame>(e);
      registry.assign<position>(e, rand()%100, rand()%100, 0);
      registry.assign<renderable>(e, "UNKNOWN", "#ff1111", false, "#11ff11");
  }
}
