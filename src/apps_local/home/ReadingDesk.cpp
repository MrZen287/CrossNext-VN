#include "ReadingDesk.h"

#include <GfxRenderer.h>
#include <HalClock.h>
#include <I18n.h>

#include <algorithm>
#include <cstdio>

#include "../../RecentBooksStore.h"
#include "../../util/BookProgress.h"
#include "../reading/ReadingStatsStore.h"
#include "../tools/VietnameseLunar.h"
#include "../ui/BookFrame.h"
#include "../ui/Dashboard.h"
#include "../ui/NavigationIcons.h"
#include "../ui/PixelMenuTheme.h"
#include "../ui/VietnameseQuotes.h"
#include "BookCoverPainter.h"

namespace readingdesk {
namespace fui = freeink::ui;
using fui::makeRect;
namespace {
constexpr auto ink = fui::Paint::solid(fui::Color::Black);
constexpr auto paper = fui::Paint::solid(fui::Color::White);
constexpr StrId days[] = {StrId::STR_READING_MON, StrId::STR_READING_TUE, StrId::STR_READING_WED,
                          StrId::STR_READING_THU, StrId::STR_READING_FRI, StrId::STR_READING_SAT,
                          StrId::STR_READING_SUN};

void text(fui::DrawTarget& target, fui::Rect rect, const char* value, int font = 0,
          fui::TextAlign align = fui::TextAlign::Left, bool inverse = false) {
  pixelui::text(target, rect, value, font, inverse ? fui::Color::White : fui::Color::Black, align);
}
void mark(fui::DrawTarget& target, const freeink::Icon& icon, fui::Rect area, int side, bool inverse = false) {
  const auto rect = makeRect(area.x + (area.width - side) / 2, area.y + (area.height - side) / 2, side, side);
  target.bitmap(rect, fui::bitmapFromIcon(icon), fui::BitmapMode::Contain, inverse ? paper : ink);
}
}  // namespace

Action Layout::at(int x, int y) const {
  for (int n = 0; n < Count; ++n) {
    const auto rect = hits[n];
    if (x >= rect.x && x < rect.right() && y >= rect.y && y < rect.bottom()) return static_cast<Action>(n);
  }
  return None;
}

Action Layout::move(Action current, int direction) const {
  int next = current == None ? (direction > 0 ? Count - 1 : 0) : current;
  for (int n = 0; n < Count; ++n) {
    next = (next + direction + Count) % Count;
    if (!hits[next].empty()) return static_cast<Action>(next);
  }
  return None;
}

Layout layout(fui::Rect safe, int shelfCount, bool hasBook, bool hasPrevious, bool hasNext) {
  Layout g;
  const bool wide = safe.width > safe.height;
  const int side = wide ? 20 : safe.width / 18;
  const int width = safe.width - 2 * side;
  const int x = safe.x + side, y = safe.y + 4;
  const int statusH = wide ? 40 : 47;
  const int navH = wide ? 52 : 60;
  g.status = makeRect(x, y, width, statusH);
  g.hits[Settings] = makeRect(g.status.right() - 106, y, 48, statusH);
  g.heading = makeRect(x, g.status.bottom() + 6, width, wide ? 39 : 62);
  const int navTop = safe.bottom() - navH;
  const int quoteHeight = wide ? 70 : 88;
  g.quote = makeRect(x, navTop - quoteHeight, width, quoteHeight - 8);
  const int bodyHeight = g.quote.y - g.heading.bottom();
  const int cardW = wide && shelfCount > 0 ? width * 57 / 100 : width;
  const int cardH = wide ? bodyHeight - 10 : bodyHeight * 56 / 100;
  g.reading = makeRect(x, g.heading.bottom(), cardW, cardH);
  const int inset = 16;
  const int coverW = std::min((cardW - 2 * inset) * 45 / 100, (cardH - 2 * inset) * 2 / 3);
  const int coverH = coverW * 3 / 2;
  g.cover = makeRect(x + inset, g.reading.y + (cardH - coverH) / 2, coverW, coverH);
  const int copyX = g.cover.right() + 14, copyW = g.reading.right() - copyX - inset;
  const int top = g.cover.y + std::max(0, (coverH - 213) / 2);
  const int buttonH = std::min(48, coverH / 4);
  const int controlY = std::min(g.cover.bottom() - buttonH, top + 165);
  const int titleH = std::clamp(coverH - 104, 40, 77);
  g.title = makeRect(copyX, top, copyW, titleH);
  g.author = makeRect(copyX, g.title.bottom(), copyW, 26);
  g.position = makeRect(copyX, g.author.bottom(), copyW, 27);
  g.progress = makeRect(copyX, g.position.bottom(), copyW, std::max(12, controlY - g.position.bottom()));
  if (hasBook) g.hits[Resume] = makeRect(copyX, controlY, copyW, buttonH);
  if (shelfCount > 0) {
    const int shelfX = wide ? g.reading.right() + 16 : x;
    const int shelfY = wide ? g.reading.y : g.reading.bottom() + 15;
    const int shelfH = g.quote.y - shelfY - 8;
    g.shelf = makeRect(shelfX, shelfY, x + width - shelfX, shelfH);
    const bool hasPages = hasPrevious || hasNext;
    const int coversArea = shelfH - (hasPages ? 25 : 0);
    const int columnW = (g.shelf.width - 2 * inset) / shelfSize;
    const int coverW = std::min(columnW - 10, (coversArea - 35) * 2 / 3);
    const int coverH = coverW * 3 / 2;
    for (int n = 0; n < shelfCount; ++n) {
      const int left = shelfX + inset + n * columnW + (columnW - coverW) / 2;
      g.hits[Recent1 + n] = makeRect(left, shelfY + (coversArea - coverH) / 2 - 4, coverW, coverH);
    }
    if (hasPrevious) g.hits[Previous] = makeRect(g.shelf.x + 19, g.shelf.bottom() - 38, 44, 30);
    if (hasNext) g.hits[Next] = makeRect(g.shelf.right() - 63, g.shelf.bottom() - 38, 44, 30);
  }
  for (int n = 0; n < 4; ++n) {
    const int left = safe.x + n * safe.width / 4, right = safe.x + (n + 1) * safe.width / 4;
    g.hits[Library + n] = makeRect(left, navTop, right - left, navH);
  }
  return g;
}

void draw(GfxRenderer& renderer, const Layout& g, const RecentBook* featured, const std::vector<RecentBook>& shelf,
          int selectedShelfSlot, Action focused, const char* quote) {
  renderer.clearScreen();
  auto target = toybox::makeTarget(renderer);
  pixelui::bindFonts(target, renderer);
  auto status = toybox::headerStatus(renderer);
  char shown[96]{};
  halClock.formatTime(shown, sizeof(shown), SETTINGS.clockFormat == 1);
  text(target, g.status, shown);
  const auto gear = g.hits[Settings];
  if (focused == Settings) target.stroke(gear.inset({3, 3, 3, 3}), ink, 2);
  mark(target, navicons::Gear32, gear, 24);
  if (status.percent >= 0) {
    std::snprintf(shown, sizeof(shown), "%d%%", status.percent);
    text(target, makeRect(gear.right(), g.status.y, g.status.right() - gear.right(), g.status.height), shown, 0,
         fui::TextAlign::Right);
  }
  renderer.drawLine(g.status.x, g.status.bottom(), g.status.right() - 1, g.status.bottom());
  text(target, g.heading, tr(STR_HOME_DESK_READING), 1);
  const int today = readingstats::today();
  if (today) {
    const auto date = readingstats::dateFromDay(today);
    std::snprintf(shown, sizeof(shown), tr(STR_HOME_DESK_DAY), I18N.get(days[readingstats::weekday(today)]),
                  date.day, date.month);
    const auto dateBox = makeRect(g.heading.x + g.heading.width / 2, g.heading.y,
                                  g.heading.width / 2, g.heading.height / 2);
    text(target, dateBox, shown, 0, fui::TextAlign::Right);
    const auto lunar = vietnameselunar::fromCivilDay(today, date.year);
    if (lunar.day) {
      std::snprintf(shown, sizeof(shown), tr(STR_HOME_DESK_LUNAR), lunar.day, lunar.month, lunar.leap ? "*" : "");
      text(target, makeRect(dateBox.x, dateBox.bottom(), dateBox.width, dateBox.height), shown, 0,
           fui::TextAlign::Right);
    }
  }

  bookframe::draw(target, g.reading);
  BookCoverPainter painter(renderer, target);
  if (!featured) {
    text(target, g.reading.inset({26, 18, 26, 18}), tr(STR_NO_OPEN_BOOK), 1, fui::TextAlign::Center);
  } else {
    const RecentBook& book = *featured;
    painter.paint(book, g.cover, 0, BookCoverPainter::Fit::Contain);
    text(target, g.title, book.title.c_str(), 2);
    text(target, g.author, book.author.c_str());
    const int percent = loadBookProgress(book.path);
    if (percent >= 0 && percent < 100) {
      const auto position = loadBookPosition(book.path);
      if (position.section)
        std::snprintf(shown, sizeof(shown), tr(STR_HOME_DESK_POSITION), position.section, position.page);
      else if (position.page)
        std::snprintf(shown, sizeof(shown), tr(STR_HOME_DESK_PAGE), position.page);
      else shown[0] = '\0';
      text(target, g.position, shown);
    }
    if (percent >= 0) {
      const int y = g.progress.y + g.progress.height / 2 - 2;
      const int width = std::max(0, int(g.progress.width) - 56);
      target.fill(makeRect(g.progress.x, y, width, 4), fui::Paint::dither(fui::Color::LightGray));
      target.fill(makeRect(g.progress.x, y, width * percent / 100, 4), ink);
      std::snprintf(shown, sizeof(shown), "%d%%", percent);
      text(target, g.progress, shown, 0, fui::TextAlign::Right);
    }
    const auto resume = g.hits[Resume];
    target.fill(resume, ink, 2);
    if (focused == Resume) target.stroke(resume.inset({3, 3, 3, 3}), paper, 1);
    text(target, resume.inset({4, 24, 4, 12}), tr(STR_HOME_DESK_RESUME), 0, fui::TextAlign::Center, true);
    dashboard::chevron(target, makeRect(resume.right() - 30, resume.y, 25, resume.height), true);
  }
  if (!g.shelf.empty()) {
    bookframe::draw(target, g.shelf);
    const int bottomInset = g.hits[Previous].empty() && g.hits[Next].empty() ? 16 : 41;
    const auto rule = g.shelf.inset({16, 18, int16_t(bottomInset), 18});
    renderer.drawLine(rule.x, rule.bottom(), rule.right() - 1, rule.bottom());
    for (int n = 0; n < static_cast<int>(shelf.size()); ++n) {
      const auto cover = g.hits[Recent1 + n];
      painter.paint(shelf[n], cover, 0, BookCoverPainter::Fit::Contain);
      if (selectedShelfSlot == n || focused == Recent1 + n) target.stroke(cover.inset({-3, -3, -3, -3}), ink, 2);
    }
    for (const Action page : {Previous, Next}) {
      const auto area = g.hits[page];
      if (area.empty()) continue;
      if (focused == page) target.stroke(area, ink, 1, 3);
      dashboard::chevron(target, area, false, page == Previous);
    }
  }
  vietnamesequotes::draw(target, g.quote, quote, pixelui::kBody);
  static constexpr const freeink::Icon* icons[] = {&navicons::Library32, &navicons::Calendar32, &navicons::Grid32,
                                                   &navicons::Games32};
  const auto first = g.hits[Library];
  renderer.drawLine(first.x, first.y, g.hits[Games].right() - 1, first.y);
  for (int n = 0; n < 4; ++n) {
    const auto area = g.hits[Library + n];
    if (focused == Library + n) target.stroke(area.inset({4, 5, 4, 5}), ink, 2);
    mark(target, *icons[n], area, 29);
  }
}
}  // namespace readingdesk
