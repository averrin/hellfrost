#ifndef __DEPS_H_
#define __DEPS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <future>
#include <memory>

#include <librandom/random.hpp>
#include <liblog/liblog.hpp>

using lu = LibLog::utils;

#include <entt/entt.hpp>
#include <cereal_json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>


#include <fmt/format.h>

// #if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
// #if __has_include(<filesystem>)
// #define GHC_USE_STD_FS
// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;
// #endif
// #endif
// #ifndef GHC_USE_STD_FS
#include <filesystem.hpp>
namespace fs = ghc::filesystem;
// #endif


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
