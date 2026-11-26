#include "LunarSleepCalendar.h"
#include "ZenLockLayout.h"
#include "apps_local/ui/CalendarIllustration.h"
#include "LunarCoverDigits.h"
#include <I18n.h>
#include <HalClock.h>
#include <algorithm>
#include <cstdio>
#include "apps_local/reading/ReadingStatsStore.h"
#include "apps_local/tools/VietnameseLunar.h"
#include "apps_local/ui/NumberTypography.h"
#include "apps_local/ui/PixelMenuTheme.h"
#include "apps_local/ui/VietnameseQuotes.h"
namespace lunarSleepCalendar {
namespace fui=freeink::ui;
namespace {
constexpr auto ink=fui::Paint::solid(fui::Color::Black);
constexpr auto white=fui::Paint::solid(fui::Color::White);
constexpr const char* viDays[]={"THỨ HAI","THỨ BA","THỨ TƯ","THỨ NĂM","THỨ SÁU","THỨ BẢY","CHỦ NHẬT"};
constexpr const char* enDays[]={"MONDAY","TUESDAY","WEDNESDAY","THURSDAY","FRIDAY","SATURDAY","SUNDAY"};
constexpr const char* months[]={"Giêng","Hai","Ba","Tư","Năm","Sáu","Bảy","Tám","Chín","Mười","Mười Một","Chạp"};
constexpr const char* stems[]={"Giáp","Ất","Bính","Đinh","Mậu","Kỷ","Canh","Tân","Nhâm","Quý"};
constexpr const char* branches[]={"Tý","Sửu","Dần","Mão","Thìn","Tỵ","Ngọ","Mùi","Thân","Dậu","Tuất","Hợi"};
void text(fui::DrawTarget& t,fui::Rect box,const char* value,int font=0,bool bold=false,fui::Color color=fui::Color::Black) {
  fui::TextStyle s;s.font=font;s.align=fui::TextAlign::Center;s.bold=bold;s.color=color;t.text(box,value,s);
}
}
bool draw(GfxRenderer& r) {
  r.clearScreen();auto t=toybox::makeTarget(r);pixelui::bindFonts(t,r);
  t.setFont(0,toybox::kSerifSmallFontId);t.setFont(1,toybox::kSerifTileFontId);t.setFont(2,toybox::kSerifTitleFontId);
  const auto geometry=zenlock::layout(r.getScreenWidth(),r.getScreenHeight());
  const auto page=geometry.body;
  t.stroke(geometry.frame,ink,1);
  // Printed paper edge, two binding holes and a perforated tear line.
  for (int x=geometry.frame.x+12;x<geometry.frame.right()-8;x+=9)
    t.line({int16_t(x),int16_t(geometry.frame.y+24)},{int16_t(x+3),int16_t(geometry.frame.y+24)},1,ink);
  for (int x : {geometry.frame.x+geometry.frame.width/3,geometry.frame.x+geometry.frame.width*2/3})
    t.stroke(fui::makeRect(x-4,geometry.frame.y+9,8,8),ink,1,4);
  // A continuous irregular torn edge with an offset shadow, visible on 1-bit e-ink.
  const int tearY=geometry.frame.bottom()-9;
  t.fill(fui::makeRect(geometry.frame.x+1,tearY-3,geometry.frame.width-2,12),white);
  constexpr int tooth[]={0,-3,-1,-5,-2,0,-4,-1};
  for(int x=geometry.frame.x+1,n=0;x<geometry.frame.right()-1;x+=6,++n) {
    const int end=std::min(x+6,int(geometry.frame.right()-1));
    const int y1=tearY+tooth[n%8],y2=tearY+tooth[(n+1)%8];
    t.line({int16_t(x),int16_t(y1+4)},{int16_t(end),int16_t(y2+4)},1,ink);
    t.line({int16_t(x),int16_t(y1)},{int16_t(end),int16_t(y2)},2,ink);
    if(n%3==1)t.line({int16_t(x+1),int16_t(y1-2)},{int16_t(x+3),int16_t(y1-5)},1,ink);
  }
  const bool vi=I18N.getLanguage()==Language::VI;
  const int today=readingstats::today();
  if(!today) {
    text(t,fui::makeRect(page.x+12,page.y+page.height/3,page.width-24,60),"CrossZen",2);
    text(t,fui::makeRect(page.x+12,page.y+page.height/2,page.width-24,50),vi?"Vui lòng đặt ngày giờ":"Please set date and time",0);
    return true;
  }
  const auto d=readingstats::dateFromDay(today);const auto lunar=vietnameselunar::fromCivilDay(today,d.year);
  char s[192]{};
  std::snprintf(s,sizeof(s),"%s",vi?viDays[readingstats::weekday(today)]:enDays[readingstats::weekday(today)]);
  text(t,fui::makeRect(page.x+6,page.y+12,page.width-12,40),s,1,true);
  const int dateTop=page.y+62,dateHeight=page.height*24/100;
  calendarart::draw(t,fui::makeRect(page.x+4,dateTop,page.width-8,dateHeight));
  std::snprintf(s,sizeof(s),"%d",d.day);
  numerals::draw(t,fui::makeRect(page.x+70,dateTop,page.width-140,dateHeight),s,fui::Color::Black,coverFace);
  int afterDate=dateTop+dateHeight+7;
  if(vi&&lunar.day) {
    std::snprintf(s,sizeof(s),"%d tháng %s%s · %s %s",lunar.day,months[std::clamp(lunar.month,1,12)-1],lunar.leap?" nhuận":"",stems[(lunar.year+6)%10],branches[(lunar.year+8)%12]);
    text(t,fui::makeRect(page.x+6,afterDate,page.width-12,34),s,0);
    afterDate+=36;
  }
  const int mid=page.x+page.width/2;
  t.line({int16_t(page.x+30),int16_t(afterDate)},{int16_t(mid-10),int16_t(afterDate)},1,ink);
  t.line({int16_t(mid+10),int16_t(afterDate)},{int16_t(page.right()-30),int16_t(afterDate)},1,ink);
  t.line({int16_t(mid),int16_t(afterDate-4)},{int16_t(mid+4),int16_t(afterDate)},1,ink);
  t.line({int16_t(mid+4),int16_t(afterDate)},{int16_t(mid),int16_t(afterDate+4)},1,ink);
  t.line({int16_t(mid),int16_t(afterDate+4)},{int16_t(mid-4),int16_t(afterDate)},1,ink);
  t.line({int16_t(mid-4),int16_t(afterDate)},{int16_t(mid),int16_t(afterDate-4)},1,ink);
  vietnamesequotes::draw(t,fui::makeRect(page.x+24,afterDate+8,page.width-48,64),vietnamesequotes::next(),1);
  const auto grid=fui::makeRect(page.x+18,afterDate+86,page.width-36,page.bottom()-afterDate-104);
  constexpr const char* vn[]={"T2","T3","T4","T5","T6","T7","CN"};
  constexpr const char* en[]={"Mo","Tu","We","Th","Fr","Sa","Su"};
  constexpr int header=30;
  // Sparse one-bit dots keep the distant ridges lighter than the calendar text.
  constexpr int peaks[3][9]={{7,10,4,13,8,3,11,5,8},{8,4,10,6,12,7,3,9,5},{4,8,5,9,4,7,10,6,8}};
  for(int ridge=0;ridge<3;++ridge){
    for(int x=0;x<grid.width;x+=4){
      const int position=x*8*64/grid.width;
      const int segment=position/64, fraction=position%64;
      const int rise=(peaks[ridge][segment]*(64-fraction)+peaks[ridge][segment+1]*fraction)/64;
      const int y=grid.bottom()-grid.height*(29-ridge*8)/100-rise*2;
      for(int shade=0;shade<3;++shade){
        const int dotY=y+shade*4+(x/4+ridge*3+shade)%3;
        if(dotY>=grid.y+header && dotY<grid.bottom() && (x/4+shade)%4!=0)
          t.fill(fui::makeRect(grid.x+x,dotY,1,1),ink);
      }
    }
  }
  for(int col=0;col<7;++col)text(t,fui::makeRect(grid.x+col*grid.width/7,grid.y,grid.width/7,header),vi?vn[col]:en[col],0,col>=5);
  r.drawLine(grid.x,grid.y+header,grid.right(),grid.y+header);
  const int first=readingstats::dayIndex({d.year,d.month,1}),offset=readingstats::weekday(first);
  const int days=readingstats::daysInMonth(d.year,d.month),rows=(offset+days+6)/7;
  for(int day=1;day<=days;++day) {
    const int n=offset+day-1,col=n%7,row=n/7;
    const int x=grid.x+col*grid.width/7,right=grid.x+(col+1)*grid.width/7;
    const int y=grid.y+header+row*(grid.height-header)/rows,bottom=grid.y+header+(row+1)*(grid.height-header)/rows;
    const auto cell=fui::makeRect(x+2,y+2,right-x-4,bottom-y-4);
    const bool selected=day==d.day;const auto color=selected?fui::Color::White:fui::Color::Black;
    if(selected)t.fill(cell,ink,2);
    std::snprintf(s,sizeof(s),"%d",day);
    const bool dual=vi&&lunar.day;
    const int solarH=dual?cell.height*56/100:cell.height;
    text(t,fui::makeRect(cell.x,cell.y,cell.width,solarH),s,1,col>=5,color);
    if(dual) {
      const auto l=vietnameselunar::fromCivilDay(first+day-1,d.year);
      if(l.day==1)std::snprintf(s,sizeof(s),"1/%d%s",l.month,l.leap?"*":"");
      else std::snprintf(s,sizeof(s),"%d",l.day);
      text(t,fui::makeRect(cell.x,cell.y+solarH+2,cell.width,std::max(0,int(cell.height)-solarH-2)),s,0,col>=5,color);
    }
  }
  return true;
}
void footer(GfxRenderer& r,bool framed) {
  auto t=toybox::makeTarget(r);pixelui::bindFonts(t,r);
  const auto g=zenlock::layout(r.getScreenWidth(),r.getScreenHeight());
  auto rect=framed?g.footer:fui::makeRect(12,r.getScreenHeight()-36,r.getScreenWidth()-24,32);
  t.fill(rect,white);
  if(framed) {
    t.stroke(g.frame,ink,1);
    t.line({int16_t(g.frame.x+6),int16_t(g.divider)},{int16_t(g.frame.right()-6),int16_t(g.divider)},1,ink);
  }
  char value[32]="--:--";halClock.formatTime(value,sizeof(value),SETTINGS.clockFormat==1);
  fui::TextStyle style;style.font=pixelui::kSmall;style.align=fui::TextAlign::Left;t.text(rect,value,style);
  const auto status=toybox::headerStatus(r);
  if(status.percent>=0)std::snprintf(value,sizeof(value),"%d%%",status.percent);else std::snprintf(value,sizeof(value),"--%%");
  style.align=fui::TextAlign::Right;t.text(rect,value,style);
}

}
