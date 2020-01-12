#ifndef __GAMEMANAGER_H_
#define __GAMEMANAGER_H_

#include <hellfrost/deps.hpp>
#include <hellfrost/game/location.hpp>
#include <hellfrost/generator/generator.hpp>

#include <liblog/liblog.hpp>
using lu = LibLog::utils;

namespace hellfrost {
namespace gm {
struct generation_start {
  std::shared_ptr<Location> location;
};
struct generation_finish {
  std::shared_ptr<Location> location;
};

enum class status { GENERATING, DONE };
}; // namespace gm

class GameManager {
  std::unique_ptr<Generator> generator;
  LibLog::Logger log = LibLog::Logger(fmt::color::coral, "GM");
  fs::path path;
  std::future<void> generation;

public:
  GameManager(fs::path, int s);
  ~GameManager() {
    // entt::service_locator<GameData>::reset();
    // if (genThread.joinable()) {
    //   genThread.join();
    // }
  }
  gm::status status = gm::status::DONE;
  int seed;
  std::shared_ptr<Location> location;
  std::shared_ptr<entt::registry> registry = std::make_shared<entt::registry>();

  void applyData();
  void reset();
  void gen();
  void setSeed(int s) { seed = s; }

  void loadData() {
    // std::ifstream file(path, std::ios::in | std::ios::binary);
    // cereal::BinaryInputArchive iarchive(file);
    // GameData data;

    // iarchive(data);
    // entt::service_locator<GameData>::set(data);
  }
  void saveData() {
    // std::ofstream file(path, std::ios::out | std::ios::binary);
    // cereal::BinaryOutputArchive oarchive(file);
    // auto data = entt::service_locator<GameData>::ref();
    // oarchive(data);
  }

  void start();
  void start(int);
};
} // namespace hellfrost
#endif // __GAMEMANAGER_H_
