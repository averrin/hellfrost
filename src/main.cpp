#if __APPLE__
    #include <libproc.h>
#endif
#include <backward.hpp>

namespace backward {
backward::SignalHandling sh;
} // namespace backward

#include <application.hpp>

std::string VERSION = "0.4.0";
std::string APP_NAME = "Hellfrost";

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

int main(int argc, char *argv[]) {
    auto seed = time(NULL);
    if (argc > 1) {
        seed = std::atoi(argv[1]);
    }
    auto path = get_selfpath();
    Application app(APP_NAME, path, VERSION, seed);
    app.log.start(APP_NAME);
    app.serve();
    app.log.stop(APP_NAME);
    return EXIT_SUCCESS;
}
