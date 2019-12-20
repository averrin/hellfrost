#ifndef __LIGHT_H_
#define __LIGHT_H_
#include <lss/deps.hpp>

enum class LightType { NONE, CLEAR, FIRE, MAGIC, ACID, FROST };

struct LightSpec {
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & ar) {
      ar( distance, type, stable );
  };
  float distance;
  LightType type;
  bool stable = false;
};

namespace Glow {
const LightSpec NONE = {0, LightType::NONE};
}

#endif // __LIGHT_H_
