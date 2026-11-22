#pragma once

#include <FreeInkUI.h>

namespace vietnamesequotes {
const char* next();
void drawSingle(freeink::ui::DrawTarget&,freeink::ui::Rect,const char*,freeink::ui::FontId);
void draw(freeink::ui::DrawTarget& target, freeink::ui::Rect box, const char* quote, freeink::ui::FontId font);
}  // namespace vietnamesequotes
