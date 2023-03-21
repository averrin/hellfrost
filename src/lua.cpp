#include "lss/gameManager.hpp"
#include <app/application.hpp>
#include <app/scriptWrappers.hpp>
#include <lss/game/cell.hpp>
#include <lss/generator/generator.hpp>
#include <lss/generator/mapUtils.hpp>
#include <lss/generator/room.hpp>
#include <random.hpp>
using Random = effolkronium::random_static;

void Application::initLuaBindings() {
  lua.set("HEIGHT", 100);
  lua.set("WIDTH", 100);
  lua.new_enum("CellType", "EMPTY", CellType::EMPTY, "UNKNOWN",
               CellType::UNKNOWN, "FLOOR", CellType::FLOOR, "GROUND",
               CellType::GROUND, "WALL", CellType::WALL, "ROOF", CellType::ROOF,
               "DOWNSTAIRS", CellType::DOWNSTAIRS, "UPSTAIRS",
               CellType::UPSTAIRS, "WATER", CellType::WATER, "VOID",
               CellType::VOID);
  lua.new_enum("CellFeature", "BLOOD", CellFeature::BLOOD, "FROST",
               CellFeature::FROST, "CORRUPT", CellFeature::CORRUPT,
               "DUNGEON", CellFeature::DUNGEON,
               "POI", CellFeature::POI,
               "WIPE", CellFeature::WIPE
               );
  lua.new_enum("LocationType", "ZERO", LocationType::ZERO, "DUNGEON",
               LocationType::DUNGEON, "CAVERN", LocationType::CAVERN,
               "EXTERIOR", LocationType::EXTERIOR, "BUILDING",
               LocationType::BUILDING);
  // lua.new_enum<LocationFeature>("LocationFeature");

  lua.new_usertype<LocationSpec>("LocationSpec", "type", &LocationSpec::getType,
                                 "setType", &LocationSpec::setType,
                                 "setGenFunc", &LocationSpec::setGenFunc);
  lua.set_function("randomLocationSpec",
                   [&](int s) { return randomLocationSpec(s); });
  lua.set_function(
      "genLocation",
      sol::overload([&](int s, LocationSpec spec) { genLocation(s, spec); },
                    [&](int s) { genLocation(s); }, [&]() { genLocation(); }));

  lua.new_usertype<Cell>("Cell", "x", &Cell::x, "y", &Cell::y, "addFeature",
                         &Cell::addFeature, "type", &Cell::type, "passThrough", &Cell::passThrough,
                         "seeThrough", &Cell::seeThrough, "features", &Cell::features);
  lua.new_usertype<Location>(
      "Location", "cells", &Location::cells, "rooms", &Location::rooms,
      "addRoom", &Location::addRoom, "tags", &Location::tags, "addEntity",
      &Location::addEntity, "placeDoor", &Location::placeDoor);
  lua.new_usertype<Room>("Room", "cells", &Room::cells, "x", &Room::x, "y",
                         &Room::y, "rotate", &Room::rotate, "rotateEntities",
                         &Room::rotateEntities, "addEntity", &Room::addEntity,
                         "getRandomCell", &Room::getRandomCell);

  auto utils = lua["utils"].get_or_create<sol::table>();
  utils.set_function("fill", &mapUtils::fill);
  utils.set_function("fixOverlapped", &Generator::fixOverlapped);
  utils.set_function("makeRiver", &Generator::makeRiver);
  utils.set_function("placeTorches", &Generator::placeTorches);
  utils.set_function("placeEnemies", &Generator::placeEnemies);
  utils.set_function("placeLoot", &Generator::placeLoot);
  utils.set_function("scatter", &Generator::scatter);
  utils.set_function(
      "updatCell",
      sol::overload(
          sol::resolve<void(std::shared_ptr<Cell>, CellSpec)>(
              &mapUtils::updateCell),
          sol::resolve<void(std::shared_ptr<Cell>, CellSpec,
                            std::vector<CellFeature>)>(&mapUtils::updateCell)));
  utils.set_function(
      "paste",
      sol::overload(
          sol::resolve<void(Cells, std::shared_ptr<Location>, int, int)>(
              &mapUtils::paste),
          sol::resolve<void(std::vector<std::shared_ptr<Cell>>,
                            std::shared_ptr<Location>, int, int)>(
              &mapUtils::paste)));
  // utils.set_function("paste", [&](Cells c, std::shared_ptr<Location> l, int
  // x, int y){ mapUtils::paste(c, l, x, y)});
  utils.set_function("makeRoom", &Room::makeRoom);
  utils.set_function("makeBlob", &Room::makeBlob);
  utils.set_function("printRoom", &Room::printRoom);
  utils.set_function("placeWalls", &Generator::placeWalls);
  utils.set_function("placeDoors", &Generator::placeDoors);
  utils.set_function("placeTemplateInRoom", &Generator::placeTemplateInRoom);

  lua.new_usertype<RoomTemplate>("RoomTemplate");
  utils.set_function(
      "makeTemplate",
      sol::overload(
          [](sol::function f, int s) {
            return std::make_shared<RoomTemplate>(
                [f](std::shared_ptr<Location> location) { return f(location); },
                s);
          },
          [](sol::function f) {
            return std::make_shared<RoomTemplate>(
                [f](std::shared_ptr<Location> location) {
                  return f(location);
                });
          }));
  lua.new_usertype<Feature>("Feature");
  utils.set_function(
      "makeFeature",
      sol::overload(
          [](sol::function f, int s) {
            return std::make_shared<Feature>(
                [f](std::shared_ptr<Location> location) { f(location); }, s);
          },
          [](sol::function f) {
            return std::make_shared<Feature>(
                [f](std::shared_ptr<Location> location) { f(location); });
          }));

  lua.set("templates", &gm->templates);
  lua.set("features", &gm->features);

  lua.new_usertype<GameManager>("GameManage", "size", &GameManager::size,
                                "create", &GameManager::createInLua, "getData",
                                &GameManager::getData, "getLocation", &GameManager::getLocation);
  lua.new_usertype<EntityWrapper>("Entity", "getId", &EntityWrapper::getId,
                                  "entity", &EntityWrapper::entity);
  lua.new_usertype<GameData>("GameData", "probability", &GameData::probability);
  lua.set("gm", gm);

  auto rand = lua["Random"].get_or_create<sol::table>();
  rand.set_function("seed", [&](int s) { Random::seed(s); });
  rand.set_function("int",
                    [&](int min, int max) { Random::get<int>(min, max); });
  rand.set_function(
      "bool", sol::overload([&]() { return Random::get<bool>(); },
                            [&](float p) { return Random::get<bool>(p); }));

  utils.set_function("makeTemplateFromSpec", [](sol::table spec) {
    return std::make_shared<RoomTemplate>(
        [spec](std::shared_ptr<Location> location) {
          std::vector<std::string> empty;
          std::vector<std::vector<std::string>> eempty;
          auto map = spec.get_or<std::vector<std::string>>("map", empty);
          auto maps = spec.get_or<std::vector<std::vector<std::string>>>(
              "maps", eempty);
          if (maps.size() != 0) {
            map = *Random::get(maps);
          }
          auto w = map.front().size();
          auto h = map.size();
          auto room = Room::makeRoom(h, h, w, w, spec["floor"]);

          auto cf = spec.get<std::map<char, sol::function>>("cells");
          auto _r = 0;
          auto _c = 0;
          for (auto r : map) {
            for (auto c : r) {
              auto cell = room->getCell(_c, _r);
              cf.at(c)(location, room, cell);
              _c++;
            }
            _c = 0;
            _r++;
          }

          if (spec.get<bool>("rotate")) {
            auto n = Random::get(0, 3);
            for (auto i = 0; i < n; i++) {
              room->rotateEntities(location->registry);
              room->rotate();
            }
          }
          // auto postprocess = spec.get_or<sol::function>("postprocess",
          // nullptr); if (postprocess != nullptr) {
          //                    postprocess(location, room);
          //                  }
          return room;
        });
  });

  auto folders = {"rooms", "features"};
  for (auto f : folders) {
    for (const fs::path &file : fs::directory_iterator(PATH / "scripts" / f)) {
      if (file.extension() == ".lua") {
        lua.script_file(file);
      }
    }
  }
}
