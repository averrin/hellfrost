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
    entt::registry prototypes;
    entt::snapshot_loader{prototypes}
        .get<entt::entity>(ar)
        .template get<hf::meta>(ar).template get<hf::visible>(ar).template get<hf::ineditor>(ar)
        .template get<hf::pickable>(ar).template get<hf::wearable>(ar).template get<hf::glow>(ar)
        .template get<hf::renderable>(ar).template get<hf::wall>(ar).template get<hf::tags>(ar)
        .template get<hf::player>(ar).template get<hf::vision>(ar).template get<hf::obstacle>(ar)
        .template get<hf::creature>(ar).template get<hf::script>(ar).template get<entt::tag<"item"_hs>>(ar)
        .template get<entt::tag<"enemy"_hs>>(ar).template get<entt::tag<"terrain"_hs>>(ar).template get<entt::tag<"proto"_hs>>(ar)
        .orphans();
  };
  template <class Archive> void save(Archive &ar) const {
    ar(probability, itemSpecs, terrainSpecs, lootBoxes, mapFeatures);
      entt::snapshot{prototypes}
          .get<entt::entity>(ar)
          .template get<hf::meta>(ar).template get<hf::visible>(ar).template get<hf::ineditor>(ar)
          .template get<hf::pickable>(ar).template get<hf::wearable>(ar).template get<hf::glow>(ar)
          .template get<hf::renderable>(ar).template get<hf::wall>(ar).template get<hf::tags>(ar)
          .template get<hf::player>(ar).template get<hf::vision>(ar).template get<hf::obstacle>(ar)
          .template get<hf::creature>(ar).template get<hf::script>(ar).template get<entt::tag<"item"_hs>>(ar)
          .template get<entt::tag<"enemy"_hs>>(ar).template get<entt::tag<"terrain"_hs>>(ar).template get<entt::tag<"proto"_hs>>(ar);
  };

public:
  std::map<std::string, float> probability;
  std::map<std::string, ItemSpec> itemSpecs;
  std::map<std::string, std::map<std::string, float>> mapFeatures;
  std::map<std::string, TerrainSpec> terrainSpecs;
  entt::registry& prototypes;
  std::vector<LootBox> lootBoxes;

  GameData(entt::registry& p) : prototypes(p) {}
  GameData(const GameData& other) : prototypes(other.prototypes) {}
};

#endif // __GAMEDATA_H_
