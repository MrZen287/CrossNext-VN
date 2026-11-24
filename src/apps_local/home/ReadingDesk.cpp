#include "ReadingDesk.h"
#include <GfxRenderer.h>
#include <HalClock.h>
#include <I18n.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include "../../RecentBooksStore.h"
#include "../../util/BookProgress.h"
#include "../reading/ReadingStatsStore.h"
#include "../tools/VietnameseLunar.h"
#include "../ui/CompanionArt.h"
#include "../ui/CrossZenArt.h"
#include "../ui/FitText.h"
#include "../ui/Dashboard.h"
#include "../ui/NavigationIcons.h"
#include "../ui/PixelMenuTheme.h"
#include "../ui/VietnameseQuotes.h"
#include "BookCoverPainter.h"
namespace readingdesk {
namespace fui=freeink::ui;
using fui::makeRect;
namespace {
constexpr auto ink=fui::Paint::solid(fui::Color::Black);
constexpr auto white=fui::Paint::solid(fui::Color::White);
constexpr Action menuActions[]={Stats,Calendar,Transfer,Settings};
const char* label(Action a) {
  const bool vi=I18N.getLanguage()==Language::VI;
  switch(a) {
    case Library:return vi?"Thư viện":"Library";
    case Stats:return vi?"Thống kê đọc":"Reading stats";
    case Calendar:return vi?"Lịch":"Calendar";
    case Apps:return "APP";
    case Transfer:return vi?"Truyền tệp":"File transfer";
    case Settings:return vi?"Cài đặt":"Settings";
    default:return "";
  }
}
void text(fui::DrawTarget& t,fui::Rect r,const char* s,int font=0,int lines=1,bool bold=false,
          fui::TextAlign align=fui::TextAlign::Left,fui::Color color=fui::Color::Black) {
  fui::TextStyle style; style.font=font;style.maxLines=lines;style.bold=bold;style.align=align;style.color=color;
  t.text(r,s,style);
}
}
void draw(GfxRenderer& r,Layout& g,const RecentBook* featured,const std::vector<RecentBook>& shelf,
          int,Action focus,const char* quote,unsigned pose) {
  r.clearScreen();auto t=toybox::makeTarget(r);pixelui::bindFonts(t,r);
  char s[160]{};
  const auto clockBox=g.status;
  t.fill(clockBox,white,6);t.stroke(clockBox,ink,1,6);
  for(int x : {clockBox.x+18,clockBox.right()-18}) {
    t.fill(makeRect(x-2,clockBox.y-5,5,11),white,2);
    t.stroke(makeRect(x-2,clockBox.y-5,5,11),ink,1,2);
  }
  t.line({int16_t(clockBox.x+7),int16_t(clockBox.y+43)},{int16_t(clockBox.right()-7),int16_t(clockBox.y+43)},1,ink);
  halClock.formatTime(s,sizeof(s),SETTINGS.clockFormat==1);
  text(t,makeRect(clockBox.x+4,clockBox.y+5,clockBox.width-8,36),s,1,1,true,fui::TextAlign::Center);
  const bool vi=true;
  const int today=readingstats::today();
  if(today) {
    const auto d=readingstats::dateFromDay(today);
    constexpr const char* vn[]={"T2","T3","T4","T5","T6","T7","CN"};
    const int day=readingstats::weekday(today);
    std::snprintf(s,sizeof(s),"%s · %02d.%02d",vn[day],d.day,d.month);
    fittext::draw(t,makeRect(g.heading.x,g.heading.y,g.heading.width,24),s,0,1,fui::TextAlign::Center);
    const auto l=vietnameselunar::fromCivilDay(today,d.year);
    if(l.day) {
      std::snprintf(s,sizeof(s),"Âm %d.%d",l.day,l.month);
      fittext::draw(t,makeRect(g.heading.x,g.heading.y+24,g.heading.width,24),s,0,1,fui::TextAlign::Center);
    }
  }
  if(!featured) text(t,g.reading,vi?"Mở một cuốn sách từ Thư viện":"Open a book from Library",1,2,false,fui::TextAlign::Center);
  else {
    // Constant book shell: visible top plane and spine; a single flat lower edge.
    const auto shell=g.cover;
    const int depth=7,spine=7;
    const auto face=makeRect(shell.x+spine,shell.y,shell.width-spine,shell.height);
    BookCoverPainter painter(r,t);painter.paint(*featured,face,0,BookCoverPainter::Fit::Stretch);
    const fui::Point topLeft{shell.x,shell.y},topRight{shell.right(),shell.y};
    const fui::Point backRight{int16_t(shell.right()+depth),int16_t(shell.y-depth)},backLeft{int16_t(shell.x+depth),int16_t(shell.y-depth)};
    t.triangle(topLeft,topRight,backRight,white);t.triangle(topLeft,backRight,backLeft,white);
    t.line(topLeft,topRight,1,ink);t.line(topRight,backRight,1,ink);t.line(backRight,backLeft,1,ink);t.line(backLeft,topLeft,1,ink);
    t.line({int16_t(shell.x+depth),int16_t(shell.y-3)},{int16_t(shell.right()+3),int16_t(shell.y-3)},1,ink);
    t.stroke(shell,ink,1);t.line({int16_t(shell.x+spine),shell.y},{int16_t(shell.x+spine),shell.bottom()},1,ink);
    g.cat=catForCover(shell);

    text(t,g.title,featured->title.c_str(),1,2,true);
    text(t,g.author,featured->author.c_str(),0,2);
    const auto pos=loadBookPosition(featured->path);const int percent=loadBookProgress(featured->path);
    if(pos.section)std::snprintf(s,sizeof(s),vi?"Ch. %d · Trang %d":"Ch. %d · Page %d",pos.section,pos.page);
    else std::snprintf(s,sizeof(s),vi?"Trang %d":"Page %d",pos.page);
    text(t,g.position,s);
    if(percent>=0) {
      int width=std::max(0,int(g.progress.width)-55),y=g.progress.y+14;
      t.stroke(makeRect(g.progress.x,y,width,5),ink,1);t.fill(makeRect(g.progress.x,y,width*std::clamp(percent,0,100)/100,5),ink);
      std::snprintf(s,sizeof(s),"%d%%",percent);text(t,g.progress,s,0,1,false,fui::TextAlign::Right);
    }
    const auto b=g.hits[Resume];
    // A soft cloud with raised lobes; its hit region remains the full button.
    t.fill(b,white,18);t.stroke(b,ink,2,18);
    for(int i=0;i<3;++i){
      const int d=28,cx=b.x+b.width*(i+1)/4;
      t.fill(makeRect(cx-d/2,b.y-7,d,d),white,d/2);
      t.stroke(makeRect(cx-d/2,b.y-7,d,d),ink,2,d/2);
    }
    t.fill(makeRect(b.x+20,b.y+7,b.width-40,23),white);
    text(t,b,"Đọc tiếp",0,1,false,fui::TextAlign::Center);
    zenart::duo(t,g.duo);
    if(pose%5==2)zenart::corner(t,g.cat);else companionart::cat(t,g.cat,pose);
    if(focus==Resume)t.stroke(b.inset({3,3,3,3}),ink,1,16);
  }
  // One layout and one quote paint path, regardless of popup state.
  vietnamesequotes::drawSingle(t,g.quote,quote,pixelui::kSmall);
  BookCoverPainter painter(r,t);
  for(int n=0;n<int(shelf.size())&&n<shelfSize;++n){const auto b=g.hits[Recent1+n];painter.paint(shelf[n],b,0,BookCoverPainter::Fit::Contain);if(focus==Recent1+n)t.stroke(b.inset({-2,-2,-2,-2}),ink,2);}
  r.drawLine(g.shelf.x,g.shelf.bottom(),g.shelf.right(),g.shelf.bottom());
  r.drawLine(g.shelf.x,g.shelf.bottom()+3,g.shelf.right(),g.shelf.bottom()+3);
  if(g.popup.empty()) companionart::worm(t,g.mascot);
  else if(pose%2) zenart::wormRest(t,g.mascot);else zenart::wormHappy(t,g.mascot);
  const auto battery=toybox::headerStatus(r);
  if(battery.percent>=0){
    const auto bubble=makeRect(g.mascot.x-4,g.mascot.y-52,76,36);
    t.fill(bubble,white,18);t.stroke(bubble,ink,1,18);
    std::snprintf(s,sizeof(s),"%d%%",battery.percent);
    text(t,bubble,s,0,1,false,fui::TextAlign::Center);
    t.fill(makeRect(g.mascot.x+28,g.mascot.y-12,7,7),white,3);
    t.stroke(makeRect(g.mascot.x+28,g.mascot.y-12,7,7),ink,1,3);
  }
  if(focus==Menu)t.stroke(g.hits[Menu],ink,1,3);

  if(!g.popup.empty()) {
    t.fill(g.popup,white,4);t.stroke(g.popup,ink,2,4);
    auto cx=g.close.x+18,cy=g.close.y+14;
    t.line({int16_t(cx-5),int16_t(cy-5)},{int16_t(cx+5),int16_t(cy+5)},2,ink);
    t.line({int16_t(cx+5),int16_t(cy-5)},{int16_t(cx-5),int16_t(cy+5)},2,ink);
    constexpr const freeink::Icon* icons[]={&navicons::Stats32,&navicons::Calendar32,&navicons::Send32,&navicons::Gear32};
    for(int n=0;n<4;++n){const auto a=menuActions[n];auto b=g.hits[a];if(focus==a)t.stroke(b,ink,1);t.bitmap(makeRect(b.x+8,b.y+(b.height-26)/2,26,26),fui::bitmapFromIcon(*icons[n]),fui::BitmapMode::Contain,ink);text(t,makeRect(b.x+45,b.y,b.width-48,b.height),label(a));if(n<3)r.drawLine(b.x,b.bottom(),b.right(),b.bottom());}
  }
}
} // namespace readingdesk
