#pragma once
#include <FreeInkUI.h>
#include <FreeInkUIIcon.h>
#include <Icon.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include "../reading/ReadingStatsCore.h"
#include "../tools/VietnameseLunar.h"
#include "../ui/FitText.h"
namespace homelauncher {
namespace fui=freeink::ui;
enum Action {None,Calendar,Stats,Library,Settings,App=100};
struct Layout {
  fui::Rect safe,status,widgets,date,stats,grid,dock;
  std::array<fui::Rect,12> cells{};
  std::array<fui::Rect,4> shortcuts{};
  int at(int x,int y,int page,int count) const {
    if(widgets.contains(x,y))return page?Calendar:date.contains(x,y)?Calendar:stats.contains(x,y)?Stats:None;
    constexpr int actions[]={Library,Stats,Calendar,Settings};
    for(int n=0;n<4;++n)if(shortcuts[n].contains(x,y))return actions[n];
    for(int n=0;n<std::min(12,count);++n)if(cells[n].contains(x,y))return App+n;
    return None;
  }
};
inline Layout layout(fui::Rect safe) {
  Layout g;g.safe=safe;int margin=16,gap=12;
  g.status=fui::makeRect(safe.x+margin,safe.y+4,safe.width-2*margin,30);
  int wh=std::clamp(int(safe.height)*31/100,190,250);
  g.widgets=fui::makeRect(safe.x+margin,g.status.bottom()+10,safe.width-2*margin,wh);
  g.date=fui::makeRect(g.widgets.x,g.widgets.y,(g.widgets.width-gap)/2,wh);
  g.stats=fui::makeRect(g.date.right()+gap,g.widgets.y,g.widgets.right()-g.date.right()-gap,wh);
  g.dock=fui::makeRect(safe.x+margin,safe.bottom()-86,safe.width-2*margin,70);
  g.grid=fui::makeRect(g.widgets.x,g.widgets.bottom()+12,g.widgets.width,g.dock.y-g.widgets.bottom()-24);
  for(int n=0;n<12;++n){int c=n%4,r=n/4;g.cells[n]=fui::makeRect(g.grid.x+c*g.grid.width/4,g.grid.y+r*g.grid.height/3,(c+1)*g.grid.width/4-c*g.grid.width/4,(r+1)*g.grid.height/3-r*g.grid.height/3);}
  for(int n=0;n<4;++n)g.shortcuts[n]=fui::makeRect(g.dock.x+n*g.dock.width/4,g.dock.y,g.dock.width/4,g.dock.height);
  return g;
}
struct Tile{const char* title="";const freeink::Icon* icon=nullptr;};
struct Model {
  int page=0,count=0,today=0,battery=0;
  const char* time="";
  bool statsReady=false,statsFailed=false;
  unsigned goal=0;
  std::array<uint32_t,7> seconds{};
  std::array<Tile,12> tiles{};
  std::array<const freeink::Icon*,4> dock{};
};
inline void text(fui::DrawTarget& t,fui::Rect r,const char* s,int font=0,bool bold=false){
  fui::TextStyle st;st.font=font;st.bold=bold;st.align=fui::TextAlign::Center;fittext::draw(t,r,s,st,2);
}
inline void draw(fui::DrawTarget& t,const Layout& g,const Model& m){
  auto ink=fui::Paint::solid(fui::Color::Black),white=fui::Paint::solid(fui::Color::White);
  t.fill(g.safe,white);char s[80];
  text(t,fui::makeRect(g.status.x,g.status.y,90,g.status.height),m.time,0,true);
  std::snprintf(s,sizeof(s),"%d%%",m.battery);text(t,fui::makeRect(g.status.right()-70,g.status.y,70,g.status.height),s);
  auto date=readingstats::dateFromDay(m.today);bool known=m.today!=0;
  if(m.page){
    t.stroke(g.widgets,ink,1,20);
    if(known)std::snprintf(s,sizeof(s),"THÁNG %02d · %d",date.month,date.year);else std::snprintf(s,sizeof(s),"CHƯA ĐẶT NGÀY");
    text(t,fui::makeRect(g.widgets.x+12,g.widgets.y+6,g.widgets.width-24,32),s,1,true);
    if(known){
      int start=readingstats::weekday(readingstats::dayIndex({date.year,date.month,1})),days=readingstats::daysInMonth(date.year,date.month);
      int cw=(g.widgets.width-20)/7,rh=(g.widgets.height-65)/6;
      constexpr const char* labels[]={"T2","T3","T4","T5","T6","T7","CN"};
      for(int c=0;c<7;++c)text(t,fui::makeRect(g.widgets.x+10+c*cw,g.widgets.y+37,cw,24),labels[c],0,c>=5);
      for(int d=1;d<=days;++d){int i=start+d-1;auto cell=fui::makeRect(g.widgets.x+10+(i%7)*cw,g.widgets.y+64+(i/7)*rh,cw,rh);if(d==date.day)t.stroke(cell,ink,2,6);std::snprintf(s,sizeof(s),"%d",d);text(t,cell,s,0,i%7>=5||d==date.day);}
    }
  }else{
    t.stroke(g.date,ink,1,20);t.stroke(g.stats,ink,1,20);
    constexpr const char* days[]={"THỨ HAI","THỨ BA","THỨ TƯ","THỨ NĂM","THỨ SÁU","THỨ BẢY","CHỦ NHẬT"};
    text(t,fui::makeRect(g.date.x+8,g.date.y+10,g.date.width-16,32),known?days[readingstats::weekday(m.today)]:"HÔM NAY",0,true);
    if(known)std::snprintf(s,sizeof(s),"%02d",date.day);else std::snprintf(s,sizeof(s),"—");
    text(t,fui::makeRect(g.date.x+8,g.date.y+52,g.date.width-16,86),s,5,true);
    if(known)std::snprintf(s,sizeof(s),"Tháng %d · %d",date.month,date.year);else std::snprintf(s,sizeof(s),"Chưa đặt ngày");
    text(t,fui::makeRect(g.date.x+8,g.date.bottom()-72,g.date.width-16,30),s);
    if(known){auto lunar=vietnameselunar::fromCivilDay(m.today,date.year);std::snprintf(s,sizeof(s),"Âm %d.%d%s",lunar.day,lunar.month,lunar.leap?" nhuận":"");text(t,fui::makeRect(g.date.x+8,g.date.bottom()-39,g.date.width-16,30),s);}
    text(t,fui::makeRect(g.stats.x+8,g.stats.y+10,g.stats.width-16,32),"ĐỌC HÔM NAY",0,true);
    std::snprintf(s,sizeof(s),"%u phút",m.seconds[6]/60);
    text(t,fui::makeRect(g.stats.x+8,g.stats.y+50,g.stats.width-16,44),m.statsReady?s:m.statsFailed?"Chưa tải được":"Đang tải…",1,true);
    std::snprintf(s,sizeof(s),"Mục tiêu %u phút",m.goal);text(t,fui::makeRect(g.stats.x+8,g.stats.y+98,g.stats.width-16,28),m.goal?s:"7 ngày gần đây");
    auto peak=std::max<uint32_t>(60,*std::max_element(m.seconds.begin(),m.seconds.end()));
    int bw=std::max(4,(int(g.stats.width)-44)/7),base=g.stats.bottom()-35,ch=std::max(14,int(g.stats.height)-170);
    for(int n=0;n<7;++n){int h=m.statsReady?int(uint64_t(m.seconds[n])*ch/peak):0;if(h)t.fill(fui::makeRect(g.stats.x+20+n*bw,base-h,bw-5,h),ink,2);t.line({int16_t(g.stats.x+20+n*bw),int16_t(base)},{int16_t(g.stats.x+20+n*bw+bw-5),int16_t(base)},1,ink);}
    text(t,fui::makeRect(g.stats.x+8,g.stats.bottom()-29,g.stats.width-16,24),"THỐNG KÊ ĐỌC");
  }
  for(int n=0;n<m.count&&n<12;++n){auto c=g.cells[n];int size=std::min({64,int(c.width)-20,int(c.height)-40});auto icon=fui::makeRect(c.x+(c.width-size)/2,c.y+5,size,size);t.stroke(icon,ink,1,14);if(m.tiles[n].icon)t.bitmap(icon.inset({8,8,8,8}),fui::bitmapFromIcon(*m.tiles[n].icon),fui::BitmapMode::Contain,ink);text(t,fui::makeRect(c.x+2,icon.bottom()+4,c.width-4,c.bottom()-icon.bottom()-5),m.tiles[n].title);}
  t.stroke(g.dock,ink,1,22);
  for(int n=0;n<4;++n)if(m.dock[n]){auto c=g.shortcuts[n];int size=40;t.bitmap(fui::makeRect(c.x+(c.width-size)/2,c.y+(c.height-size)/2,size,size),fui::bitmapFromIcon(*m.dock[n]),fui::BitmapMode::Contain,ink);}
}
}
