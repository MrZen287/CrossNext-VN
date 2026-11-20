#include "VietnameseLunar.h"

#include <cstdint>

#include "../reading/ReadingStatsCore.h"

namespace vietnameselunar {
namespace {
constexpr int firstYear = 1969;
constexpr uint32_t years[] = {
#include "VietnameseLunarYears.inc"
};
static_assert(sizeof(years) / sizeof(years[0]) == 2199 - firstYear + 1);

int monthLength(uint32_t code, int month, bool leap) {
  const int bit = leap ? 16 : 16 - month;
  return 29 + ((code >> bit) & 1);
}
}  // namespace

Date fromCivilDay(int dayIndex, int gregorianYear) {
  if (gregorianYear < 1970 || gregorianYear > 2199) return {};

  int year = gregorianYear;
  uint32_t code = years[year - firstYear];
  const int dayOfYear = dayIndex - readingstats::dayIndex({year, 1, 1});
  if (dayOfYear < 0 || dayOfYear >= 337 + readingstats::daysInMonth(year, 2)) return {};
  int days = dayOfYear - static_cast<int>(code >> 17);
  if (days < 0) {
    // The date precedes Tet. January 1 of the current and previous civil
    // years are 365 or 366 days apart, even at the 1970 epoch boundary.
    --year;
    code = years[year - firstYear];
    days = dayOfYear + 337 + readingstats::daysInMonth(year, 2) - static_cast<int>(code >> 17);
  }

  const int leapMonth = code & 0xf;
  for (int month = 1; month <= 12; ++month) {
    const int length = monthLength(code, month, false);
    if (days < length) return {days + 1, month, year, false};
    days -= length;
    if (month == leapMonth) {
      const int leapLength = monthLength(code, month, true);
      if (days < leapLength) return {days + 1, month, year, true};
      days -= leapLength;
    }
  }
  return {};
}

std::optional<int> toCivilDay(Date date) {
  if (date.year < firstYear || date.year > 2199 || date.month < 1 || date.month > 12 || date.day < 1)
    return std::nullopt;
  const uint32_t code = years[date.year - firstYear];
  const int leapMonth = code & 0xf;
  if ((date.leap && date.month != leapMonth) || date.day > monthLength(code, date.month, date.leap))
    return std::nullopt;
  int day = date.year == firstYear ? -365 : readingstats::dayIndex({date.year, 1, 1});
  day += static_cast<int>(code >> 17) + date.day - 1;
  for (int month = 1; month < date.month; ++month) {
    day += monthLength(code, month, false);
    if (month == leapMonth) day += monthLength(code, month, true);
  }
  if (date.leap) day += monthLength(code, date.month, false);
  if (day < 0 || day > readingstats::dayIndex({2199, 12, 31})) return std::nullopt;
  return day;
}
}  // namespace vietnameselunar
