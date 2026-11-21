#pragma once
#include <FreeInkUI.h>
#include <algorithm>
#include "CompanionSprites.h"
namespace companionart {
namespace fui=freeink::ui;
inline void draw(fui::DrawTarget& t,fui::Rect area,const Sprite& sprite) {
  if(area.empty())return;
  const int w=std::min(int(area.width),int(area.height)*sprite.width/sprite.height);
  const int h=w*sprite.height/sprite.width;
  const int left=area.x+(area.width-w)/2,top=area.bottom()-h;
  const auto ink=fui::Paint::solid(fui::Color::Black);
  t.fill(fui::makeRect(left,top,w,h),fui::Paint::solid(fui::Color::White));
  const int stride=(sprite.width+7)/8;
  for(int y=0;y<h;++y) {
    const int sy=y*sprite.height/h;
    int start=-1;
    for(int x=0;x<=w;++x) {
      const int sx=x<w?x*sprite.width/w:0;
      const bool black=x<w&&(sprite.bits[sy*stride+sx/8]&(0x80>>(sx%8)));
      if(black&&start<0)start=x;
      if(!black&&start>=0){t.fill(fui::makeRect(left+start,top+y,x-start,1),ink);start=-1;}
    }
  }
}
inline void cat(fui::DrawTarget& t,fui::Rect area,unsigned pose){draw(t,area,Cats[pose%5]);}
inline void worm(fui::DrawTarget& t,fui::Rect area){draw(t,area,Worm);}
inline void logo(fui::DrawTarget& t,fui::Rect area){draw(t,area,Logo);}
}
