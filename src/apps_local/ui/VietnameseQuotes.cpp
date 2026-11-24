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
    const char* value=readingdesk::vietnameseQuotes[candidate];
    size_t glyphs=0;
    for(const unsigned char* p=reinterpret_cast<const unsigned char*>(value);*p;++p)
      if((*p&0xc0)!=0x80)++glyphs;
    if(candidate!=previous && glyphs<=82) {previous=candidate;return value;}
  }
  return "Đi một ngày đàng, học một sàng khôn.";
}

void drawSingle(freeink::ui::DrawTarget& target,freeink::ui::Rect box,const char* quote,freeink::ui::FontId font) {
  freeink::ui::TextStyle style;style.font=font;style.italic=true;style.align=freeink::ui::TextAlign::Center;style.maxLines=1;
  char copy[384];const char* candidate=quote;
#ifdef SIMULATOR
  const size_t start=toybox::seed()%std::size(readingdesk::vietnameseQuotes);
#else
  const size_t start=esp_random()%std::size(readingdesk::vietnameseQuotes);
#endif
  for(size_t attempt=0;attempt<=std::size(readingdesk::vietnameseQuotes);++attempt) {
    std::snprintf(copy,sizeof(copy),"“%s”",candidate);
    if(target.measureText(font,copy,style).width<=box.width&&fittext::complete(target,box,copy,style)){target.text(box,copy,style);return;}
    candidate=readingdesk::vietnameseQuotes[(start+attempt)%std::size(readingdesk::vietnameseQuotes)];
  }
  // Never truncate or replace with a four-word quote. Hide only if no full line fits.

}
void draw(freeink::ui::DrawTarget& target, freeink::ui::Rect box, const char* quote, freeink::ui::FontId font) {
  char text[384];
  std::snprintf(text, sizeof(text), "“%s”", quote);
  freeink::ui::TextStyle style;
  style.font = font;
  style.italic = true;
  style.align = freeink::ui::TextAlign::Center;
  style.maxLines=2;
  target.text(box,text,style);
}
}  // namespace vietnamesequotes
