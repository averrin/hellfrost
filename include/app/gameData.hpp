#ifndef __GAMEDATA_H_
#define __GAMEDATA_H_
#include <lss/deps.hpp>
#include "lss/game/itemSpec.hpp"
#include <map>
#include <vector>

class GameData {
    friend class cereal::access;
    // template<class Archive>
    // void serialize(Archive & ar) {
    //     ar( probability, itemSpecs );
    // };
    template<class Archive>
    void load(Archive & ar) {
        ar( probability, itemSpecs );
    };
    template<class Archive>
    void save(Archive & ar) const {
        ar( probability, itemSpecs );
    };

public:
    std::map<std::string, float> probability;
    std::vector<ItemSpec> itemSpecs;
};

#endif // __GAMEDATA_H_
