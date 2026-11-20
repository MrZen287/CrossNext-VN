#pragma once

#include <I18n.h>

#include "VietnameseLunar.h"

namespace calendarlabels {
inline const char* holiday(vietnameselunar::Date date) {
  if (date.leap) return nullptr;
  if (date.month == 1 && date.day >= 1 && date.day <= 3) return tr(STR_LUNAR_TET);
  if (date.month == 3 && date.day == 10) return tr(STR_LUNAR_HUNG_KINGS);
  if (date.month == 8 && date.day == 15) return tr(STR_LUNAR_MID_AUTUMN);
  return nullptr;
}
}  // namespace calendarlabels
