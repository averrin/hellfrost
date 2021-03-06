#ifndef __ITEMSPEC_H_
#define __ITEMSPEC_H_
#include "lss/game/wearableType.hpp"
#include <lss/deps.hpp>
#include <string>

struct ItemCategory {
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & ar) {
      ar( name );
  };
  std::string name;
  friend bool operator!=(ItemCategory &lhs, const ItemCategory &rhs) {
    return lhs.name != rhs.name;
  }
  friend bool operator==(ItemCategory &lhs, const ItemCategory &rhs) {
    return lhs.name == rhs.name;
  }
};
namespace ItemCategories {
static const ItemCategory WEAPONS = {"Weapons"};
static const ItemCategory ARMOR = {"Armor"};
static const ItemCategory LIGHT = {"Light"};
static const ItemCategory JEWELLERY = {"Jewellery"};
static const ItemCategory CONSUMABLES = {"Consumables"};
static const ItemCategory MISC = {"Misc"};
static const ItemCategory THROWABLE = {"Throwable"};
static const ItemCategory USABLE = {"Usable"};
} // namespace ItemCategories

struct ItemSpec {
    friend class cereal::access;
    template<class Archive>
    void load(Archive & ar) {
        ar( name, category, wearableType, durability, identified, sign );
    };
    template<class Archive>
    void save(Archive & ar) const {
        ar( name, category, wearableType, durability, identified, sign );
    };
public:
  std::string name = "";
  ItemCategory category;
  WearableType wearableType = WearableType::INVALID;
  int durability = -1;
  bool identified = false;
  std::string sign;

  friend bool operator<(const ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name < rhs.name;
  }
  friend bool operator==(ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name == rhs.name;
  }
};

#endif // __ITEMSPEC_H_
