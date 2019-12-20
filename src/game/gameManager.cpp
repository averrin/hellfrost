#include <lss/generator/generator.hpp>
#include <lss/gameManager.hpp>
#include "lss/components.hpp"

GameManager::GameManager(fs::path p) : path(p) {
    loadData();
    generator = std::make_unique<Generator>();
}

void GameManager::gen(LocationSpec spec) {
  srand(seed);

  registry.reset();
  location = generator->getLocation(spec);
  for (auto i = 0; i < 100; i++) {
      auto e = registry.create();
      registry.assign<hellfrost::ingame>(e);
      registry.assign<hellfrost::position>(e, rand()%100, rand()%100, 0);
      registry.assign<hellfrost::renderable>(e, "UNKNOWN", "#ff1111", false, "#11ff11");
  }
}
