#ifndef __DEPS_H_
#define __DEPS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

#include <entt/entt.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
// #include <cereal/archives/json.hpp>
#include <fmt/format.h>

//TODO: migrate json to cereal
#include <json.hpp>
using json = nlohmann::json;

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <filesystem.hpp>
namespace fs = ghc::filesystem;
#endif


namespace hellfrost {
    struct event_emitter: entt::emitter<event_emitter> {};
    struct redraw_event {};
    struct regen_event {int seed = -2;};
    struct resize_event {};
    struct center_event {int x; int y;};
    struct damage_event {int x; int y;};
    struct exec_event {std::string code;};
    struct log_event {std::string msg;};
    struct duk_error {std::string msg;};
    struct clear_markers_event {};
    struct location_update_event {};
}

#endif // __DEPS_H_
