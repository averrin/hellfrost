#ifndef __DEPS_H_
#define __DEPS_H_

#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
// #include <cereal/archives/json.hpp>
#include <string>
//TODO: migrate json to cereal
#include <json.hpp>
#include <iostream>
#include <fstream>

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

using json = nlohmann::json;

#endif // __DEPS_H_
