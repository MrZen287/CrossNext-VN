#pragma once
#include <FreeInkUI.h>
#include <array>
#include <vector>
class GfxRenderer;
struct RecentBook;
namespace readingdesk {
inline constexpr int shelfSize=3;
enum Action {None=-1,Settings,Resume,Recent1,Recent2,Recent3,Previous,Next,Library,Calendar,Apps,Games,Menu,Stats,Transfer,Cat,Count};
struct Layout {
  freeink::ui::Rect status,heading,reading,cover,title,author,position,progress,shelf,quote,mascot,cat,popup,close;
  std::array<freeink::ui::Rect,Count> hits{};
  Action at(int x,int y) const;
  Action move(Action current,int direction) const;
};
Layout layout(freeink::ui::Rect safe,int shelfCount,bool hasBook,bool hasPrevious,bool hasNext,bool menu=false);
void draw(GfxRenderer&,const Layout&,const RecentBook*,const std::vector<RecentBook>&,int,Action,const char*,unsigned pose=0);
}
