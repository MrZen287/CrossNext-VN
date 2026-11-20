#include "LunarSleepCalendar.h"

#include <I18n.h>

#include <algorithm>
#include <cstdio>

#include "LunarCoverDigits.h"
#include "apps_local/reading/ReadingStatsStore.h"
#include "apps_local/tools/CalendarLabels.h"
#include "apps_local/tools/VietnameseLunar.h"
#include "apps_local/ui/BookFrame.h"
#include "apps_local/ui/NumberTypography.h"
#include "apps_local/ui/PixelMenuTheme.h"
#include "apps_local/ui/VietnameseQuotes.h"

namespace lunarSleepCalendar {
namespace fui = freeink::ui;
namespace {
constexpr StrId weekdays[] = {
    StrId::STR_STATS_MONDAY, StrId::STR_STATS_TUESDAY, StrId::STR_STATS_WEDNESDAY, StrId::STR_STATS_THURSDAY,
    StrId::STR_STATS_FRIDAY, StrId::STR_STATS_SATURDAY, StrId::STR_STATS_SUNDAY};
constexpr const char* shortWeekdays[] = {"T2", "T3", "T4", "T5", "T6", "T7", "CN"};
constexpr auto ink = fui::Paint::solid(fui::Color::Black);

void divider(fui::DrawTarget& target, fui::Rect page, int y) {
  const int mid = page.x + page.width / 2;
  const auto pt = [y](int x, int dy = 0) {
    return fui::Point{static_cast<int16_t>(x), static_cast<int16_t>(y + dy)};
  };
  target.line(pt(page.x + 20), pt(mid - 13), 1, ink);
  target.line(pt(mid + 13), pt(page.right() - 21), 1, ink);
  target.triangle(pt(mid - 7), pt(mid, -5), pt(mid + 7), ink);
  target.triangle(pt(mid - 7), pt(mid, 5), pt(mid + 7), ink);
}

}  // namespace

bool draw(GfxRenderer& renderer) {
  const int today = readingstats::today();
  if (!today) return false;
  const auto date = readingstats::dateFromDay(today);
  const auto lunar = vietnameselunar::fromCivilDay(today, date.year);
  if (!lunar.day) return false;

  renderer.clearScreen();
  auto target = toybox::makeTarget(renderer);
  pixelui::bindFonts(target, renderer, I18N.getLanguage() != Language::EN);
  target.setFont(0, toybox::kSerifSmallFontId);
  target.setFont(1, toybox::kSerifTileFontId);
  target.setFont(2, toybox::kSerifTitleFontId);
  const auto area = target.deviceContext().safeRect();
  const auto paper = area.inset({10, 10, 10, 10});
  const auto inner = paper.inset({5, 5, 5, 5});
  const auto label = [&](fui::Rect rect, const char* text, int font, fui::Color color = fui::Color::Black,
                         bool emphasis = false) {
    pixelui::text(target, rect, text, font, color, fui::TextAlign::Center, emphasis);
  };
  const auto cellLabel = [&](fui::Rect box, const char* text, int font, fui::Color color, bool emphasis) {
    box = box.inset({2, 3, 2, 3});
    const auto previousClip = target.clipRect();
    target.setClipRect(box);
    label(box, text, font, color, emphasis);
    target.setClipRect(previousClip);
  };
  bookframe::draw(target, paper);
  char buffer[96];
  label(fui::makeRect(inner.x + 28, inner.y + 14, inner.width - 56, 46),
        I18N.get(weekdays[readingstats::weekday(today)]), 2);
  std::snprintf(buffer, sizeof(buffer), tr(STR_LUNAR_COVER_DATE), date.day, date.month, date.year);
  label(fui::makeRect(inner.x + 25, inner.y + 63, inner.width - 50, 44), buffer, 2);
  const int dateEnd = paper.y + paper.height * 48 / 100;
  const int quoteHeight = std::clamp(int(paper.height) / 10, 70, 80);
  const int split = dateEnd + quoteHeight;
  std::snprintf(buffer, sizeof(buffer), "%d", date.day);
  numerals::draw(target, fui::makeRect(inner.x + 45, inner.y + 111, inner.width - 90, dateEnd - inner.y - 173),
                 buffer, fui::Color::Black, coverFace);
  std::snprintf(buffer, sizeof(buffer), lunar.leap ? tr(STR_TOOLS_LUNAR_LEAP_DATE) : tr(STR_TOOLS_LUNAR_DATE),
                lunar.day, lunar.month, lunar.year);
  const char* holidayName = calendarlabels::holiday(lunar);
  label(fui::makeRect(inner.x, dateEnd - (holidayName ? 62 : 43), inner.width, 33), buffer, 2);
  if (holidayName) label(fui::makeRect(inner.x, dateEnd - 29, inner.width, 23), holidayName, 0);
  vietnamesequotes::draw(target, fui::makeRect(inner.x + 18, dateEnd, inner.width - 36, quoteHeight - 8),
                         vietnamesequotes::next(), 1);

  const int first = readingstats::dayIndex({date.year, date.month, 1});
  const int offset = readingstats::weekday(first);
  const int days = readingstats::daysInMonth(date.year, date.month);
  const int rows = (offset + days + 6) / 7;
  divider(target, inner, split);
  const auto grid = fui::makeRect(inner.x + 15, split + 15, inner.width - 30, inner.bottom() - split - 33);
  constexpr int weekdayHeight = 30;
  const int gridY = grid.y + weekdayHeight;
  const int gridHeight = grid.bottom() - gridY;
  const int weekendX = grid.x + 5 * grid.width / 7;
  for (int y = grid.y + 2; y < grid.bottom() - 1; y += 4)
    for (int x = weekendX + 2 + (y % 8 ? 0 : 2); x < grid.right() - 1; x += 4)
      renderer.drawPixel(x, y);
  target.stroke(grid, ink, 1);
  for (int col = 0; col < 7; ++col) {
    const int x = grid.x + col * grid.width / 7;
    const int right = grid.x + (col + 1) * grid.width / 7;
    cellLabel(fui::makeRect(x, grid.y, right - x, weekdayHeight), shortWeekdays[col], 0, fui::Color::Black, col >= 5);
    if (col) renderer.drawLine(x, grid.y, x, grid.bottom() - 1);
  }
  for (int row = 0; row < rows; ++row) {
    const int y = gridY + row * gridHeight / rows;
    renderer.drawLine(grid.x, y, grid.right() - 1, y);
  }
  for (int day = 1; day <= days; ++day) {
    const int n = offset + day - 1;
    const int col = n % 7, row = n / 7;
    const int x = grid.x + col * grid.width / 7, right = grid.x + (col + 1) * grid.width / 7;
    const int y = gridY + row * gridHeight / rows, bottom = gridY + (row + 1) * gridHeight / rows;
    const int half = (bottom - y) / 2;
    const bool selected = day == date.day;
    const auto color = selected ? fui::Color::White : fui::Color::Black;
    if (selected) renderer.fillRect(x + 1, y + 1, right - x - 1, bottom - y - 1);
    std::snprintf(buffer, sizeof(buffer), "%d", day);
    cellLabel(fui::makeRect(x, y, right - x, half), buffer, 0, color, selected || col >= 5);
    const auto entry = vietnameselunar::fromCivilDay(first + day - 1, date.year);
    if (entry.day == 1)
      std::snprintf(buffer, sizeof(buffer), "1/%d%s", entry.month, entry.leap ? "*" : "");
    else
      std::snprintf(buffer, sizeof(buffer), "%d", entry.day);
    cellLabel(fui::makeRect(x, y + half, right - x, bottom - y - half), buffer, 3, color, false);
  }
  return true;
}
}  // namespace lunarSleepCalendar
