#ifndef __LIBCOLOR_HPP_
#define __LIBCOLOR_HPP_
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

namespace LibColor {
typedef std::tuple<int, int, int> RGB;
typedef std::tuple<int, int, int, int> RGBA;

const std::map<std::string, std::string> webColorNames = {
    {"maroon", "#800000"},
    {"dark red", "#8B0000"},
    {"brown", "#A52A2A"},
    {"firebrick", "#B22222"},
    {"crimson", "#DC143C"},
    {"red", "#FF0000"},
    {"tomato", "#FF6347"},
    {"coral", "#FF7F50"},
    {"indian red", "#CD5C5C"},
    {"light coral", "#F08080"},
    {"dark salmon", "#E9967A"},
    {"salmon", "#FA8072"},
    {"light salmon", "#FFA07A"},
    {"orange red", "#FF4500"},
    {"dark orange", "#FF8C00"},
    {"orange", "#FFA500"},
    {"gold", "#FFD700"},
    {"dark golden rod", "#B8860B"},
    {"golden rod", "#DAA520"},
    {"pale golden rod", "#EEE8AA"},
    {"dark khaki", "#BDB76B"},
    {"khaki", "#F0E68C"},
    {"olive", "#808000"},
    {"yellow", "#FFFF00"},
    {"yellow green", "#9ACD32"},
    {"dark olive green", "#556B2F"},
    {"olive drab", "#6B8E23"},
    {"lawn green", "#7CFC00"},
    {"chart reuse", "#7FFF00"},
    {"green yellow", "#ADFF2F"},
    {"dark green", "#006400"},
    {"green", "#008000"},
    {"forest green", "#228B22"},
    {"lime", "#00FF00"},
    {"lime green", "#32CD32"},
    {"light green", "#90EE90"},
    {"pale green", "#98FB98"},
    {"dark sea green", "#8FBC8F"},
    {"medium spring green", "#00FA9A"},
    {"spring green", "#00FF7F"},
    {"sea green", "#2E8B57"},
    {"medium aqua marine", "#66CDAA"},
    {"medium sea green", "#3CB371"},
    {"light sea green", "#20B2AA"},
    {"dark slate gray", "#2F4F4F"},
    {"teal", "#008080"},
    {"dark cyan", "#008B8B"},
    {"aqua", "#00FFFF"},
    {"cyan", "#00FFFF"},
    {"light cyan", "#E0FFFF"},
    {"dark turquoise", "#00CED1"},
    {"turquoise", "#40E0D0"},
    {"medium turquoise", "#48D1CC"},
    {"pale turquoise", "#AFEEEE"},
    {"aqua marine", "#7FFFD4"},
    {"powder blue", "#B0E0E6"},
    {"cadet blue", "#5F9EA0"},
    {"steel blue", "#4682B4"},
    {"corn flower blue", "#6495ED"},
    {"deep sky blue", "#00BFFF"},
    {"dodger blue", "#1E90FF"},
    {"light blue", "#ADD8E6"},
    {"sky blue", "#87CEEB"},
    {"light sky blue", "#87CEFA"},
    {"midnight blue", "#191970"},
    {"navy", "#000080"},
    {"dark blue", "#00008B"},
    {"medium blue", "#0000CD"},
    {"blue", "#0000FF"},
    {"royal blue", "#4169E1"},
    {"blue violet", "#8A2BE2"},
    {"indigo", "#4B0082"},
    {"dark slate blue", "#483D8B"},
    {"slate blue", "#6A5ACD"},
    {"medium slate blue", "#7B68EE"},
    {"medium purple", "#9370DB"},
    {"dark magenta", "#8B008B"},
    {"dark violet", "#9400D3"},
    {"dark orchid", "#9932CC"},
    {"medium orchid", "#BA55D3"},
    {"purple", "#800080"},
    {"thistle", "#D8BFD8"},
    {"plum", "#DDA0DD"},
    {"violet", "#EE82EE"},
    {"magenta", "#FF00FF"},
    {"fuchsia", "#FF00FF"},
    {"orchid", "#DA70D6"},
    {"medium violet red", "#C71585"},
    {"pale violet red", "#DB7093"},
    {"deep pink", "#FF1493"},
    {"hot pink", "#FF69B4"},
    {"light pink", "#FFB6C1"},
    {"pink", "#FFC0CB"},
    {"antique white", "#FAEBD7"},
    {"beige", "#F5F5DC"},
    {"bisque", "#FFE4C4"},
    {"blanched almond", "#FFEBCD"},
    {"wheat", "#F5DEB3"},
    {"corn silk", "#FFF8DC"},
    {"lemon chiffon", "#FFFACD"},
    {"light golden rod yellow", "#FAFAD2"},
    {"light yellow", "#FFFFE0"},
    {"saddle brown", "#8B4513"},
    {"sienna", "#A0522D"},
    {"chocolate", "#D2691E"},
    {"peru", "#CD853F"},
    {"sandy brown", "#F4A460"},
    {"burly wood", "#DEB887"},
    {"tan", "#D2B48C"},
    {"rosy brown", "#BC8F8F"},
    {"moccasin", "#FFE4B5"},
    {"navajo white", "#FFDEAD"},
    {"peach puff", "#FFDAB9"},
    {"misty rose", "#FFE4E1"},
    {"lavender blush", "#FFF0F5"},
    {"linen", "#FAF0E6"},
    {"old lace", "#FDF5E6"},
    {"papaya whip", "#FFEFD5"},
    {"sea shell", "#FFF5EE"},
    {"mint cream", "#F5FFFA"},
    {"slate gray", "#708090"},
    {"light slate gray", "#778899"},
    {"light steel blue", "#B0C4DE"},
    {"lavender", "#E6E6FA"},
    {"floral white", "#FFFAF0"},
    {"alice blue", "#F0F8FF"},
    {"ghost white", "#F8F8FF"},
    {"honeydew", "#F0FFF0"},
    {"ivory", "#FFFFF0"},
    {"azure", "#F0FFFF"},
    {"snow", "#FFFAFA"},
    {"black", "#000000"},
    {"dim grey", "#696969"},
    {"grey", "#808080"},
    {"dark grey", "#A9A9A9"},
    {"light grey", "#D3D3D3"},
    {"dim gray", "#696969"},
    {"gray", "#808080"},
    {"dark gray", "#A9A9A9"},
    {"light gray", "#D3D3D3"},
    {"silver", "#C0C0C0"},
    {"gainsboro", "#DCDCDC"},
    {"white smoke", "#F5F5F5"},
    {"white", "#FFFFFF"}};

class Color {
public:
  int r = 0;
  int red() { return r; }
  Color *red(int v) {
    r = v;
    updateHSV();
    return this;
  }
  int g = 0;
  int green() { return g; }
  Color *green(int v) {
    g = v;
    updateHSV();
    return this;
  }
  int b = 0;
  int blue() { return b; }
  Color *blue(int v) {
    b = v;
    updateHSV();
    return this;
  }

