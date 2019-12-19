#ifndef __GAMEMANAGER_H_
#define __GAMEMANAGER_H_
#include <entt/entt.hpp>
#include <liblog/liblog.hpp>
#include <app/gameData.hpp>
#include <lss/generator/generator.hpp>
#include <lss/game/location.hpp>

struct ingame {};

struct position {
    int x = 0;
    int y = 0;
    int z = 0;
};

struct renderable {
    std::string spriteKey = "UNKNOWN";
    std::string fgColor = "#fff";
    bool hasBg = false;
    std::string bgColor = "#fffffff00";
};

class GameManager {
    std::unique_ptr<Generator> generator;
    LibLog::Logger &log = LibLog::Logger::getInstance();
  public:
    GameManager(fs::path);
    int seed;
    entt::registry registry{};
    std::shared_ptr<GameData> data;
    std::shared_ptr<Location> location;
    fs::path path;

    void reset();
    void gen(LocationSpec);
    void setSeed(int s) {seed = s;}

    void loadData() {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        cereal::BinaryInputArchive iarchive(file);
        GameData d;
        iarchive(d);
        data = std::make_shared<GameData>(d);
    }
    void saveData() {
        std::ofstream file(path, std::ios::out | std::ios::binary);
        cereal::BinaryOutputArchive oarchive(file);
        oarchive(*data);
    }
};

#endif // __GAMEMANAGER_H_
