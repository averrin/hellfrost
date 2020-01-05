#ifndef __GAMEMANAGER_H_
#define __GAMEMANAGER_H_
#include <entt/entt.hpp>
#include <liblog/liblog.hpp>
#include <lss/gameData.hpp>
#include <lss/generator/generator.hpp>
#include <lss/game/location.hpp>
#include <lss/components.hpp>
#include <lss/game/itemSpec.hpp>
#include <lss/game/terrain.hpp>

class GameManager {
    std::unique_ptr<Generator> generator;
    LibLog::Logger &log = LibLog::Logger::getInstance();
  public:
    GameManager(fs::path);
    int seed;
    std::shared_ptr<entt::registry> registry = std::make_shared<entt::registry>();
    std::shared_ptr<Location> location;
    fs::path path;

    void applyData();
    void reset();
    void gen();
    void setSeed(int s) {seed = s;}

    void loadData() {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        cereal::BinaryInputArchive iarchive(file);
        GameData data;

        iarchive(data);
        entt::service_locator<GameData>::set(data);
    }
    void saveData() {
        std::ofstream file(path, std::ios::out | std::ios::binary);
        cereal::BinaryOutputArchive oarchive(file);
        auto data = entt::service_locator<GameData>::ref();
        oarchive(data);
    }
};

#endif // __GAMEMANAGER_H_
