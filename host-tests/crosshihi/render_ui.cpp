#include <FreeInkUIDisplayTarget.h>
#include "apps_local/home/HomePaws.h"
#include "apps_local/home/CatExpressions.h"
#include "apps_local/home/CoverFace.h"
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
#include "../../src/apps_local/home/HomeLauncher.h"
#include "../../src/apps_local/home/ApprovedHomeUi.h"
#include "../../src/apps_local/library/LibraryMenu.h"
#include "../../src/apps_local/library/LibraryControls.h"
#include "../../src/apps_local/ui/NavigationIcons.h"
#include "../../src/apps_local/ui/GameMenuIcons.h"
namespace fui=freeink::ui;
I18n& I18n::getInstance(){static I18n i;return i;}
void I18n::setLanguage(Language){_language=Language::VI;}
const char* I18n::get(StrId id)const{auto v=getLanguageStrings(Language::VI);auto off=v.offsets[(size_t)id];if(off&0x80000000){auto en=getLanguageStrings(Language::EN);return en.data+(off&0x7fffffff);}return v.data+off;}
struct Font {
 std::vector<fui::FontGlyph> glyphs;fui::BitmapFont font;
 Font(const EpdFontData& source,bool digitsOnly=false):glyphs(0x203b-32){
  for(unsigned cp=32;cp<0x203b;++cp){unsigned index=31;for(unsigned i=0;i<source.intervalCount;++i){auto r=source.intervals[i];if((!digitsOnly||cp<128)&&cp>=r.first&&cp<=r.last){index=r.offset+cp-r.first;break;}}
   const auto g=source.glyph[index];assert(g.dataOffset<65536);glyphs[cp-32]={uint16_t(g.dataOffset),g.width,g.height,uint8_t(fp4::toPixel(g.advanceX)),int8_t(g.left),int8_t(-g.top)};
  }
  font={source.bitmap,glyphs.data(),32,0x203a,uint8_t(source.advanceY),uint8_t(source.ascender),100,100,1};
 }
};
struct Canvas{
 std::vector<uint8_t> pixels=std::vector<uint8_t>(60*800,255);fui::DisplayTarget target{pixels.data(),480,800,60};
 fui::DeviceContext device;fui::InputSnapshot input;toybox::Interactions hits;toybox::Frame frame;toybox::Screen screen;
 Font readingBold{sans13b},huge{sans28b,true},small{sans10},body{sans12},title{sans18b},tiny{sans08},reading{sans13};
 Canvas():device([&]{auto d=target.deviceContext();d.hasTouch=true;return d;}()),frame(target,device,input,hits,nullptr,{82,false,true,"10:30"}),screen(frame){device.hasTouch=true;target.setFont(0,small.font);target.setFont(1,body.font);target.setFont(2,title.font);target.setFont(5,huge.font);target.setFont(3,tiny.font);target.setFont(4,small.font);target.setFont(fui::HEADER_CLOCK_FONT,small.font);}
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
 for(int page=0;page<2;++page){
  Canvas c;c.target.setFont(5,c.huge.font);homelauncher::Model m;m.page=page;m.count=page?10:12;m.today=readingstats::dayIndex({2026,11,24});m.clockKnown=true;m.hour=7;m.minute=45;m.battery=82;m.statsReady=true;m.goal=120;m.seconds={1800,3600,2700,4200,1200,4800,5700};
  m.dock={&navicons::Books32,&navicons::Stats32,&navicons::Calendar32,&navicons::Gear32};
  const homelauncher::Tile tiles[]={{tr(STR_UI_FLASHCARDS_E0439E),&navicons::Cards32},{tr(STR_UI_READING_QUEST_6AD39C),&navicons::BookOpen32},{"Mèo & Mọt",&navicons::Paw32},{tr(STR_UI_CLOCK_6E3C9D),&navicons::Clock32},{tr(STR_CALCULATOR_APP),&navicons::Calculator32},{tr(STR_UI_WALLPAPERS_2AA278),&navicons::Image32},{"SUDOKU",&gameicons::Sudoku40},{"PICROSS",&gameicons::Picross40},{"WORDLE",&gameicons::Wordle40},{tr(STR_UI_MINESWEEPER_A2F226),&gameicons::Minesweeper40},{"2048",&gameicons::Game204840},{tr(STR_UI_CHESS_902609),&gameicons::Chess40},{"GO",&gameicons::Go40},{"OTHELLO",&navicons::Othello32},{tr(STR_UI_SOLITAIRE_8C3ACB),&gameicons::Solitaire40},{tr(STR_UI_HEARTS_29FBCB),&gameicons::Hearts40},{tr(STR_UI_BATTLESHIP_4046EE),&gameicons::Battleship40},{tr(STR_UI_CONNECT_FOUR_A81733),&gameicons::ConnectFour40},{tr(STR_INTERCEPT_NAME),&gameicons::Intercept40},{"MURDLE",&gameicons::Murdle40},{tr(STR_CAR_TITLE),&gameicons::Carnivoro40},{tr(STR_CHICKEN_GAME),&navicons::Paw32}};
  for(int n=0;n<m.count;++n)m.tiles[n]=tiles[page*12+n];
  homelauncher::draw(c.target,homelauncher::layout(c.device.safeRect()),m);c.save(std::string(argv[1])+"/home2-"+std::to_string(page)+".pbm");
 }