  int a = 255;
  int alpha() { return a; }
  Color *alpha(int v) {
    a = v;
    updateHSV();
    return this;
  }

  double h = 0;
  double hue() { return h; }
  Color *hue(double _h) {
    h = _h;
    updateRGB();
    return this;
  }
  double s = 0;
  double saturation() { return s; }
  Color *saturation(double _s) {
    s = _s;
    updateRGB();
    return this;
  }
  double v = 0;
  double value() { return v; }
  Color *value(double _v) {
    v = _v;
    updateRGB();
    return this;
  }

  std::string hex() {
    char hexcol[16];
    snprintf(hexcol, sizeof hexcol, "#%02x%02x%02x", r, g, b);
    return std::string(hexcol);
  }

  std::string hexA() {
    char hexcol[16];
    snprintf(hexcol, sizeof hexcol, "#%02x%02x%02x%02x", r, g, b, a);
    return std::string(hexcol);
  }

  Color() {}

  Color(int _r, int _g, int _b) : r(_r), g(_g), b(_b) { updateHSV(); }

  Color(int _r, int _g, int _b, int _a) : r(_r), g(_g), b(_b), a(_a) { updateHSV(); }

  Color(double _h, double _s, double _v) : h(_h), s(_s), v(_v) { updateRGB(); }

