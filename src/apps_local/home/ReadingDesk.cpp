#include "ReadingDesk.h"
#include "ApprovedHomeUi.h"
#include "ZodiacHome.h"
#include "BookCoverPainter.h"
#include "CoverSceneCache.h"
#include <functional>
#include "HomePaws.h"
#include "CatExpressions.h"
#include <GfxRenderer.h>
#include <Epub.h>
#include <FsHelpers.h>
#include <Memory.h>
#include <KOReaderDocumentId.h>
#include "../../util/ReadingTimeMemo.h"
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
constexpr Action menuActions[]={Library,Stats,Calendar,Transfer,Settings};
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
          int,Action focus,const char* quote,unsigned pose,unsigned wormPose,unsigned zodiac) {
  r.clearScreen();auto t=toybox::makeTarget(r);pixelui::bindFonts(t,r);
  const bool native=r.getScreenWidth()==480 && r.getScreenHeight()==800;
  char s[160]{},chapterCopy[80]{},remainingCopy[80]{},estimateCopy[160]{};
  int livePercent=-1;
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
    if(!native){BookCoverPainter painter(r,t);painter.paint(*featured,face,0,BookCoverPainter::Fit::Stretch);}
    const fui::Point topLeft{shell.x,shell.y},topRight{shell.right(),shell.y};
    const fui::Point backRight{int16_t(shell.right()+depth),int16_t(shell.y-depth)},backLeft{int16_t(shell.x+depth),int16_t(shell.y-depth)};
    t.triangle(topLeft,topRight,backRight,white);t.triangle(topLeft,backRight,backLeft,white);
    t.line(topLeft,topRight,1,ink);t.line(topRight,backRight,1,ink);t.line(backRight,backLeft,1,ink);t.line(backLeft,topLeft,1,ink);
    t.line({int16_t(shell.x+depth),int16_t(shell.y-3)},{int16_t(shell.right()+3),int16_t(shell.y-3)},1,ink);
    t.stroke(shell,ink,1);t.line({int16_t(shell.x+spine),shell.y},{int16_t(shell.x+spine),shell.bottom()},1,ink);
    if(!native)g.cat=catForCover(shell);

    text(t,g.title,featured->title.c_str(),1,2,true);
    text(t,g.author,featured->author.c_str(),0,2);
    const auto pos=loadBookPosition(featured->path);const int percent=loadBookProgress(featured->path);livePercent=percent;
    struct Details {std::string path;uint32_t revision=0;int chapter=0,total=0;uint64_t seconds=0;bool known=false;};
    static Details info;
    const auto revision=Storage.resourceRevision()^Storage.stateFileRevision(readingstats::kJournalPath)^Storage.stateFileRevision(("/.crosspoint/epub_"+std::to_string(static_cast<unsigned long>(std::hash<std::string>{}(featured->path)))+"/progress.bin").c_str());
    if(info.path!=featured->path || info.revision!=revision){
      info={};info.path=featured->path;info.revision=revision;
      if(FsHelpers::hasReflowableBookExtension(featured->path)){
        auto epub=makeUniqueNoThrow<Epub>(featured->path,"/.crosspoint");
        if(epub && epub->load(false,true)){
          info.total=epub->getTocItemsCount();
          if(pos.section>0){const int toc=epub->getTocIndexForSpineIndex(pos.section-1);if(toc>=0&&toc<info.total)info.chapter=toc+1;}
        }
      }
      if(!readingtime::lookup(featured->path.c_str(),info.seconds,info.known)){
        struct Snapshot {readingstats::Repository repo;readingstats::Book book;};auto snap=makeUniqueNoThrow<Snapshot>();
        if(snap){auto doc=KOReaderDocumentId::calculate(featured->path);info.known=snap->repo.cachedBook(doc.c_str(),snap->book);if(info.known)info.seconds=snap->book.seconds;}
      }
    }
    if(info.chapter && info.total)std::snprintf(s,sizeof(s),"Chương %d / %d",info.chapter,info.total);
    else std::snprintf(s,sizeof(s),"Trang %d",pos.page);
    std::snprintf(chapterCopy,sizeof(chapterCopy),"%s",s);
    text(t,g.position,s);
    if(percent>=0){int width=std::max(0,int(g.progress.width)-55),y=g.progress.y+14;
      t.stroke(makeRect(g.progress.x,y,width,5),ink,1);t.fill(makeRect(g.progress.x,y,width*std::clamp(percent,0,100)/100,5),ink);
      std::snprintf(s,sizeof(s),"%d%%",percent);text(t,g.progress,s,0,1,false,fui::TextAlign::Right);
    }
    if(info.chapter&&info.total)std::snprintf(s,sizeof(s),"Còn %d chương",std::max(0,info.total-info.chapter));
    else std::snprintf(s,sizeof(s),"Chưa có số chương");
    std::snprintf(remainingCopy,sizeof(remainingCopy),"%s",s);
    text(t,makeRect(g.remaining.x,g.remaining.y,g.remaining.width,25),s);
    if(percent>=100)std::snprintf(s,sizeof(s),"Đã đọc hết");
    else if(info.known&&info.seconds>=300&&percent>=5){
      const uint64_t minutes=std::min<uint64_t>(99999,info.seconds*uint64_t(100-percent)/unsigned(percent)/60);
      std::snprintf(s,sizeof(s),"Ước tính %llu giờ %u phút",static_cast<unsigned long long>(minutes/60),unsigned(minutes%60));
    }else std::snprintf(s,sizeof(s),"Chưa đủ dữ liệu");
    std::snprintf(estimateCopy,sizeof(estimateCopy),"%s",s);
    text(t,makeRect(g.remaining.x,g.remaining.y+25,g.remaining.width,29),s);
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
    if(!native){if(pose%5==2)zenart::corner(t,g.cat);else companionart::cat(t,g.cat,pose);}
    if(focus==Resume)t.stroke(b.inset({3,3,3,3}),ink,1,16);
  }
  // One layout and one quote paint path, regardless of popup state.
  vietnamesequotes::draw(t,g.quote,quote,pixelui::kSmall);
  BookCoverPainter painter(r,t);
  for(int n=0;!native&&n<int(shelf.size())&&n<3;++n){const auto b=g.hits[Recent1+n];painter.paint(shelf[n],b,0,BookCoverPainter::Fit::Contain);if(focus==Recent1+n)t.stroke(b.inset({-2,-2,-2,-2}),ink,2);}
  r.drawLine(g.shelf.x,g.shelf.bottom(),g.shelf.right(),g.shelf.bottom());
  r.drawLine(g.shelf.x,g.shelf.bottom()+3,g.shelf.right(),g.shelf.bottom()+3);
  if(wormPose==0) companionart::worm(t,g.mascot);
  else if(wormPose==1) zenart::wormHappy(t,g.mascot);
  else zenart::wormRest(t,g.mascot);
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

  if(native){
    approvedhome::draw(t,makeRect(0,0,480,800));
    zodiacart::clearOldMascots(r,t,g);
    // The scene stores only the fixed book artwork, before any zodiac mask.
    const auto scene = makeRect(16, 180, 312, 588);
    uint64_t coverKey = 0x5a6f64696163ULL;
    const auto mix = [&](uint64_t value) { coverKey = (coverKey ^ value) * 1099511628211ULL; };
    const auto mixBook = [&](const RecentBook& item) {
      const std::hash<std::string> hash;
      mix(hash(item.path));
      mix(hash(item.coverBmpPath));
      mix(hash(item.title));
    };
    mix(static_cast<unsigned>(r.getOrientation()));
    mix(SETTINGS.screenInverted);
    mix(SETTINGS.xtcCoverRotation);
    mix(featured != nullptr);
    if(featured) mixBook(*featured);
    const int coverCount = std::min(int(shelf.size()), 3);
    mix(coverCount);
    for(int n=0;n<coverCount;++n) mixBook(shelf[n]);
    if(!homecover::restoreScene(r,scene,coverKey)){
      BookCoverPainter nativePainter(r,t);
      if(featured) nativePainter.paintFace(*featured,homecover::mainFace,g.cover.height);
      else text(t,g.cover,"Mở sách từ Thư viện",1,3,false,fui::TextAlign::Center);
      for(int n=0;n<coverCount;++n)
        nativePainter.paintFace(shelf[n],homecover::shelfFaces[n],g.hits[Recent1+n].height);
      homecover::storeScene(r);
    }
    zodiacart::draw(t,g,zodiac,wormPose);
    t.setFont(5,toybox::kHugeFontId);t.setFont(6,toybox::kReadingBoldFontId);
    char time[16]{},date[40]{},lunar[40]{};
    halClock.formatTime(time,sizeof(time),SETTINGS.clockFormat==1);
    if(today){auto d=readingstats::dateFromDay(today);constexpr const char* days[]={"T2","T3","T4","T5","T6","T7","CN"};
      std::snprintf(date,sizeof(date),"%s · %02d.%02d",days[readingstats::weekday(today)],d.day,d.month);
      auto l=vietnameselunar::fromCivilDay(today,d.year);if(l.day)std::snprintf(lunar,sizeof(lunar),"Âm %d.%d",l.day,l.month);}
    approvedhome::Info model;
    model.clock=time;model.date=date;model.lunar=lunar;model.title=featured?featured->title.c_str():"Chưa có sách";
    model.author=featured?featured->author.c_str():"";model.chapter=chapterCopy;model.remaining=remainingCopy;model.estimate=estimateCopy;
    model.percent=livePercent;model.battery=battery.percent;model.quote=quote;
    approvedhome::overlay(t,g,model);
  }

  if(!g.popup.empty()) {
    t.fill(g.popup,white,4);t.stroke(g.popup,ink,2,4);
    auto cx=g.close.x+18,cy=g.close.y+14;
    t.line({int16_t(cx-5),int16_t(cy-5)},{int16_t(cx+5),int16_t(cy+5)},2,ink);
    t.line({int16_t(cx+5),int16_t(cy-5)},{int16_t(cx-5),int16_t(cy+5)},2,ink);
    constexpr const freeink::Icon* icons[]={&navicons::Books32,&navicons::Stats32,&navicons::Calendar32,&navicons::Send32,&navicons::Gear32};
    for(int n=0;n<5;++n){const auto a=menuActions[n];auto b=g.hits[a];if(focus==a)t.stroke(b,ink,1);t.bitmap(makeRect(b.x+8,b.y+(b.height-26)/2,26,26),fui::bitmapFromIcon(*icons[n]),fui::BitmapMode::Contain,ink);text(t,makeRect(b.x+45,b.y,b.width-48,b.height),label(a));if(n<4)r.drawLine(b.x,b.bottom(),b.right(),b.bottom());}
  }
}
} // namespace readingdesk
