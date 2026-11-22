#pragma once

class GfxRenderer;

namespace lunarSleepCalendar {
void footer(GfxRenderer&,bool framed=false);
bool draw(GfxRenderer& renderer);
}
