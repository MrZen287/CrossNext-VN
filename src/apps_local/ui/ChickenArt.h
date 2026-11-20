#pragma once

#include <algorithm>

#include "ToyboxScreen.h"

namespace chickenart {
namespace fui = freeink::ui;

// Draw an original ink chicken with the same silhouette in portraits and the room.
inline void draw(fui::DrawTarget& target, fui::Rect area, unsigned stage, unsigned mood = 0, bool inverse = false) {
  const int side = std::min<int>(area.width, area.height);
  if (side < 8) return;
  const int left = area.x + (area.width - side) / 2;
  const int top = area.y + (area.height - side) / 2;
  const auto ink = fui::Paint::solid(inverse ? fui::Color::White : fui::Color::Black);
  const auto paper = fui::Paint::solid(inverse ? fui::Color::Black : fui::Color::White);
  const auto unit = [side](int n) { return n * side / 64; };
  const auto dot = [&](int x, int y, int diameter, auto paint) {
    const int d = std::max(1, unit(diameter));
    target.fill(fui::makeRect(left + unit(x), top + unit(y), d, d), paint, d / 2);
  };
  const auto line = [&](int x, int y, int endX, int endY) {
    target.line({int16_t(left + unit(x)), int16_t(top + unit(y))},
                {int16_t(left + unit(endX)), int16_t(top + unit(endY))}, std::max(1, unit(2)), ink);
  };
  const auto oval = [&](int x, int y, int w, int h) {
    const auto box = fui::makeRect(left + unit(x), top + unit(y), unit(w), unit(h));
    target.fill(box, paper, std::min<int>(box.width, box.height) / 2);
    target.stroke(box, ink, std::max(1, unit(2)), std::min<int>(box.width, box.height) / 2);
  };

  if (stage == 0) {
    oval(13, 8, 38, 48);
    line(16, 31, 23, 35);
    line(23, 35, 28, 30);
    line(28, 30, 36, 39);
    line(36, 39, 45, 33);
    line(45, 33, 50, 34);
    dot(24, 19, 3, ink);
    dot(32, 14, 2, ink);
    dot(37, 45, 3, ink);
    return;
  }

  const bool asleep = mood == 3;
  const bool grown = stage >= 3;
  const int step = asleep ? 7 : 0;
  const int head = stage == 1 ? 15 : 12;
  if (!asleep) {
    line(25, 47, 24, 57);
    line(24, 57, 30, 57);
    line(38, 47, 38, 57);
    line(38, 57, 44, 57);
  }
  if (grown) {
    oval(4, 22, 15, 26);
    oval(10, 19, 13, 27);
  }
  oval(12, 28 + step, 40, asleep ? 20 : 25);
  oval(29, head + step, 26, 26);
  line(54, head + step + 12, 61, head + step + 16);
  line(61, head + step + 16, 54, head + step + 19);
  if (asleep) {
    line(42, head + step + 14, 46, head + step + 16);
    line(46, head + step + 16, 50, head + step + 14);
  } else if (mood == 1) {
    line(42, head + 13, 46, head + 10);
    line(46, head + 10, 50, head + 13);
  } else {
    dot(44, head + 10, 5, ink);
    dot(45, head + 10, 2, paper);
  }
  if (grown) {
    dot(33, head + step - 7, 8, ink);
    dot(39, head + step - 9, 9, ink);
    dot(46, head + step - 5, 6, ink);
    if (stage == 4) dot(52, head + step + 19, 7, ink);
  } else {
    line(34, head + step + 1, 30, head + step - 4);
    line(36, head + step, 35, head + step - 5);
  }
  line(19, 40 + step, 26, 45 + step);
  line(26, 45 + step, 34, 42 + step);
  if (mood == 2) dot(59, head + step + 25, 3, ink);
}

}  // namespace chickenart

