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
inline freeink::ui::Rect catForCover(freeink::ui::Rect cover) {
  return freeink::ui::makeRect(cover.right()-68,cover.y-78,84,84);
}
Layout layout(freeink::ui::Rect safe,int shelfCount,bool hasBook,bool hasPrevious,bool hasNext,bool menu=false);
void draw(GfxRenderer&,Layout&,const RecentBook*,const std::vector<RecentBook>&,int,Action,const char*,unsigned pose=0);
}
