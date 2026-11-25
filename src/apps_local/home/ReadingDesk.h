#pragma once
#include <FreeInkUI.h>
#include <array>
#include <algorithm>
#include <vector>
class GfxRenderer;
struct RecentBook;
namespace readingdesk {
inline constexpr int shelfSize=3;
enum Action {None=-1,Settings,Resume,Recent1,Recent2,Recent3,Previous,Next,Library,Calendar,Apps,Games,Menu,Stats,Transfer,Cat,Quote,Dictionary,Count};
struct Layout {
  freeink::ui::Rect status,heading,reading,cover,title,author,position,progress,remaining,shelf,quote,mascot,cat,duo,popup,close;
  std::array<freeink::ui::Rect,Count> hits{};
  Action at(int x,int y) const;
  Action move(Action current,int direction) const;
};
inline freeink::ui::Rect catForCover(freeink::ui::Rect cover) {
  const int size=std::clamp(int(cover.width)*3/5,100,150);
  return freeink::ui::makeRect(cover.right()-size*3/4,cover.y-size*3/5,size,size);
}
Layout layout(freeink::ui::Rect safe,int shelfCount,bool hasBook,bool hasPrevious,bool hasNext,bool menu=false);
// Embedded art uses panel coordinates, not bezel-safe content coordinates.
inline Layout screenLayout(const freeink::ui::DeviceContext& d,int count,bool book,bool prev,bool next,bool menu=false){
  return layout(d.width==480&&d.height==800?freeink::ui::makeRect(0,0,480,800):d.safeRect(),count,book,prev,next,menu);
}
inline freeink::ui::Rect coverFace(freeink::ui::Rect shell){return shell.inset({3,3,3,3});}
void draw(GfxRenderer&,Layout&,const RecentBook*,const std::vector<RecentBook>&,int,Action,const char*,unsigned pose=0,unsigned wormPose=0);
}
