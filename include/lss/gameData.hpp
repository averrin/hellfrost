#ifndef __GAMEDATA_H_
#define __GAMEDATA_H_
#include <lss/deps.hpp>
#include <map>
#include <lss/game/itemSpec.hpp>
#include <lss/game/terrain.hpp>
#include <vector>

class GameData {
    friend class cereal::access;
    // template<class Archive>
    // void serialize(Archive & ar) {
    //     ar( probability, itemSpecs );
    // };
    template<class Archive>
    void load(Archive & ar) {
        ar( probability, itemSpecs, terrainSpecs );
    };
    template<class Archive>
    void save(Archive & ar) const {
        ar( probability, itemSpecs, terrainSpecs );
    };

public:
    std::map<std::string, float> probability;
    std::map<std::string, ItemSpec> itemSpecs;
    std::map<std::string, TerrainSpec> terrainSpecs;
};

#endif // __GAMEDATA_H_
