#include <fstream>
#include <unistd.h>
#include <limits.h>
#if __APPLE__
#include <libproc.h>
#endif

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

fs::path get_selfpath() {
#ifdef _WIN32
  char buff[MAX_PATH];
  GetModuleFileName(NULL, buff, sizeof(buff));
#elif __APPLE__
  char buff[PROC_PIDPATHINFO_MAXSIZE];
  fmt::print("PID: {}\n", getpid());
  proc_pidpath(getpid(), buff, sizeof(buff));
#else
#include <linux/limits.h>
  char buff[PATH_MAX];
  ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
  if (len != -1) {
    buff[len] = '\0';
  }
#endif
  auto path = fs::path(buff);
  return path.parent_path();
}
