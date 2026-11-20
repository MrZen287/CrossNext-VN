#pragma once
#include <optional>

namespace vietnameselunar {
struct Date {
  int day;
  int month;
  int year;
  bool leap;
};

// dayIndex is the Gregorian civil day in Vietnam (days since 1970-01-01).
Date fromCivilDay(int dayIndex, int gregorianYear);
std::optional<int> toCivilDay(Date date);
}  // namespace vietnameselunar
