#include "VietnameseQuotes.h"

#ifndef SIMULATOR
#include <esp_random.h>
#endif

#include <cstdio>
#include <iterator>

#include "../home/VietnameseHomeQuotes.h"
#include "FitText.h"
#include "ToyboxSeed.h"

namespace vietnamesequotes {
const char* next() {
  constexpr size_t count = std::size(readingdesk::vietnameseQuotes);
  static size_t previous = count;
#ifdef SIMULATOR
  const uint32_t random = toybox::seed();
#else
  const uint32_t random = esp_random();
#endif
  previous = previous < count ? (previous + 1 + random % (count - 1)) % count : random % count;
  return readingdesk::vietnameseQuotes[previous];
}

void draw(freeink::ui::DrawTarget& target, freeink::ui::Rect box, const char* quote, freeink::ui::FontId font) {
  char text[224];
  std::snprintf(text, sizeof(text), "“%s”", quote);
  freeink::ui::TextStyle style;
  style.font = font;
  style.italic = true;
  style.align = freeink::ui::TextAlign::Center;
  fittext::draw(target, box, text, style, 3);
}
}  // namespace vietnamesequotes
