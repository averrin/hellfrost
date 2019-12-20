#include <lss/generator/generator.hpp>
#include <lss/gameManager.hpp>
#include "lss/components.hpp"
#include "lss/utils.hpp"

//TODO: remove it
#include <app/ui/viewport.hpp>
#include <app/fonts/material_design_icons.h>

GameManager::GameManager(fs::path p) : path(p) {
    loadData();
    generator = std::make_unique<Generator>();
}

void GameManager::gen(LocationSpec spec) {
  srand(seed);

  registry.reset();
  // registry
  //     .on_construct<hellfrost::renderable>()
  //     .connect<&entt::registry::assign<hellfrost::position>>(registry);

  location = generator->getLocation(spec);

  auto view_map = entt::service_locator<Viewport>::get().lock();
  for (auto t : utils::castObjects<Terrain>(location->objects)) {
      auto e = registry.create();
      registry.assign<hellfrost::ingame>(e);
      registry.assign<hellfrost::meta>(e, t->type.name);
      if (t->type.name == "tree") {
        registry.assign<hellfrost::ineditor>(e, t->type.name, "Terrain", ICON_MDI_TREE);
      }
      registry.assign<hellfrost::visible>(e, t->type.sign);
      registry.assign<hellfrost::renderable>(e, t->type.sign,
                                             view_map->colors["TERRAIN"].contains(t->type.sign)
                                                ? view_map->colors["TERRAIN"][t->type.sign]
                                             : "#aa3333ff");
      registry.assign<hellfrost::position>(e, t->currentCell->x, t->currentCell->y, 0);
  }
}
