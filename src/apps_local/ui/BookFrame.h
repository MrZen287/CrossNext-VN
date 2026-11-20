#pragma once

#include <FreeInkUI.h>

namespace bookframe {
namespace fui = freeink::ui;
inline constexpr auto ink = fui::Paint::solid(fui::Color::Black);

inline void corner(fui::DrawTarget& target, int x, int y, int sx, int sy) {
  const auto point = [=](int dx, int dy) {
    return fui::Point{static_cast<int16_t>(x + sx * dx), static_cast<int16_t>(y + sy * dy)};
  };
  constexpr int arc[][2] = {{0, 16}, {1, 10}, {4, 5}, {10, 1}, {16, 0}};
  for (int i = 1; i < 5; ++i)
    target.line(point(arc[i - 1][0], arc[i - 1][1]), point(arc[i][0], arc[i][1]), 1, ink);
  const auto a = point(7, 3), b = point(11, 7), c = point(7, 11), d = point(3, 7);
  target.line(a, b, 1, ink);
  target.line(b, c, 1, ink);
  target.line(c, d, 1, ink);
  target.line(d, a, 1, ink);
}

inline void draw(fui::DrawTarget& target, fui::Rect rect) {
  target.stroke(rect, ink, 1);
  const auto inner = rect.inset({5, 5, 5, 5});
  target.stroke(inner, ink, 1);
  corner(target, inner.x, inner.y, 1, 1);
  corner(target, inner.right() - 1, inner.y, -1, 1);
  corner(target, inner.x, inner.bottom() - 1, 1, -1);
  corner(target, inner.right() - 1, inner.bottom() - 1, -1, -1);
}
}  // namespace bookframe
