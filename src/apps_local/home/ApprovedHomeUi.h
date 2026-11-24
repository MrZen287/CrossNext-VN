#pragma once
#include <cstdio>
#include <FreeInkUIIcon.h>
#include "ApprovedHomeArt.h"
#include "ReadingDesk.h"
#include "WidgetDigits.h"
#include "../ui/FitText.h"
namespace approvedhome {
namespace fui=freeink::ui;
struct Info {
 const char* clock="",*date="",*lunar="",*title="",*author="",*chapter="",*remaining="",*estimate="",*quote="";
 int percent=-1,battery=-1;
};
inline void text(fui::DrawTarget& t,fui::Rect box,const char* s,int font=0,bool bold=false,fui::TextAlign align=fui::TextAlign::Left,int lines=1){
 fui::TextStyle st;st.font=font;st.bold=bold;st.align=align;st.maxLines=lines;fittext::draw(t,box,s,st,lines);
}
inline void overlay(fui::DrawTarget& t,const readingdesk::Layout& g,const Info& m){
 const auto ink=fui::Paint::solid(fui::Color::Black);
 int hour=0,minute=0;
 if(m.clock&&std::sscanf(m.clock,"%d:%d",&hour,&minute)==2){
  const int numbers[]={hour/10%10,hour%10,minute/10%10,minute%10};
  for(int n=0;n<4;++n)t.bitmap(fui::makeRect(300+n*32+(n>=2?10:0),35,27,43),fui::bitmapFromIcon(*widgetdigits::digits[numbers[n]]),fui::BitmapMode::Contain,ink);
  t.fill(fui::makeRect(363,48,4,4),ink,2);t.fill(fui::makeRect(363,62,4,4),ink,2);
 }

 text(t,fui::makeRect(286,79,166,24),m.date,0,false,fui::TextAlign::Center);
 text(t,fui::makeRect(286,103,166,23),m.lunar,0,false,fui::TextAlign::Center);
 text(t,g.title,m.title,6,true,fui::TextAlign::Left,2);
 text(t,g.author,m.author);text(t,g.position,m.chapter);
 const int width=g.progress.width-39,y=g.progress.y+11;
 t.stroke(fui::makeRect(g.progress.x,y,width,7),ink,1,3);
 if(m.percent>=0){t.fill(fui::makeRect(g.progress.x+2,y+2,std::max(0,(width-4)*std::clamp(m.percent,0,100)/100),3),ink,1);
 char s[12];std::snprintf(s,sizeof(s),"%d%%",m.percent);text(t,fui::makeRect(g.progress.right()-38,g.progress.y,38,26),s,3);}
 text(t,fui::makeRect(g.remaining.x,g.remaining.y,g.remaining.width,24),m.remaining);
 text(t,fui::makeRect(g.remaining.x,g.remaining.y+25,g.remaining.width,40),m.estimate,3,false,fui::TextAlign::Left,2);
 text(t,fui::makeRect(319,485,132,50),"Đọc tiếp",6,true,fui::TextAlign::Center);
 fui::TextStyle qs;qs.font=0;qs.italic=true;qs.align=fui::TextAlign::Center;qs.maxLines=1;
 fittext::draw(t,g.quote,m.quote,qs,1);
 if(m.battery>=0){char s[12];std::snprintf(s,sizeof(s),"%d%%",m.battery);text(t,fui::makeRect(389,644,64,31),s,1,true,fui::TextAlign::Center);}
}
}
