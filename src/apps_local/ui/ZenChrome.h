#pragma once
#include <FreeInkUI.h>
#include <cstdio>
#include "BackButton.h"
namespace zenchrome {
namespace fui=freeink::ui;
inline constexpr int height=48;
inline fui::Rect footer(const fui::DeviceContext& d) {
  const auto safe=d.safeRect();return fui::makeRect(safe.x,safe.bottom()-height,safe.width,height);
}
inline void draw(fui::DrawTarget& t,const fui::DeviceContext& d,const char* clock,int percent,bool back=true) {
  const auto b=footer(d);const auto ink=fui::Paint::solid(fui::Color::Black);
  t.fill(b,fui::Paint::solid(fui::Color::White));
  t.line({b.x,b.y},{b.right(),b.y},1,ink);
  auto hit=fui::makeRect(b.x+4,b.y+2,48,44);
  backui::publish(back&&d.hasTouch,hit);
  if(back)backui::paint(t,hit,fui::Color::Black);
  fui::TextStyle style;style.font=0;style.maxLines=1;style.align=fui::TextAlign::Center;
  if(clock&&*clock)t.text(fui::makeRect(b.x+60,b.y,b.width-120,b.height),clock,style);
  if(percent>=0){char s[12];std::snprintf(s,sizeof(s),"%d%%",percent);style.align=fui::TextAlign::Right;t.text(fui::makeRect(b.right()-74,b.y,66,b.height),s,style);}
}
}
