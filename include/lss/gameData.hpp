#ifndef __GAMEDATA_H_
#define __GAMEDATA_H_
#include <lss/components.hpp>
#include <lss/game/itemSpec.hpp>
#include <lss/game/lootBox.hpp>
#include <lss/game/terrain.hpp>
#include <map>
#include <vector>

namespace hf = hellfrost;

// template<typename Type>
// void copy(entt::sparse_set& base, entt::registry &to) {
//     auto &src = static_cast<entt::storage_for_t<Type> &>(base);
//     to.insert<Type>(src.entt::sparse_set::begin(), src.entt::sparse_set::end(), src.begin());
// }

// void register_meta() {
//     using namespace entt::literals;
//     entt::meta<Type>().func<&copy<Type>>("copy"_hs);
//     // ...
// }

// void copy_all(entt::registry &from, entt::registry &to) {
//     for (auto [id, storage] : from.storage()) {
//         using namespace entt::literals;
//         entt::resolve(storage.type()).invoke("copy"_hs, {}, entt::forward_as_meta(storage), entt::forward_as_meta(to));
//     }
// }

class GameData {
  friend class cereal::access;
  template <class Archive> void load(Archive &ar) {
    ar(probability, itemSpecs, terrainSpecs, lootBoxes, mapFeatures);
    fmt::print("Map features loaded: {}\n", mapFeatures.size());
    fmt::print("Item specs loaded: {}\n", itemSpecs.size());
    fmt::print("Terrain specs loaded: {}\n", terrainSpecs.size());
    fmt::print("Loot boxes loaded: {}\n", lootBoxes.size());
    fmt::print("Probabilities loaded: {}\n", probability.size());
    auto &registry = entt::locator<entt::registry>::value();
    entt::snapshot_loader{registry}
        .get<entt::entity>(ar)
        .template get<hf::meta>(ar)
      .template get<hf::visible>(ar)
      .template get<hf::ineditor>(ar)
        .template get<hf::pickable>(ar).template get<hf::wearable>(ar).template get<hf::glow>(ar)
        .template get<hf::renderable>(ar).template get<hf::wall>(ar).template get<hf::tags>(ar)
        .template get<hf::player>(ar).template get<hf::vision>(ar).template get<hf::obstacle>(ar)
        .template get<hf::creature>(ar).template get<hf::script>(ar).template get<entt::tag<"item"_hs>>(ar)
        // .template get<entt::tag<"enemy"_hs>>(ar).template get<entt::tag<"terrain"_hs>>(ar)
      .template get<entt::tag<"proto"_hs>>(ar)
        .orphans();
    // for(auto entity: registry.view<entt::entity>()) {
    //   registry.emplace<entt::tag<"proto"_hs>>(entity);
    // }
  };
  template <class Archive> void save(Archive &ar) const {
    ar(probability, itemSpecs, terrainSpecs, lootBoxes, mapFeatures);
    auto &registry = entt::locator<entt::registry>::value();
    auto view = registry.view<entt::tag<"proto"_hs>,hf::meta>();
    entt::snapshot{registry}
        .get<entt::entity>(ar)
        .template get<hf::meta>(ar, view.begin(), view.end()).template get<hf::visible>(ar, view.begin(), view.end()).template get<hf::ineditor>(ar, view.begin(), view.end())
        .template get<hf::pickable>(ar, view.begin(), view.end()).template get<hf::wearable>(ar, view.begin(), view.end()).template get<hf::glow>(ar, view.begin(), view.end())
        .template get<hf::renderable>(ar, view.begin(), view.end()).template get<hf::wall>(ar, view.begin(), view.end()).template get<hf::tags>(ar, view.begin(), view.end())
        .template get<hf::player>(ar, view.begin(), view.end()).template get<hf::vision>(ar, view.begin(), view.end()).template get<hf::obstacle>(ar, view.begin(), view.end())
        .template get<hf::creature>(ar, view.begin(), view.end()).template get<hf::script>(ar, view.begin(), view.end()).template get<entt::tag<"item"_hs>>(ar, view.begin(), view.end())
        // .template get<entt::tag<"enemy"_hs>>(ar, view.begin(), view.end()).template get<entt::tag<"terrain"_hs>>(ar, view.begin(), view.end())
      .template get<entt::tag<"proto"_hs>>(ar, view.begin(), view.end());
    ;
  };

public:
  std::map<std::string, float> probability;
  std::map<std::string, ItemSpec> itemSpecs;
  std::map<std::string, std::map<std::string, float>> mapFeatures;
  std::map<std::string, TerrainSpec> terrainSpecs;
  std::vector<LootBox> lootBoxes;

  GameData() {}
  GameData(const GameData& other) {
    probability = other.probability;
    itemSpecs = other.itemSpecs;
    terrainSpecs = other.terrainSpecs;
    lootBoxes = other.lootBoxes;
    mapFeatures = other.mapFeatures;
  }
};

#endif // __GAMEDATA_H_
