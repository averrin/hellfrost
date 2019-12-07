#ifndef __TILEMAP_H_
#define __TILEMAP_H_

#include <utility>
#include <vector>

class TileMap {
    public:
    static const std::pair<int, int> UNKNOWN;
    static const std::pair<int, int> GRASS;
    static const std::pair<int, int> FLOOR;

    static const std::pair<int, int> WALL_H;
    static const std::pair<int, int> WALL_V;

    static const std::pair<int, int> WALL_HB;
    static const std::pair<int, int> WALL_HT;
    static const std::pair<int, int> WALL_VL;
    static const std::pair<int, int> WALL_VR;

    static const std::pair<int, int> WALL_LT;
    static const std::pair<int, int> WALL_LB;
    static const std::pair<int, int> WALL_RT;
    static const std::pair<int, int> WALL_RB;

    static const std::pair<int, int> WALL_CROSS;
};

#endif // __TILEMAP_H_
