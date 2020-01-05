#include "libcolor.hpp"
#include <string>

int main() {
  printf("Color from '#123'\n");
  auto c = Color::fromHexString("#123");
  printf("  RGB: %d, %d, %d\n", c.red(), c.green(), c.blue());
  printf("  HSV: %f, %f, %f\n", c.h, c.s, c.v);
  printf("  HEX: %s\n", c.hex().c_str());

  printf("\nColor from 'pink'\n");
  c = Color::fromWebName("pink");
  printf("  RGB: %d, %d, %d\n", c.red(), c.green(), c.blue());
  printf("  HSV: %f, %f, %f\n", c.h, c.s, c.v);
  printf("  HEX: %s\n", c.hex().c_str());

  printf("\nBlend #fff color with #000 (k = 0.5)\n");
  auto w = Color::fromHexString("#ffffff");
  auto b = Color::fromHexString("#000");
  w.blend(b);
  printf("  RGB: %d, %d, %d\n", w.red(), w.green(), w.blue());
  printf("  HSV: %f, %f, %f\n", w.h, w.s, w.v);
  printf("  HEX: %s\n", w.hex().c_str());

  printf("\nBlend #fff color with #000 (k = 1)\n");
  w = Color::fromHexString("#ffffff");
  w.blend(b, 1);
  printf("  RGB: %d, %d, %d\n", w.red(), w.green(), w.blue());
  printf("  HSV: %f, %f, %f\n", w.h, w.s, w.v);
  printf("  HEX: %s\n", w.hex().c_str());

  printf("\nRotate hue from red to red\n");
  w = Color::fromHexString("#ff0000");
  printf("  RGB: %d, %d, %d\n", w.red(), w.green(), w.blue());
  printf("  HSV: %f, %f, %f\n", w.h, w.s, w.v);
  printf("  HEX: %s\n", w.hex().c_str());

  for (auto i = 0; i < 5; i++) {
    w.hue(w.hue() + 360.f / 5);
    printf("  RGB: %d, %d, %d\n", w.red(), w.green(), w.blue());
    printf("  HSV: %f, %f, %f\n", w.h, w.s, w.v);
    printf("  HEX: %s\n", w.hex().c_str());
  }
}
