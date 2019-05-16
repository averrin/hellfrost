#ifndef __DAMAGE_H_
#define __DAMAGE_H_
#include "game/damageSpec.hpp"
#include "game/object.hpp"
#include "game/randomTools.hpp"
#include "game/trait.hpp"

class Damage : public Object {
public:
  Damage(DamageSpec ds) : Object(), spec(ds) {}
  int damage = 0;
  bool isCritical = false;
  std::vector<Trait> traits;
  std::vector<Trait> defTraits;
  int deflected = 0;
  DamageSpec spec;
};

#endif // __DAMAGE_H_
