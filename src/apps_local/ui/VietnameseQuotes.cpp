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
  static constexpr const char* english[] = {
    "A quiet page can open a wider world.", "Make room for one more page.",
    "Small reading moments become a rich life.", "Let curiosity choose the next chapter.",
    "Every page leaves a little light behind.", "Slow down. A good story will wait.",
    "Read a little, wonder a little more.", "Keep a book close and your mind open.",
    "One chapter today, new paths tomorrow.", "A book is a place you can carry."
  };
  const bool vi = I18N.getLanguage()==Language::VI;
  const size_t count = vi ? std::size(readingdesk::vietnameseQuotes) : std::size(english);
  static size_t previous = count;
#ifdef SIMULATOR
  const uint32_t random = toybox::seed();
#else
  const uint32_t random = esp_random();
#endif
  const size_t start = random % count;
  for (size_t n=0;n<count;++n) {
    const size_t candidate=(start+n)%count;
    const char* value=vi?readingdesk::vietnameseQuotes[candidate]:english[candidate];
    size_t glyphs=0;
    for(const unsigned char* p=reinterpret_cast<const unsigned char*>(value);*p;++p)
      if((*p&0xc0)!=0x80)++glyphs;
    if(candidate!=previous && glyphs<=82) {previous=candidate;return value;}
  }
  return vi?"Đi một ngày đàng, học một sàng khôn.":"Make room for one more page.";
}

void drawSingle(freeink::ui::DrawTarget& target,freeink::ui::Rect box,const char* quote,freeink::ui::FontId font) {
  freeink::ui::TextStyle style;style.font=font;style.italic=true;style.align=freeink::ui::TextAlign::Center;style.maxLines=1;
  char copy[384];const char* candidate=quote;
  for(int attempt=0;attempt<64;++attempt) {
    std::snprintf(copy,sizeof(copy),"“%s”",candidate);
    if(target.measureText(font,copy,style).width<=box.width&&fittext::complete(target,box,copy,style)){target.text(box,copy,style);return;}
    if(I18N.getLanguage()==Language::VI)candidate=readingdesk::vietnameseQuotes[attempt%std::size(readingdesk::vietnameseQuotes)];
    else candidate="Read a little. Dream a lot.";
  }
  candidate=I18N.getLanguage()==Language::VI?"Có chí thì nên.":"Read and grow.";
  std::snprintf(copy,sizeof(copy),"“%s”",candidate);fittext::draw(target,box,copy,style,1);
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
