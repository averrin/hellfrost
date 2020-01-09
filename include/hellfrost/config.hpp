#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <hellfrost/deps.hpp>

namespace hellfrost {
class Config {
public:
    std::string tileset;
    std::string data_file;

  friend class cereal::access;
  template <class Archive> void serialize(Archive &ar) {
      ar(CEREAL_NVP(tileset), CEREAL_NVP(data_file));
  };
};
}
#endif // __CONFIG_H_
