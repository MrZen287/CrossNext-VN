#pragma once

#include <charconv>
#include <optional>
#include <string_view>

#include "../reading/ReadingStatsCore.h"
#include "VietnameseLunar.h"

namespace calendarlookup {
inline std::optional<int> dayIndex(std::string_view input, bool lunar) {
  bool leap = false;
  if (lunar && input.ends_with('*')) {
    input.remove_suffix(1);
    leap = true;
  }
  int values[3]{};
  for (int index = 0; index < 3; ++index) {
    const auto separator = input.find('/');
    if ((index == 2) != (separator == std::string_view::npos)) return std::nullopt;
    const auto part = input.substr(0, separator);
    if (part.empty() || part.front() < '0' || part.front() > '9' || part.size() > (index == 2 ? 4u : 2u))
      return std::nullopt;
    const auto result = std::from_chars(part.data(), part.data() + part.size(), values[index]);
    if (result.ec != std::errc{} || result.ptr != part.data() + part.size()) return std::nullopt;
    if (index != 2) input.remove_prefix(separator + 1);
  }
  const auto [day, month, year] = values;
  if (lunar) return vietnameselunar::toCivilDay({day, month, year, leap});
  if (year < 1970 || year > 2199 || month < 1 || month > 12 || day < 1 ||
      day > readingstats::daysInMonth(year, month))
    return std::nullopt;
  return readingstats::dayIndex({year, month, day});
}
}  // namespace calendarlookup
