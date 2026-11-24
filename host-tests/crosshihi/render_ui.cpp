#include <FreeInkUIDisplayTarget.h>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <I18n.h>
#include <I18nStrings.h>
#include "../../src/apps_local/ui/fonts/RobotoFonts.h"
#include "../../src/apps_local/ui/ToyboxScreen.h"
#include "../../src/apps_local/ui/QuickSettingsScreen.h"
#include "../../src/apps_local/tools/ToolsScreens.h"
#include "../../src/apps_local/reading/ReadingStatsScreens.h"
namespace fui=freeink::ui;
I18n& I18n::getInstance(){static I18n i;return i;}
void I18n::setLanguage(Language){_language=Language::VI;}
const char* I18n::get(StrId id)const{auto v=getLanguageStrings(Language::VI);auto off=v.offsets[(size_t)id];if(off&0x80000000){auto en=getLanguageStrings(Language::EN);return en.data+(off&0x7fffffff);}return v.data+off;}
struct Font {
 std::vector<fui::FontGlyph> glyphs;fui::BitmapFont font;
 Font(const EpdFontData& source):glyphs(0x203b-32){
  for(unsigned cp=32;cp<0x203b;++cp){unsigned index=31;for(unsigned i=0;i<source.intervalCount;++i){auto r=source.intervals[i];if(cp>=r.first&&cp<=r.last){index=r.offset+cp-r.first;break;}}
   const auto g=source.glyph[index];assert(g.dataOffset<65536);glyphs[cp-32]={uint16_t(g.dataOffset),g.width,g.height,uint8_t(fp4::toPixel(g.advanceX)),int8_t(g.left),int8_t(-g.top)};
  }
  font={source.bitmap,glyphs.data(),32,0x203a,uint8_t(source.advanceY),uint8_t(source.ascender),100,100,1};
 }
};
struct Canvas{
 std::vector<uint8_t> pixels=std::vector<uint8_t>(60*800,255);fui::DisplayTarget target{pixels.data(),480,800,60};
 fui::DeviceContext device;fui::InputSnapshot input;toybox::Interactions hits;toybox::Frame frame;toybox::Screen screen;
 Font small{sans10},body{sans12},title{sans18b},tiny{sans08},reading{sans13};
 Canvas():device([&]{auto d=target.deviceContext();d.hasTouch=true;return d;}()),frame(target,device,input,hits,nullptr,{82,false,true,"10:30"}),screen(frame){device.hasTouch=true;target.setFont(0,small.font);target.setFont(1,body.font);target.setFont(2,title.font);target.setFont(3,tiny.font);target.setFont(4,small.font);target.setFont(fui::HEADER_CLOCK_FONT,small.font);}
 void save(const std::string& path){FILE* f=fopen(path.c_str(),"wb");assert(f);fprintf(f,"P4\n480 800\n");for(auto v:pixels)fputc(v^255,f);fclose(f);}
};
int main(int argc,char**argv){assert(argc==2);I18N.setLanguage(Language::VI);chrome::lightHeader=true;pixelui::preserveTranslatedCase=true;pixelui::uppercaseLabels=false;
 const int today=readingstats::dayIndex({2026,11,23});readingstats::Summary summary;summary.anchorDay=today;readingstats::PeriodStats annual,day;readingstats::setPeriod(annual,readingstats::Period::Year,today,false);readingstats::setPeriod(day,readingstats::Period::Day,today,false);
 summary.seconds=128*3600+40*60;summary.completed=46;summary.pages=2120;summary.sessions=320;summary.longest=24;summary.run=18;summary.lastReadDay=today;for(int n=0;n<215;++n)summary.dailySeconds[n]=2400+(n%6)*480;
 annual.seconds=summary.seconds;annual.pages=summary.pages;annual.sessions=summary.sessions;annual.completed=46;for(int n=0;n<11;++n)annual.annualSeconds[n]=(7+n)*3600;
 day.seconds=95*60;day.pages=72;day.sessions=6;day.hours[1]=15*60;day.hours[2]=30*60;day.hours[3]=10*60;day.hours[4]=40*60;
 readingstats::Book books[5]{};const char* names[]={"Dế Mèn phiêu lưu ký","Nhà giả kim","Hoàng tử bé","Đi tìm lẽ sống","Totto-chan"};for(int n=0;n<5;++n){std::snprintf(books[n].title,sizeof(books[n].title),"%s",names[n]);books[n].seconds=(380-n*48)*60;books[n].progressBp=7200;books[n].sessions=8;}
 for(int tab=0;tab<4;++tab){Canvas c;readingui::Model m;readingui::Work w;m.summary=&summary;m.annualStats=&annual;m.periodStats=&day;m.today=m.displayedDay=today;m.month=2026*12+10;m.tab=tab;m.books=books;m.count=m.bookTotal=5;m.goalMinutes=120;readingui::build(c.screen,m,w);c.save(std::string(argv[1])+"/stats-"+std::to_string(tab)+".pbm");}
 for(int mode=0;mode<5;++mode){Canvas c;toolsui::Model m;toolsui::Work w;m.year=2026;m.month=11;m.selected=m.today=today;m.dayDetail=mode==1||mode==2;m.moreDay=mode==2;m.monthPicker=mode==3;m.holidays=mode==4;toolsui::build(c.screen,m,w);c.save(std::string(argv[1])+"/calendar-"+std::to_string(mode)+".pbm");}
 {Canvas c;quickui::Model m;quickui::Work w;quickui::build(c.screen,m,w);c.save(std::string(argv[1])+"/quick.pbm");}
}
