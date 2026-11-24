#pragma once
#include <FreeInkUI.h>
#include <FreeInkUIIcon.h>
#include <Icon.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cmath>
#include "../reading/ReadingStatsCore.h"
#include "../tools/VietnameseLunar.h"
#include "../ui/FitText.h"
#include "WidgetDigits.h"
namespace homelauncher {
namespace fui=freeink::ui;
enum Action {None,Calendar,Stats,Library,Settings,WidgetSettings,Clock,App=100};
enum class Widget:uint8_t {Date,Clock,Reading,Month};
inline Widget widget(unsigned value){return value<=3?Widget(value):Widget::Date;}
inline int action(Widget w){return w==Widget::Clock?Clock:w==Widget::Reading?Stats:Calendar;}
struct Layout {
  fui::Rect safe,status,picker,widgets,date,stats,grid,dock;
  std::array<fui::Rect,12> cells{};
  std::array<fui::Rect,4> shortcuts{};
  int at(int x,int y,int page,int count,const std::array<Widget,3>& types={Widget::Date,Widget::Clock,Widget::Reading})const{
    if(picker.contains(x,y))return WidgetSettings;
    if(widgets.contains(x,y))return page?action(types[2]):date.contains(x,y)?action(types[0]):stats.contains(x,y)?action(types[1]):None;
    constexpr int actions[]={Library,Stats,Calendar,Settings};
    for(int n=0;n<4;++n)if(shortcuts[n].contains(x,y))return actions[n];
    for(int n=0;n<std::min(12,count);++n)if(cells[n].contains(x,y))return App+n;
    return None;
  }
};
inline Layout layout(fui::Rect safe){
  Layout g;g.safe=safe;int margin=16,gap=12;
  g.status=fui::makeRect(safe.x+margin,safe.y+4,safe.width-2*margin,30);
  g.picker=fui::makeRect(safe.x+(safe.width-110)/2,safe.y,110,40);
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
struct Model{
  int page=0,count=0,today=0,battery=0;
  const char* time="";
  bool statsReady=false,statsFailed=false,clockKnown=false;
  uint8_t hour=0,minute=0;
  unsigned goal=0;
  std::array<Widget,3> widgets{Widget::Date,Widget::Clock,Widget::Reading};
  std::array<uint32_t,7> seconds{};
  std::array<Tile,12> tiles{};
  std::array<const freeink::Icon*,4> dock{};
};
inline void text(fui::DrawTarget& t,fui::Rect r,const char* s,int font=0,bool bold=false){
  fui::TextStyle st;st.font=font;st.bold=bold;st.align=fui::TextAlign::Center;fittext::draw(t,r,s,st,2);
}
inline void bigDay(fui::DrawTarget& t,fui::Rect box,unsigned day){
  auto ink=fui::Paint::solid(fui::Color::Black);
  const int gap=8,h=box.height,w=std::min(int(box.width)/2-8,h*3/4),left=box.x+(box.width-(2*w+gap))/2;
  for(int n=0;n<2;++n){unsigned digit=n?day%10:day/10%10;t.bitmap(fui::makeRect(left+n*(w+gap),box.y,w,h),fui::bitmapFromIcon(*widgetdigits::digits[digit]),fui::BitmapMode::Contain,ink);}
}
inline fui::Point clockPoint(int cx,int cy,int radius,double ticks){
  constexpr double pi=3.14159265358979323846;
  const double angle=ticks*pi/30;
  return {int16_t(cx+std::lround(std::sin(angle)*radius)),int16_t(cy-std::lround(std::cos(angle)*radius))};
}
inline void analogClock(fui::DrawTarget& t,fui::Rect box,const Model& m){
  auto ink=fui::Paint::solid(fui::Color::Black);
  const int side=std::max(40,std::min(int(box.width)-28,int(box.height)-66));
  const int radius=side/2,cx=box.x+box.width/2,cy=box.y+box.height/2;
  // The widget border is the clock frame: ticks follow its four straight edges.
  const int left=box.x+10,right=box.right()-10,top=box.y+10,bottom=box.bottom()-10;
  for(int n=0;n<60;++n){auto q=clockPoint(0,0,1000,n);const double scale=std::max(std::abs(int(q.x)),std::abs(int(q.y)));
    const int px=box.x+box.width/2+int(q.x*(box.width/2-10)/scale),py=box.y+box.height/2+int(q.y*(box.height/2-10)/scale);
    if(n%15==0)continue;
    const int len=n%5?3:8;
    if(std::abs(int(q.x))>=std::abs(int(q.y)))t.line({int16_t(px),int16_t(py)},{int16_t(px+(px<cx?len:-len)),int16_t(py)},n%5?1:2,ink);
    else t.line({int16_t(px),int16_t(py)},{int16_t(px),int16_t(py+(py<box.y+box.height/2?len:-len))},n%5?1:2,ink);
  }
  text(t,fui::makeRect(cx-18,top+2,36,28),"12",0,true);text(t,fui::makeRect(cx-18,bottom-30,36,28),"6",0,true);
  text(t,fui::makeRect(left+2,box.y+box.height/2-14,28,28),"9",0,true);text(t,fui::makeRect(right-30,box.y+box.height/2-14,28,28),"3",0,true);
  if(m.clockKnown){
    const fui::Point center{int16_t(cx),int16_t(cy)};
    t.line(center,clockPoint(cx,cy,radius*50/100,(m.hour%12)*5+m.minute/12.0),5,ink);
    t.line(center,clockPoint(cx,cy,radius*74/100,m.minute),3,ink);
    t.fill(fui::makeRect(cx-4,cy-4,8,8),ink,4);
  }else text(t,fui::makeRect(cx-radius+10,cy-13,2*radius-20,26),"Chưa đặt giờ",0);
}
inline void drawWidget(fui::DrawTarget& t,fui::Rect box,Widget kind,const Model& m){
  const auto ink=fui::Paint::solid(fui::Color::Black);t.stroke(box,ink,1,20);
  char s[96];const bool known=m.today!=0;const auto date=readingstats::dateFromDay(m.today);
  const bool wide=box.width>300;
  if(kind==Widget::Clock){

    analogClock(t,box,m);return;
  }
  if(kind==Widget::Date){
    constexpr const char* days[]={"THỨ HAI","THỨ BA","THỨ TƯ","THỨ NĂM","THỨ SÁU","THỨ BẢY","CHỦ NHẬT"};
    text(t,fui::makeRect(box.x+8,box.y+8,box.width-16,30),known?days[readingstats::weekday(m.today)]:"HÔM NAY",0,true);
    const int top=box.y+44,bottom=box.bottom()-78;
    if(known)bigDay(t,fui::makeRect(box.x+14,top,box.width-28,std::max(40,bottom-top)),date.day);
    else text(t,fui::makeRect(box.x+8,top,box.width-16,60),"—",2);
    if(known)std::snprintf(s,sizeof(s),"Tháng %d · %d",date.month,date.year);else std::snprintf(s,sizeof(s),"Chưa đặt ngày");
    text(t,fui::makeRect(box.x+8,box.bottom()-72,box.width-16,30),s);
    if(known){auto l=vietnameselunar::fromCivilDay(m.today,date.year);std::snprintf(s,sizeof(s),"Âm %d.%d%s",l.day,l.month,l.leap?" nhuận":"");text(t,fui::makeRect(box.x+8,box.bottom()-39,box.width-16,30),s);}return;
  }
  if(kind==Widget::Month){
    if(known)std::snprintf(s,sizeof(s),"THÁNG %02d · %d",date.month,date.year);else std::snprintf(s,sizeof(s),"CHƯA ĐẶT NGÀY");
    text(t,fui::makeRect(box.x+12,box.y+6,box.width-24,32),s,wide?1:0,true);
    if(!known)return;
    int start=readingstats::weekday(readingstats::dayIndex({date.year,date.month,1})),days=readingstats::daysInMonth(date.year,date.month);
    int cw=(box.width-20)/7,rh=(box.height-65)/6;
    constexpr const char* labels[]={"T2","T3","T4","T5","T6","T7","CN"};
    for(int c=0;c<7;++c)text(t,fui::makeRect(box.x+10+c*cw,box.y+37,cw,24),labels[c],wide?0:3,c>=5);
    for(int d=1;d<=days;++d){int i=start+d-1;auto cell=fui::makeRect(box.x+10+(i%7)*cw,box.y+64+(i/7)*rh,cw,rh);if(d==date.day)t.stroke(cell,ink,2,6);std::snprintf(s,sizeof(s),"%d",d);text(t,cell,s,wide?0:3,i%7>=5||d==date.day);}return;
  }
  text(t,fui::makeRect(box.x+8,box.y+9,box.width-16,32),"THỐNG KÊ ĐỌC",0,true);
  const int leftWidth=wide?box.width*44/100:box.width;
  std::snprintf(s,sizeof(s),"%u phút",m.seconds[6]/60);
  text(t,fui::makeRect(box.x+8,box.y+52,leftWidth-16,48),m.statsReady?s:m.statsFailed?"Chưa tải được":"Đang tải…",1,true);
  std::snprintf(s,sizeof(s),"Hôm nay · mục tiêu %u phút",m.goal);
  text(t,fui::makeRect(box.x+8,box.y+(wide?106:100),leftWidth-16,wide?50:28),m.goal?s:"Đọc hôm nay");
  const auto peak=std::max<uint32_t>(60,*std::max_element(m.seconds.begin(),m.seconds.end()));
  int chartX=wide?box.x+leftWidth:box.x+18,chartW=wide?box.width-leftWidth-18:box.width-36;
  int base=box.bottom()-65,ch=wide?box.height-114:std::max(4,int(box.height)-205),bw=chartW/7;
  constexpr const char* labels[]={"T2","T3","T4","T5","T6","T7","CN"};
  for(int n=0;n<7;++n){int h=m.statsReady?int(uint64_t(m.seconds[n])*ch/peak):0;
    if(h)t.fill(fui::makeRect(chartX+n*bw+3,base-h,std::max(2,bw-8),h),ink,2);
    t.line({int16_t(chartX+n*bw+2),int16_t(base)},{int16_t(chartX+(n+1)*bw-3),int16_t(base)},1,ink);
    if(known&&wide)text(t,fui::makeRect(chartX+n*bw,base+4,bw,24),labels[readingstats::weekday(m.today-6+n)],3);
  }
  text(t,fui::makeRect(box.x+8,box.bottom()-29,box.width-16,24),"7 ngày gần đây");
}
inline void draw(fui::DrawTarget& t,const Layout& g,const Model& m){
  auto ink=fui::Paint::solid(fui::Color::Black),white=fui::Paint::solid(fui::Color::White);
  t.fill(g.safe,white);char s[32];
  text(t,fui::makeRect(g.status.x,g.status.y,90,g.status.height),m.time,0,true);
  text(t,g.picker,"Widget",0,true);
  std::snprintf(s,sizeof(s),"%d%%",m.battery);text(t,fui::makeRect(g.status.right()-70,g.status.y,70,g.status.height),s);
  if(m.page)drawWidget(t,g.widgets,m.widgets[2],m);else{drawWidget(t,g.date,m.widgets[0],m);drawWidget(t,g.stats,m.widgets[1],m);}
  for(int n=0;n<m.count&&n<12;++n){auto c=g.cells[n];int size=std::min({64,int(c.width)-20,int(c.height)-40});auto icon=fui::makeRect(c.x+(c.width-size)/2,c.y+5,size,size);t.stroke(icon,ink,1,14);if(m.tiles[n].icon)t.bitmap(icon.inset({8,8,8,8}),fui::bitmapFromIcon(*m.tiles[n].icon),fui::BitmapMode::Contain,ink);text(t,fui::makeRect(c.x+2,icon.bottom()+4,c.width-4,c.bottom()-icon.bottom()-5),m.tiles[n].title);}
  t.stroke(g.dock,ink,1,22);
  for(int n=0;n<4;++n)if(m.dock[n]){auto c=g.shortcuts[n];t.bitmap(fui::makeRect(c.x+(c.width-40)/2,c.y+(c.height-40)/2,40,40),fui::bitmapFromIcon(*m.dock[n]),fui::BitmapMode::Contain,ink);}
}
}