 {Canvas c;pixelui::begin(c.screen,36);libraryui::filterTabs(c.screen,3,1);
  fui::ListItem rows[7]{};const char* names[]={"Dế Mèn phiêu lưu ký","Nhà giả kim","Hoàng tử bé","Đi tìm lẽ sống","Totto-chan","Tuổi trẻ đáng giá bao nhiêu?","Không gia đình"};
  for(int n=0;n<7;++n){rows[n].label=names[n];rows[n].value="epub";}
  fui::ListProps p;libraryui::styleBookRows(p,c.target);libraryui::lightRows(p);p.items=rows;p.count=7;p.selectedIndex=-1;
  const auto area=c.screen.body();pixelui::list(c.screen,p);
  int black=0,all=0;for(int y=area.y;y<area.bottom();++y)for(int x=area.x;x<area.right();++x){++all;if(!(c.pixels[y*60+x/8]&(0x80>>(x%8))))++black;}
  assert(black>100 && black<all/3);c.save(std::string(argv[1])+"/library-all.pbm");
 }

 {Canvas c;c.target.setFont(5,c.huge.font);c.target.setFont(6,c.readingBold.font);
  c.device.safeArea={2,2,2,2};
  auto g=readingdesk::screenLayout(c.device,3,true,true,true,false);
  approvedhome::draw(c.target,fui::makeRect(0,0,480,800));
  // Native cover planes, with a public sample rather than a private reader photo.
  for(int n=0;n<4;++n){auto face=n?homecover::shelfFaces[n-1]:homecover::mainFace;const auto bounds=face.bounds();
    face.mapRect(bounds,bounds.x,bounds.y,bounds.width,bounds.height,[&](int x,int y,int w,int h){c.target.fill(fui::makeRect(x,y,w,h),fui::Paint::solid(fui::Color::White));});
    c.target.line({int16_t(face.left),int16_t(face.topLeft)},{int16_t(face.right),int16_t(face.topRight)},1,fui::Paint::solid(fui::Color::Black));
    c.target.line({int16_t(face.left),int16_t(face.bottomLeft)},{int16_t(face.right),int16_t(face.bottomRight)},1,fui::Paint::solid(fui::Color::Black));
    approvedhome::text(c.target,bounds.inset({9,24,9,24}),n?"Sách\ntrên thẻ":"Bìa sách\ntrên thẻ",1,false,fui::TextAlign::Center,2);
  }
  homepaws::draw(c.target);catexpressions::draw(c.target,0);
  approvedhome::Info m;m.clock="07:45";m.date="T3 · 24.11";m.lunar="Âm 16.10";m.title="Dế Mèn phiêu lưu ký";m.author="Tô Hoài";m.chapter="Chương 4 / 10";m.remaining="Còn 6 chương";m.estimate="Ước tính 2 giờ 15 phút";m.percent=40;m.battery=82;m.quote="Đi một ngày đàng, học một sàng khôn.";
  approvedhome::overlay(c.target,g,m);c.save(std::string(argv[1])+"/home-approved.pbm");
 }

}
