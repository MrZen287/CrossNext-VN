#include "ToyboxFonts.h"

#include <EpdFont.h>
#include <EpdFontFamily.h>
#include <I18n.h>

#include "../../fontIds.h"
#include "PixelText.h"
#include "fonts/RobotoFonts.h"

namespace toybox {
namespace {
EpdFont sans8Regular(&sans08), sans8Bold(&sans08b);
EpdFont sans10Regular(&sans10), sans10Bold(&sans10b);
EpdFont sans12Regular(&sans12), sans12Bold(&sans12b), sans12Italic(&sans12i), sans12BoldItalic(&sans12bi);
EpdFont sans18Bold(&sans18b), sans28Bold(&sans28b);
EpdFont readingRegular(&sans13), readingBold(&sans13b), readingItalic(&sans13i);

EpdFontFamily smallSans(&sans8Regular, &sans8Bold);
EpdFontFamily labelSans(&sans10Regular, &sans10Bold);
EpdFontFamily bodySans(&sans12Regular, &sans12Bold, &sans12Italic, &sans12BoldItalic);
EpdFontFamily titleSans(&sans18Bold);
EpdFontFamily largeSans(&sans28Bold);
// Share the two oversized app-display aliases; retain the entire glyph repertoire.
EpdFontFamily hugeSans(&sans28Bold);
EpdFontFamily readingFamily(&readingRegular, &readingBold, &readingItalic);
EpdFontFamily boldLabel(&sans10Bold);
EpdFontFamily boldReading(&readingBold);

struct Binding {
  int id;
  EpdFontFamily& family;
  int fallback;
};
const Binding appFonts[] = {
    {kDisplayFontId, titleSans, UI_12_FONT_ID},
    {kUiFontId, bodySans, UI_12_FONT_ID},
    {kTileFontId, smallSans, SMALL_FONT_ID},
    {kSerifTileFontId, readingFamily, UI_12_FONT_ID},
    {kSerifTitleFontId, titleSans, UI_12_FONT_ID},
    {kSerifSmallFontId, labelSans, UI_10_FONT_ID},
    {kReadingFontId, readingFamily, UI_12_FONT_ID},
    {kReadingBoldFontId, boldReading, UI_12_FONT_ID},
    {kButtonFontId, labelSans, UI_10_FONT_ID},
    {kReadingSmallFontId, labelSans, UI_10_FONT_ID},
    {kReadingBoldSmallFontId, boldLabel, UI_10_FONT_ID},
    {kHugeFontId, hugeSans, UI_12_FONT_ID},
    {kLargeFontId, largeSans, UI_12_FONT_ID},
    {kProseFontId, bodySans, UI_12_FONT_ID},
    {kLibraryFontId, labelSans, UI_10_FONT_ID},
    {kBookListFontId, labelSans, UI_10_FONT_ID},
    {kCarouselTitleFontId, titleSans, UI_12_FONT_ID},
    {kClockLabelFontId, titleSans, UI_12_FONT_ID},
};

void syncTextCase() {
  pixelui::turkicCase = I18N.getLanguage() == Language::TR || I18N.getLanguage() == Language::AZ;
  pixelui::preserveTranslatedCase = I18N.getLanguage() != Language::EN && I18N.getLanguage() != Language::ES;
  pixelui::uppercaseLabels = false;
}
}  // namespace

void registerSystemFonts(GfxRenderer& renderer) {
  renderer.insertFont(SMALL_FONT_ID, smallSans);
  renderer.insertFont(UI_10_FONT_ID, labelSans);
  renderer.insertFont(UI_12_FONT_ID, bodySans);
}

// Use the non-pixel layout paths with the Roboto font IDs registered here.
menufont::Style interfaceFontStyle() { return menufont::Style::NotoSans; }

void syncInterfaceFonts(GfxRenderer& renderer) { ensureFonts(renderer); }

void ensureFonts(GfxRenderer& renderer) {
  syncTextCase();
  // insertFont preserves a previously registered id.
  for (const auto& binding : appFonts) {
    if (renderer.getFontMap().count(binding.id)) continue;
    renderer.insertFont(binding.id, binding.family);
    renderer.setFallbackFont(binding.id, binding.fallback);
  }
}

FontMetrics metricsFor(int fontId) {
  const EpdFontFamily* family = &bodySans;
  for (const auto& binding : appFonts) {
    if (binding.id != fontId) continue;
    family = &binding.family;
    break;
  }
  FontMetrics metrics;
  metrics.ascender = family->getData()->ascender;
  if (const auto* glyph = family->getGlyph('H')) {
    metrics.capTop = glyph->top;
    metrics.capHeight = glyph->height;
  }
  return metrics;
}

int drawCapsCentered(const GfxRenderer& renderer, int fontId, int x, int boxY, int boxH,
                     const char* text, bool black) {
  const auto metrics = metricsFor(fontId);
  const int y = boxY + (boxH - metrics.capHeight) / 2 - metrics.ascender + metrics.capTop;
  renderer.drawText(fontId, x, y, text, black);
  return x;
}
}  // namespace toybox
