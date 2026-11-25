#pragma once

#include <FreeInkUI.h>

namespace menucompanions {

inline constexpr int width = 96;
inline constexpr int baseline = 53;
inline uint8_t pose = 0;
#include "MenuCompanionSprites.inc"

inline void draw(freeink::ui::DrawTarget& target, int left, int lineY) {
  namespace ui = freeink::ui;
  const auto black = ui::Paint::solid(ui::Color::Black);
  const auto white = ui::Paint::solid(ui::Color::White);
  const auto& sprite = sprites[pose];
  const int top = lineY - baseline;
  target.fill(ui::makeRect(left, top, width, 62), white);
  for (int y = 0; y < 62; ++y) {
    int start = -1;
    for (int x = 0; x <= width; ++x) {
      const bool ink = x < width && (sprite[y][x / 8] & (0x80 >> (x % 8)));
      if (ink && start < 0) start = x;
      if (!ink && start >= 0) {
        target.fill(ui::makeRect(left + start, top + y, x - start, 1), black);
        start = -1;
      }
    }
  }
}

}  // namespace menucompanions