  static RGBA parseHexString(std::string hexString) {
    if (hexString.front() != '#')
      throw std::invalid_argument("hexString");
    hexString = hexString.substr(1, hexString.size() - 1);
    if (hexString.size() == 3) {
      char hexcol[8];
      snprintf(hexcol, sizeof hexcol, "%c%c%c%c%c%c", hexString[0],
               hexString[0], hexString[1], hexString[1], hexString[2],
               hexString[2]);
      hexString = std::string(hexcol);
    }
    std::stringstream st;
    st << hexString;
    int64_t color;
    st >> std::hex >> color;
    int _r;
    int _g;
    int _b;
    int _a;
    if (hexString.size() == 6) {
      _r = (color & 0x00ff0000) >> 16;
      _g = (color & 0x0000ff00) >> 8;
      _b = (color & 0x000000ff);
      _a = 255;
    } else
    if (hexString.size() == 8) {
      _r = (color & 0xff000000) >> 24;
      _g = (color & 0x00ff0000) >> 16;
      _b = (color & 0x0000ff00) >> 8;
      _a = (color & 0x000000ff);
    }
    return std::make_tuple(_r, _g, _b, _a);
  }

  static Color fromWebName(std::string name) {
    auto hexString = webColorNames.at(name);
    return fromHexString(hexString);
  }

  static Color fromHexString(std::string hexString) {
    auto rgb = parseHexString(hexString);
    return Color(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb), std::get<3>(rgb));
  }

  static Color fromString(std::string str) {
    try {
      auto rgb = parseHexString(str);
      return Color(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb), std::get<3>(rgb));
    } catch (std::invalid_argument &ex) {
      auto hexString = webColorNames.at(str);
      return fromHexString(hexString);
    }
  }

  void blend(Color color, float k = 0.5) {
    r = r * (1 - k) + color.r * k;
    g = g * (1 - k) + color.g * k;
    b = b * (1 - k) + color.b * k;
    a = a * (1 - k) + color.a * k;

    if (r > 255)
      r = 255;
    if (g > 255)
      g = 255;
    if (b > 255)
      b = 255;
    if (a > 255)
      a = 255;
    updateHSV();
  }

  void updateHSV() {
    auto hsv = rgb2hsv({r / 255.f, g / 255.f, b / 255.f});
    h = hsv.h;
    s = hsv.s;
    v = hsv.v;
  }

  void updateRGB() {
    auto rgb = hsv2rgb({h, s, v});

    r = rgb.r * 255;
    g = rgb.g * 255;
    b = rgb.b * 255;
  }

  typedef struct {
    double r; // a fraction between 0 and 1
    double g; // a fraction between 0 and 1
    double b; // a fraction between 0 and 1
  } rgb;

  typedef struct {
    double r; // a fraction between 0 and 1
    double g; // a fraction between 0 and 1
    double b; // a fraction between 0 and 1
    double a; // a fraction between 0 and 1
  } rgba;

  typedef struct {
    double h; // angle in degrees
    double s; // a fraction between 0 and 1
    double v; // a fraction between 0 and 1
  } hsv;

  static hsv rgb2hsv(rgb in) {
    hsv out;
    double min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.v = max; // v
    delta = max - min;
    if (delta < 0.00001) {
      out.s = 0;
      out.h = 0; // undefined, maybe nan?
      return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
      out.s = (delta / max); // s
    } else {
      // if max is 0, then r = g = b = 0
      // s = 0, h is undefined
      out.s = 0.0;
      out.h = std::numeric_limits<double>::quiet_NaN(); // its now undefined
      return out;
    }
    if (in.r >= max)                 // > is bogus, just keeps compilor happy
      out.h = (in.g - in.b) / delta; // between yellow & magenta
    else if (in.g >= max)
      out.h = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
    else
      out.h = 4.0 + (in.r - in.g) / delta; // between magenta & cyan

    out.h *= 60.0; // degrees

    if (out.h < 0.0)
      out.h += 360.0;

    return out;
  }

  static rgb hsv2rgb(hsv in) {
    double hh, p, q, t, ff;
    long i;
    rgb out;

    if (in.s <= 0.0) { // < is bogus, just shuts up warnings
      out.r = in.v;
      out.g = in.v;
      out.b = in.v;
      return out;
    }
    hh = in.h;
    if (hh >= 360.0)
      hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch (i) {
    case 0:
      out.r = in.v;
      out.g = t;
      out.b = p;
      break;
    case 1:
      out.r = q;
      out.g = in.v;
      out.b = p;
      break;
    case 2:
      out.r = p;
      out.g = in.v;
      out.b = t;
      break;

    case 3:
      out.r = p;
      out.g = q;
      out.b = in.v;
      break;
    case 4:
      out.r = t;
      out.g = p;
      out.b = in.v;
      break;
    case 5:
    default:
      out.r = in.v;
      out.g = p;
      out.b = q;
      break;
    }
    return out;
  }
};
} // namespace LibColor
#endif // __LIBCOLOR_HPP_
