#include <FreeInkUI.h>
#include <cassert>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include "../../src/apps_local/ui/ToyboxScreen.h"
#include "../../src/apps_local/ui/QuickSettingsScreen.h"
#include "../../src/apps_local/ui/ZenChrome.h"
#ifdef ZEN_SCREENS_TEST
#include "../../src/apps_local/tools/ToolsScreens.h"
#include "../../src/apps_local/tools/Almanac.h"
#include "../../src/apps_local/reading/ReadingStatsScreens.h"
#endif
namespace fui=freeink::ui;
I18n& I18n::getInstance(){static I18n instance;return instance;}
const char* I18n::get(StrId) const{return "Mục";}
struct Target final:fui::DrawTarget {
 struct Mark{fui::Rect r;uint8_t radius;};std::vector<Mark> shapes;
 std::vector<std::pair<fui::Rect,std::string>> words;
 fui::Size measureText(fui::FontId,const char* s,fui::TextStyle)const override{return {int16_t(s?strlen(s)*7:0),20};}
 int16_t lineHeight(fui::FontId)const override{return 20;}
 void fill(fui::Rect r,fui::Paint,uint8_t radius,uint8_t)override{shapes.push_back({r,radius});}
 void stroke(fui::Rect r,fui::Paint,uint8_t,uint8_t radius,uint8_t)override{shapes.push_back({r,radius});}
 void line(fui::Point,fui::Point,uint8_t,fui::Paint)override{}
 void triangle(fui::Point,fui::Point,fui::Point,fui::Paint)override{}
 void text(fui::Rect r,const char* s,fui::TextStyle)override{words.push_back({r,s?s:""});}
 void bitmap(fui::Rect,fui::BitmapRef,fui::BitmapMode,fui::Paint,fui::Rotation)override{}
};
int main(){
 chrome::lightHeader=true;
 for(const auto size:{fui::Size{480,800},fui::Size{800,480}}){
  fui::DeviceContext d;d.width=size.width;d.height=size.height;d.hasTouch=true;
  Target t;zenchrome::draw(t,d,"10:30",82,true);
  assert(HeaderBackTapTarget::y>=d.height-48);
  assert(HeaderBackTapTarget::contains(24,d.height-24));
  auto body=pixelui::geometry(d).body;assert(body.bottom()<=zenchrome::footer(d).y);
  assert(t.words.size()==2);
  for(auto& x:t.words)assert(x.first.y>=d.height-48);
  Target qt;toybox::Interactions hits;fui::InputSnapshot input;
  toybox::Frame frame(qt,d,input,hits);toybox::Screen screen(frame);
  quickui::Model model;quickui::Work work;quickui::build(screen,model,work);
  int circles=0;for(auto& shape:qt.shapes){
   assert(shape.r.x>=0&&shape.r.y>=0&&shape.r.right()<=d.width&&shape.r.bottom()<=d.height);
   if(shape.r.width==shape.r.height&&shape.radius==shape.r.width/2)++circles;
  }
  assert(circles>=6); // Four shortcuts plus the two circular slider knobs.
  assert(!hits.overflowed());
 }

#ifdef ZEN_SCREENS_TEST
 const int first=readingstats::dayIndex({2026,1,1}),last=readingstats::dayIndex({2028,12,31});
 assert(last-first+1==1096);assert(!almanac::find(first-1)&&!almanac::find(last+1));
 for(int d=first;d<=last;++d){const auto* v=almanac::find(d);assert(v&&v->term<24&&v->officer<12);int hours=0;for(int h=0;h<12;++h)hours+=almanac::auspiciousHour(d,h);assert(hours==6);
  for(const auto* mask:{v->yi,v->ji})for(unsigned n=0;n<almanac::activityTotal(mask);++n){const int i=almanac::activityAt(mask,n);assert(i>=0&&unsigned(i)<almanac::activityCount()&&std::strlen(almanac::activity(i)));}
 }
 char cc[64];almanac::canChiDay(readingstats::dayIndex({2026,11,23}),cc,sizeof(cc));assert(std::string(cc)=="Tân Sửu");
 almanac::canChiMonth({15,10,2026,false},cc,sizeof(cc));assert(std::string(cc)=="Kỷ Hợi");
 assert(almanac::find(readingstats::dayIndex({2028,2,29})));
 for(auto size:{fui::Size{480,800},fui::Size{800,480}}){
  fui::DeviceContext d;d.width=size.width;d.height=size.height;d.hasTouch=true;
  readingstats::Summary summary;summary.anchorDay=readingstats::dayIndex({2026,11,23});
  readingstats::PeriodStats annual;readingstats::setPeriod(annual,readingstats::Period::Year,summary.anchorDay,false);
  readingstats::PeriodStats day;readingstats::setPeriod(day,readingstats::Period::Day,summary.anchorDay,false);
  readingstats::Book books[5]{};for(int n=0;n<5;++n){std::snprintf(books[n].title,sizeof(books[n].title),"Sách %d",n+1);books[n].seconds=600*(n+1);}
  for(int tab=0;tab<4;++tab){Target target;toybox::Interactions hits;fui::InputSnapshot input;toybox::Frame frame(target,d,input,hits);toybox::Screen screen(frame);
   readingui::Model model;readingui::Work work;model.summary=&summary;model.annualStats=&annual;model.periodStats=&day;model.today=model.displayedDay=summary.anchorDay;model.month=2026*12+10;model.tab=tab;model.books=books;model.count=model.bookTotal=5;readingui::build(screen,model,work);
   for(auto shape:target.shapes){if(shape.r.x<0||shape.r.y<0||shape.r.right()>d.width||shape.r.bottom()>d.height){std::cerr<<"Stats tab "<<tab<<" outside "<<shape.r.x<<","<<shape.r.y<<","<<shape.r.width<<","<<shape.r.height<<" on "<<d.width<<"x"<<d.height<<"\n";assert(false);}}
   for(const auto& word:target.words){const auto r=word.first;if(r.y<0||r.bottom()>d.height){std::cerr<<"Text outside "<<word.second<<" at "<<r.x<<","<<r.y<<","<<r.width<<","<<r.height<<" on "<<d.width<<"x"<<d.height<<"\n";assert(false);}}
   assert(!hits.overflowed());
  }
  for(int mode=0;mode<5;++mode){Target target;toybox::Interactions hits;fui::InputSnapshot input;toybox::Frame frame(target,d,input,hits);toybox::Screen screen(frame);
   toolsui::Model model;toolsui::Work work;model.year=2026;model.month=11;model.selected=model.today=summary.anchorDay;model.dayDetail=mode==1||mode==2;model.moreDay=mode==2;model.monthPicker=mode==3;model.holidays=mode==4;toolsui::build(screen,model,work);
   for(auto shape:target.shapes){if(shape.r.x<0||shape.r.y<0||shape.r.right()>d.width||shape.r.bottom()>d.height){std::cerr<<"Calendar mode "<<mode<<" outside "<<shape.r.x<<","<<shape.r.y<<","<<shape.r.width<<","<<shape.r.height<<" on "<<d.width<<"x"<<d.height<<"\n";assert(false);}}
   for(const auto& word:target.words){const auto r=word.first;if(r.y<0||r.bottom()>d.height){std::cerr<<"Text outside "<<word.second<<" at "<<r.x<<","<<r.y<<","<<r.width<<","<<r.height<<" on "<<d.width<<"x"<<d.height<<"\n";assert(false);}}
   assert(!hits.overflowed());
  }
 }
#endif
 std::cout<<"Bottom navigation, status geometry and circular quick controls passed\n";
}
