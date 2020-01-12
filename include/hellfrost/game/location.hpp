#ifndef __LOCATION_H_
#define __LOCATION_H_
#include <memory>

#include <hellfrost/game/region.hpp>

namespace hellfrost {
class Location: public Region {
  public:
    Location() : Region() {}
};
} // namespace hellfrost
#endif // __LOCATION_H_
