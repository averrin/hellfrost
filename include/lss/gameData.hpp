#ifndef __GAMEDATA_H_
#define __GAMEDATA_H_
#include <lss/components.hpp>
#include <lss/deps.hpp>
#include <lss/game/itemSpec.hpp>
#include <lss/game/lootBox.hpp>
#include <lss/game/terrain.hpp>
#include <map>
#include <vector>

namespace hf = hellfrost;

class GameData {
  friend class cereal::access;
  template <class Archive> void load(Archive &ar) {
    ar(probability, itemSpecs, terrainSpecs, lootBoxes, mapFeatures);
    prototypes = std::make_shared<entt::registry>();
    prototypes->loader().entities(ar);
    prototypes->loader()
        .component<hf::meta, hf::visible, hf::ineditor, hf::pickable,
                   hf::wearable, hf::glow, hf::renderable, hf::wall, hf::tags,
                   hf::player, hf::vision, hf::obstacle,
      hf::creature,
      entt::tag<"item"_hs>,
                   entt::tag<"enemy"_hs>, entt::tag<"terrain"_hs>,
                   entt::tag<"proto"_hs>>(ar);
  };
  template <class Archive> void save(Archive &ar) const {
    ar(probability, itemSpecs, terrainSpecs, lootBoxes, mapFeatures);
    prototypes->snapshot().entities(ar);
    prototypes->snapshot()
        .component<hf::meta, hf::visible, hf::ineditor, hf::pickable,
                   hf::wearable, hf::glow, hf::renderable, hf::wall, hf::tags,
                   hf::player, hf::vision, hf::obstacle,
      hf::creature, entt::tag<"item"_hs>,
                   entt::tag<"enemy"_hs>, entt::tag<"terrain"_hs>,
                   entt::tag<"proto"_hs>>(ar);
  };

public:
  std::map<std::string, float> probability;
  std::map<std::string, ItemSpec> itemSpecs;
  std::map<std::string, std::map<std::string, float>> mapFeatures;
  std::map<std::string, TerrainSpec> terrainSpecs;
  std::shared_ptr<entt::registry> prototypes;
  std::vector<LootBox> lootBoxes;
};

#endif // __GAMEDATA_H_
