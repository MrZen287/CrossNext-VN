#include "VietnameseQuotes.h"

#ifndef SIMULATOR
#include <esp_random.h>
#endif

#include <cstdio>
#include <I18n.h>
#include <iterator>

#include "../home/VietnameseHomeQuotes.h"
#include "FitText.h"
#include "ToyboxSeed.h"

namespace vietnamesequotes {
const char* next() {
  const size_t count = std::size(readingdesk::vietnameseQuotes);
  static size_t previous = count;
#ifdef SIMULATOR
  const uint32_t random = toybox::seed();
#else
  const uint32_t random = esp_random();
#endif
  const size_t start = random % count;
  for (size_t n=0;n<count;++n) {
    const size_t candidate=(start+n)%count;
    if(candidate!=previous) {previous=candidate;return readingdesk::vietnameseQuotes[candidate];}
  }
  return "Đi một ngày đàng, học một sàng khôn.";
}

void draw(freeink::ui::DrawTarget& target, freeink::ui::Rect box, const char* quote, freeink::ui::FontId font) {
  char text[384];
  std::snprintf(text, sizeof(text), "“%s”", quote ? quote : "");
  freeink::ui::TextStyle style;
  style.font = font;
  style.italic = true;
  style.align = freeink::ui::TextAlign::Center;
  style.maxLines = 2;
  if (fittext::fit(target, box, text, style, 2)) {
    fittext::draw(target, box, text, style, 2);
    return;
  }
  target.text(box, text, style);
}
}  // namespace vietnamesequotes
