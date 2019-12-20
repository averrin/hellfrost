#ifndef __COMPONENTS_H_
#define __COMPONENTS_H_
#include <string>

namespace hellfrost {
    struct ingame {};
    struct meta {
        std::string name = "";
        std::string description = "";
        std::string id = "";
    };
    struct ineditor {
        std::string name = "";
        std::string folder = "";
        std::string icon = "";
    };

    struct position {
        int x = 0;
        int y = 0;
        int z = 0;
    };


    struct visible {
        std::string sign = "";
        bool hidden = false;
        bool seeThrough = true;
        bool passThrough = true;
    };
    struct usable {};
    struct destructable {};
};

#endif // __COMPONENTS_H_
