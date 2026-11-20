#pragma once

#include <FreeInkUI.h>

#include <array>
#include <vector>

class GfxRenderer;
struct RecentBook;

namespace readingdesk {
inline constexpr int shelfSize = 3;
enum Action { None = -1, Settings, Resume, Recent1, Recent2, Recent3, Previous, Next, Library, Calendar, Apps, Games, Count };
struct Layout {
  freeink::ui::Rect status, heading, reading, cover, title, author, position, progress, shelf, quote;
  std::array<freeink::ui::Rect, Count> hits{};
  Action at(int x, int y) const;
  Action move(Action current, int direction) const;
};
Layout layout(freeink::ui::Rect safe, int shelfCount, bool hasBook, bool hasPrevious, bool hasNext);
void draw(GfxRenderer& renderer, const Layout& layout, const RecentBook* featured, const std::vector<RecentBook>& shelf,
          int selectedShelfSlot, Action focused, const char* quote);
}  // namespace readingdesk
