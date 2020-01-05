#ifndef __GAMEDATA_H_
#define __GAMEDATA_H_
#include <hellfrost/deps.hpp>

#include <lss/components.hpp>
#include <lss/game/itemSpec.hpp>
#include <lss/game/lootBox.hpp>
#include <lss/game/terrain.hpp>
#include <map>
#include <vector>

namespace hellfrost {
class GameData {
  friend class cereal::access;

  template <class Archive> void load(Archive &ar) {
    ar(probability, itemSpecs, terrainSpecs, lootBoxes);
    prototypes = std::make_shared<entt::registry>();
    prototypes->loader().entities(ar);
    prototypes->loader()
        .component<meta, visible, ineditor, pickable, wearable, glow,
                   entt::tag<"item"_hs>, entt::tag<"enemy"_hs>,
                   entt::tag<"terrain"_hs>, entt::tag<"proto"_hs>>(ar);
  };

  template <class Archive> void save(Archive &ar) const {
    ar(probability, itemSpecs, terrainSpecs, lootBoxes);
    prototypes->snapshot().entities(ar);
    prototypes->snapshot()
        .component<meta, visible, ineditor, pickable, wearable, glow,
                   entt::tag<"item"_hs>, entt::tag<"enemy"_hs>,
                   entt::tag<"terrain"_hs>, entt::tag<"proto"_hs>>(ar);
  };

public:
  std::shared_ptr<entt::registry> prototypes;
  std::map<std::string, float> probability;
  std::vector<LootBox> lootBoxes;

  // TODO: migrate to prototypes
  std::map<std::string, ItemSpec> itemSpecs;
  std::map<std::string, TerrainSpec> terrainSpecs;
};
} // namespace hellfrost
#endif // __GAMEDATA_H_
