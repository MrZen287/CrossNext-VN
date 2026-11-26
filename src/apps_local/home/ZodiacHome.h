#pragma once

#include <GfxRenderer.h>
#include <FreeInkUI.h>
#include "ZodiacMasks.h"
#include "HomeTheme.h"
#include "ReadingDesk.h"

namespace zodiacart {
namespace fui = freeink::ui;
static_assert(sizeof(mainArt)/sizeof(*mainArt) == hometheme::zodiacCount);
static_assert(sizeof(cloudArt)/sizeof(*cloudArt) == hometheme::zodiacCount);

inline void clearOldMascots(GfxRenderer& renderer, fui::DrawTarget& target, const readingdesk::Layout& layout) {
  const auto white = fui::Paint::solid(fui::Color::White);
  // Clear the artwork baked into the original Home background and reconstruct the top plane.
  target.fill(fui::makeRect(0, 0, 270, 190), white);
  target.fill(fui::makeRect(0, 190, 38, 133), white);
  target.fill(fui::makeRect(39, 190, 233, 26), white);
  const int topX[] = {38, 263, 277, 59};
  const int topY[] = {187, 166, 182, 198};
  renderer.fillPolygon(topX, topY, 4, false);
  renderer.drawLine(38,187,263,166);
  renderer.drawLine(263,166,277,182);
  renderer.drawLine(38,187,59,198);
  renderer.drawLine(59,198,277,182);
  renderer.drawLine(38,187,38,549);
  renderer.drawLine(59,198,59,543);
  target.fill(fui::makeRect(299, 418, 155, 65), white);
  target.fill(layout.mascot, white);
}

inline void draw(fui::DrawTarget& target, const readingdesk::Layout& layout, unsigned index, unsigned wormPose) {
  index %= hometheme::zodiacCount;
  zenart::draw(target, fui::makeRect(0, 0, 288, 520), *mainArt[index]);
  zenart::draw(target, fui::makeRect(303, 410, 151, 73), *cloudArt[index]);
  const auto area = layout.mascot.inset({5, 3, 5, 0});
  if (wormPose == 2) zenart::wormRest(target, area);
  else zenart::wormHappy(target, wormPose == 1 ? area.inset({2, 0, 2, 6}) : area);
}
}  // namespace zodiacart
