#include "ToolsScreens.h"

#include <I18n.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "../reading/ReadingStatsCore.h"
#include "../ui/Dashboard.h"
#include "../ui/PixelMenu.h"
#include "../ui/ToolsDigits.h"
#include "../ui/ToyboxIcons.h"
#include "VietnameseLunar.h"
#include "CalendarLabels.h"
#include "Almanac.h"
#include "../ui/CalendarIllustration.h"
#include "../../activities/boot_sleep/LunarCoverDigits.h"
#include "../ui/FitText.h"

namespace toolsui {
namespace fui = freeink::ui;
namespace {
constexpr auto ink = fui::Paint::solid(fui::Color::Black);
void text(toybox::Screen& s, fui::Rect r, const char* label, int font = 1) {
  pixelui::text(s.target(), r, label, font, fui::Color::Black, fui::TextAlign::Center);
}
void button(toybox::Screen& s, Work& w, fui::Rect r, const char* label, fui::ActionId action, bool selected = false) {
  pixelui::copyCaps(w.label, sizeof(w.label), label);
  w.button = {};
  w.button.label = w.label;
  fui::TextStyle probe;
  probe.font = 0;
  w.button.text.font = s.target().measureText(0, w.label, probe).width > r.width - 16 ? 3 : 0;
  w.button.text.align = fui::TextAlign::Center;
  w.button.styles = pixelui::buttonStyles();
  w.button.action = action;
  w.button.minTouchSize = 0;
  w.button.state = selected ? fui::StateSelected : fui::StateNormal;
  s.button(w.button, r);
}
constexpr StrId months[] = {StrId::STR_READING_MONTH_01, StrId::STR_READING_MONTH_02, StrId::STR_READING_MONTH_03,
                            StrId::STR_READING_MONTH_04, StrId::STR_READING_MONTH_05, StrId::STR_READING_MONTH_06,
                            StrId::STR_READING_MONTH_07, StrId::STR_READING_MONTH_08, StrId::STR_READING_MONTH_09,
                            StrId::STR_READING_MONTH_10, StrId::STR_READING_MONTH_11, StrId::STR_READING_MONTH_12};
constexpr StrId weekdays[] = {StrId::STR_STATS_MONDAY,   StrId::STR_STATS_TUESDAY, StrId::STR_STATS_WEDNESDAY,
                              StrId::STR_STATS_THURSDAY, StrId::STR_STATS_FRIDAY,  StrId::STR_STATS_SATURDAY,
                              StrId::STR_STATS_SUNDAY};
void caps(toybox::Screen& s, Work& w, fui::Rect r, const char* label, int font = 1) {
  pixelui::copyCaps(w.label, sizeof(w.label), label);
  text(s, r, w.label, font);
}
void control(toybox::Screen& s, Work& w, fui::Rect r, const char* value, fui::ActionId action, int argument = 0,
             bool primary = false, const freeink::Icon* icon = nullptr) {
  pixelui::copyCaps(w.label, sizeof(w.label), value);
  dashboard::button(s, r, w.label, action, argument, primary, icon);
}
struct CalendarGrid {
  fui::Rect rect;
  int offset, days, rows;
};
CalendarGrid calendarGrid(fui::Rect body, const Model& m) {
  const bool wide = body.width > body.height;
  const int navH = wide ? 42 : 58, gap = 10, rotateH = wide ? 40 : 56;
  const int footer = wide ? 2 * rotateH + 2 * gap : 3 * rotateH + 3 * gap;
  const int first = readingstats::dayIndex({m.year, m.month, 1});
  const int offset = (readingstats::weekday(first) + (m.sunday ? 1 : 0)) % 7;
  const int days = readingstats::daysInMonth(m.year, m.month);
  return {fui::makeRect(body.x, body.y + navH + gap, body.width, body.height - navH - gap - footer), offset, days,
          1 + (offset + days + 6) / 7};
}
void legacyCalendar(toybox::Screen& s, const Model& m, Work& w, fui::Rect body) {
  const bool wide = body.width > body.height;
  const int navH = wide ? 42 : 58, gap = 10, rotateH = wide ? 40 : 56;
  std::snprintf(w.label, sizeof(w.label), "%s %d", I18N.get(months[m.month - 1]), m.year);
  caps(s, w, fui::makeRect(body.x + navH + 8, body.y, body.width - 2 * navH - 16, navH), w.label);
  s.frame().hit(fui::makeRect(body.x+navH+8,body.y,body.width-2*navH-16,navH),MonthPicker);
  button(s, w, fui::makeRect(body.x, body.y, navH, navH), "<", Previous);
  button(s, w, fui::makeRect(body.right() - navH, body.y, navH, navH), ">", Next);
  const int first = readingstats::dayIndex({m.year, m.month, 1});
  const auto layout = calendarGrid(body, m);
  const int offset = layout.offset, days = layout.days, rows = layout.rows;
  const auto grid = layout.rect;
  static constexpr const char* names[]={"T2","T3","T4","T5","T6","T7","CN"};
  for (int col = 0; col < 7; ++col) {
    const int x = grid.x + col * grid.width / 7, right = grid.x + (col + 1) * grid.width / 7;
    caps(s, w, fui::makeRect(x, grid.y, right - x, grid.height / rows), names[(col + (m.sunday ? 6 : 0)) % 7],
         0);
  }
  for (int d = 1; d <= days; ++d) {
    const int n = offset + d - 1, col = n % 7, row = 1 + n / 7;
    const int x = grid.x + col * grid.width / 7, y = grid.y + row * grid.height / rows;
    const auto cell =
        fui::makeRect(x, y, grid.x + (col + 1) * grid.width / 7 - x, grid.y + (row + 1) * grid.height / rows - y);
    const bool selected = first + d - 1 == m.selected;
    if (selected) s.target().fill(cell, ink,5);
    if(first+d-1==m.today&&!selected)s.target().stroke(cell.inset({2,2,2,2}),ink,2,5);
    const auto color = selected ? fui::Color::White : fui::Color::Black;
    const auto lunar = vietnameselunar::fromCivilDay(first + d - 1, m.year);
    const bool dual=lunar.day&&cell.height>=42;
    const int solarHeight = dual ? cell.height-19 : cell.height;
    std::snprintf(w.label, sizeof(w.label), "%d", d);
    pixelui::text(s.target(), fui::makeRect(cell.x, cell.y, cell.width, solarHeight), w.label, 0, color,
                  fui::TextAlign::Center,true);
    if (dual) {
      if (lunar.day == 1) {
        std::snprintf(w.label, sizeof(w.label), "1/%d%s", lunar.month, lunar.leap ? "*" : "");
      } else {
        std::snprintf(w.label, sizeof(w.label), "%d", lunar.day);
      }
      pixelui::text(s.target(), fui::makeRect(cell.x, cell.y + solarHeight, cell.width, cell.height - solarHeight),
                    w.label, 3, color, fui::TextAlign::Center);
    }
  }
  for (int col = 1; col < 7; ++col)
    s.target().line({int16_t(grid.x + col * grid.width / 7), grid.y},
                    {int16_t(grid.x + col * grid.width / 7), grid.bottom()}, 1, fui::Paint::dither(fui::Color::LightGray));
  for (int row = 1; row < rows; ++row)
    s.target().line({grid.x, int16_t(grid.y + row * grid.height / rows)},
                    {grid.right(), int16_t(grid.y + row * grid.height / rows)}, 1, fui::Paint::dither(fui::Color::LightGray));
  lightui::outline(s.target(), grid, ink, pixelui::kBorder);
  s.frame().hit(grid, SelectDay, 0, fui::InputTouch | fui::InputPointerOnly);
  const int dayTop = grid.y + grid.height / rows;
  s.frame().navigationGrid(fui::makeRect(grid.x, dayTop, grid.width, grid.bottom() - dayTop), 7, rows - 1);
  const auto selected = readingstats::dateFromDay(m.selected);
  char date[128];
  std::snprintf(date, sizeof(date), tr(STR_TOOLS_SELECTED_DATE), I18N.get(weekdays[readingstats::weekday(m.selected)]),
                selected.day, I18N.get(months[selected.month - 1]));
  const int y = grid.bottom() + gap, half = (body.width - gap) / 2;
  const auto lunar = vietnameselunar::fromCivilDay(m.selected, selected.year);
  char lunarDate[160]{};
  const char* holiday = calendarlabels::holiday(lunar);
  if (lunar.day) {
    char converted[96];
    std::snprintf(converted, sizeof(converted),
                  lunar.leap ? tr(STR_TOOLS_LUNAR_LEAP_DATE) : tr(STR_TOOLS_LUNAR_DATE), lunar.day, lunar.month,
                  lunar.year);
    std::snprintf(lunarDate, sizeof(lunarDate), "%s%s%s", converted, holiday ? " / " : "", holiday ? holiday : "");
  }
  const int side = wide ? body.width * 22 / 100 : 0;
  const int dateX = body.x + (wide ? side + gap : 0);
  const int dateWidth = body.width - (wide ? 2 * (side + gap) : 0);
  const int dateHeight = lunar.day || m.invalidLookup ? rotateH / 2 : rotateH;
  text(s, fui::makeRect(dateX, y, dateWidth, dateHeight), date, 0);
  if (lunar.day || m.invalidLookup) {
    text(s, fui::makeRect(dateX, y + dateHeight, dateWidth, rotateH - dateHeight),
         m.invalidLookup ? tr(STR_CALENDAR_INVALID_DATE) : lunarDate, 0);
  }
  if (wide) {
    button(s, w, fui::makeRect(body.x, y, side, rotateH), tr(STR_TODAY), Today);
    button(s, w, fui::makeRect(body.right() - side, y, side, rotateH),
           m.sunday ? tr(STR_WEEK_SUNDAY) : tr(STR_WEEK_MONDAY), WeekStart);
  } else {
    button(s, w, fui::makeRect(body.x, y + rotateH + gap, half, rotateH), tr(STR_TODAY), Today);
    button(s, w, fui::makeRect(body.x + half + gap, y + rotateH + gap, half, rotateH),
           m.sunday ? tr(STR_WEEK_SUNDAY) : tr(STR_WEEK_MONDAY), WeekStart);
  }
  const int third = (body.width - 2 * gap) / 3;
  const int actionsY = body.bottom() - rotateH;
  button(s, w, fui::makeRect(body.x, actionsY, third, rotateH), tr(STR_CALENDAR_GO_SOLAR), GoToSolar);
  button(s, w, fui::makeRect(body.x + third + gap, actionsY, third, rotateH), tr(STR_CALENDAR_GO_LUNAR), GoToLunar);
  button(s, w, fui::makeRect(body.right() - third, actionsY, third, rotateH), "Chọn tháng", MonthPicker);
}
void note(toybox::Screen& s,fui::Rect r,const char* value,int font=0,bool bold=false,fui::TextAlign align=fui::TextAlign::Left){
 fui::TextStyle style;style.font=font;style.maxLines=std::max(1,int(r.height)/std::max(1,int(s.target().lineHeight(font))));style.bold=bold;style.align=align;s.target().text(r,value,style);
}
void dateFooter(toybox::Screen& s,const Model& m,Work& w,fui::Rect body){
 const int h=40,y=body.bottom()-h,third=(body.width-12)/3;
 control(s,w,fui::makeRect(body.x,y,third,h),m.moreDay?"Tờ lịch ngày":"Lịch tháng",m.moreDay?DayMore:CloseCalendarPanel);
 control(s,w,fui::makeRect(body.x+third+6,y,third,h),"‹ Ngày trước",Previous);
 control(s,w,fui::makeRect(body.x+2*third+12,y,third,h),"Ngày sau ›",Next);
 (void)m;
}
void dayDetails(toybox::Screen& s,const Model& m,Work& w,fui::Rect body){
 const auto d=readingstats::dateFromDay(m.selected);const auto l=vietnameselunar::fromCivilDay(m.selected,d.year);const auto* data=almanac::find(m.selected);
 char value[256],cc[64];const bool wide=body.width>body.height;
 if(wide&&!m.moreDay){
  const int leftW=body.width/3;auto left=fui::makeRect(body.x,body.y,leftW,body.height-48);
  calendarart::draw(s.target(),left);
  note(s,fui::makeRect(left.x,left.y,left.width,30),I18N.get(weekdays[readingstats::weekday(m.selected)]),0,true,fui::TextAlign::Center);
  std::snprintf(value,sizeof(value),"%d",d.day);numerals::draw(s.target(),fui::makeRect(left.x+20,left.y+36,left.width-40,110),value,fui::Color::Black,lunarSleepCalendar::coverFace);
  std::snprintf(value,sizeof(value),"Tháng %d · %d",d.month,d.year);note(s,fui::makeRect(left.x,left.y+154,left.width,30),value,0,true,fui::TextAlign::Center);
  almanac::canChiYear(l.year,cc,sizeof(cc));std::snprintf(value,sizeof(value),"%d.%d âm · %s",l.day,l.month,cc);note(s,fui::makeRect(left.x,left.y+190,left.width,48),value,3,false,fui::TextAlign::Center);
  const int x=left.right()+16,width=body.right()-x;int y=body.y;
  if(data){
   std::snprintf(value,sizeof(value),"%s · %s",almanac::auspiciousDay(m.selected,l)?"Hoàng đạo":"Hắc đạo",almanac::daySpirit(m.selected,l));note(s,fui::makeRect(x,y,width,30),value,0,true);y+=32;
   almanac::canChiDay(m.selected,cc,sizeof(cc));std::snprintf(value,sizeof(value),"%s · %s · Trực %s",almanac::solarTerm(data->term),cc,almanac::officer(data->officer));note(s,fui::makeRect(x,y,width,30),value,3);y+=34;
   for(int c=0;c<2;++c){const auto* mask=c?data->ji:data->yi;int id=almanac::activityAt(mask,0);std::snprintf(value,sizeof(value),"%s: %s",c?"Nên tránh":"Nên làm",id>=0?almanac::activity(id):"Không ghi mục cụ thể");note(s,fui::makeRect(x,y,width,42),value,0);y+=46;}
   control(s,w,fui::makeRect(x,body.bottom()-90,width,36),"Xem đầy đủ · Lịch dân gian",DayMore);
  }else note(s,fui::makeRect(x,y,width,80),"Dữ liệu dân gian chỉ có trong 2026–2028.");
  dateFooter(s,m,w,body);return;
 }
 if(m.moreDay){
  std::snprintf(value,sizeof(value),"%02d.%02d.%d · %d.%d âm lịch",d.day,d.month,d.year,l.day,l.month);
  note(s,fui::makeRect(body.x,body.y,body.width,32),value,1,true);
  almanac::canChiDay(m.selected,cc,sizeof(cc));std::snprintf(value,sizeof(value),"Ngày %s · %s",cc,data?almanac::solarTerm(data->term):"Ngoài dữ liệu 2026–2028");
  note(s,fui::makeRect(body.x,body.y+38,body.width,36),value);
  almanac::canChiMonth(l,cc,sizeof(cc));std::snprintf(value,sizeof(value),"Tháng %s · Trực %s",cc,data?almanac::officer(data->officer):"—");note(s,fui::makeRect(body.x,body.y+76,body.width,32),value);
  std::snprintf(value,sizeof(value),"Ngày kỵ: %s",almanac::avoidedDay(l));note(s,fui::makeRect(body.x,body.y+110,body.width,42),value);
  if(data){
   const int areaTop=body.y+162,areaH=body.height-248,rows=std::max(1,areaH/38),half=(body.width-12)/2;
   const unsigned total=std::max(almanac::activityTotal(data->yi),almanac::activityTotal(data->ji));const int pages=std::max(1,(int(total)+std::max(1,rows-1)-1)/std::max(1,rows-1));const int page=std::min(m.almanacPage,pages-1);
   for(int col=0;col<2;++col){const auto* mask=col?data->ji:data->yi;const int x=body.x+col*(half+12);note(s,fui::makeRect(x,areaTop,half,28),col?"NÊN TRÁNH":"NÊN LÀM",0,true);
    for(int n=0;n<rows-1;++n){const int id=almanac::activityAt(mask,page*(rows-1)+n);if(id<0)break;note(s,fui::makeRect(x,areaTop+30+n*38,half,36),almanac::activity(id));}
   }
   const int maxPages=std::max(1,(int(total)+std::max(1,rows-1)-1)/std::max(1,rows-1));
   control(s,w,fui::makeRect(body.x,body.bottom()-84,64,36),"‹",AlmanacPage,std::max(0,page-1));
   std::snprintf(value,sizeof(value),"%d / %d · Theo lịch dân gian",std::min(page+1,maxPages),maxPages);note(s,fui::makeRect(body.x+70,body.bottom()-84,body.width-140,36),value,3,false,fui::TextAlign::Center);
   control(s,w,fui::makeRect(body.right()-64,body.bottom()-84,64,36),"›",AlmanacPage,std::min(page+1,maxPages-1));
  }else note(s,fui::makeRect(body.x,body.y+180,body.width,70),"Chưa có dữ liệu dân gian ngoài 2026–2028.");
  dateFooter(s,m,w,body);return;
 }
 const int top=body.y,headerH=wide?116:274;const auto head=fui::makeRect(body.x,top,body.width,headerH);
 calendarart::draw(s.target(),fui::makeRect(head.x,head.y+30,head.width,headerH-76));
 note(s,fui::makeRect(head.x,head.y,head.width,32),I18N.get(weekdays[readingstats::weekday(m.selected)]),1,true,fui::TextAlign::Center);
 std::snprintf(value,sizeof(value),"%d",d.day);numerals::draw(s.target(),fui::makeRect(head.x+70,head.y+36,head.width-140,headerH-98),value,fui::Color::Black,lunarSleepCalendar::coverFace);
 std::snprintf(value,sizeof(value),"Tháng %d · %d",d.month,d.year);note(s,fui::makeRect(head.x,head.bottom()-58,head.width,28),value,0,true,fui::TextAlign::Center);
 almanac::canChiYear(l.year,cc,sizeof(cc));std::snprintf(value,sizeof(value),"%d tháng %d%s · %s",l.day,l.month,l.leap?" nhuận":"",cc);note(s,fui::makeRect(head.x,head.bottom()-28,head.width,28),value,0,false,fui::TextAlign::Center);
 int y=head.bottom()+8;const int available=body.bottom()-48-y;const int compact=available<400;
 s.target().line({head.x,int16_t(y)},{head.right(),int16_t(y)},1,ink);y+=8;
 if(!data){note(s,fui::makeRect(body.x,y,body.width,100),"Lịch âm xem được bình thường. Dữ liệu tốt/xấu chỉ có trong 2026–2028.");dateFooter(s,m,w,body);return;}
 std::snprintf(value,sizeof(value),"%s · %s",almanac::auspiciousDay(m.selected,l)?"HOÀNG ĐẠO":"HẮC ĐẠO",almanac::daySpirit(m.selected,l));note(s,fui::makeRect(body.x,y,body.width,32),value,0,true,fui::TextAlign::Center);y+=34;
 almanac::canChiDay(m.selected,cc,sizeof(cc));std::snprintf(value,sizeof(value),"%s · Ngày %s · Trực %s",almanac::solarTerm(data->term),cc,almanac::officer(data->officer));note(s,fui::makeRect(body.x,y,body.width,compact?28:38),value,0,false,fui::TextAlign::Center);y+=compact?30:40;
 value[0]=0;size_t at=0;for(int h=0;h<12;++h)if(almanac::auspiciousHour(m.selected,h)){const int start=h?2*h-1:23,end=h?2*h+1:1;const int written=std::snprintf(value+at,sizeof(value)-at,"%s%s %02d–%02d",at?" · ":"",almanac::branch(h),start,end);if(written>0)at=std::min(sizeof(value)-1,at+size_t(written));}
 note(s,fui::makeRect(body.x,y,body.width,22),"Giờ hoàng đạo",0,true);y+=24;note(s,fui::makeRect(body.x,y,body.width,compact?38:52),value,3);y+=compact?42:56;
 for(int col=0;col<2;++col){const auto* mask=col?data->ji:data->yi;value[0]=0;size_t nchar=0;for(int n=0;n<3;++n){const int id=almanac::activityAt(mask,n);if(id<0)break;const int wrote=std::snprintf(value+nchar,sizeof(value)-nchar,"%s%s",n?" · ":"",almanac::activity(id));if(wrote>0)nchar=std::min(sizeof(value)-1,nchar+size_t(wrote));}
  const int h=compact?42:54;note(s,fui::makeRect(body.x,y,body.width,20),col?"NÊN TRÁNH":"NÊN LÀM",3,true);note(s,fui::makeRect(body.x,y+20,body.width,h-20),value,3);y+=h+4;
 }
 const int buttonY=std::max(y,body.bottom()-86);control(s,w,fui::makeRect(body.x,buttonY,body.width,34),"Xem đầy đủ · Theo lịch dân gian",DayMore);
 dateFooter(s,m,w,body);
}
void monthPicker(toybox::Screen& s,const Model& m,Work& w,fui::Rect body){
 text(s,fui::makeRect(body.x,body.y,body.width,42),"Chọn tháng và năm",1);
 button(s,w,fui::makeRect(body.x,body.y+50,52,44),"‹",Previous);button(s,w,fui::makeRect(body.right()-52,body.y+50,52,44),"›",Next);
 char value[24];std::snprintf(value,sizeof(value),"%d",m.year);text(s,fui::makeRect(body.x+60,body.y+50,body.width-120,44),value,1);
 const bool wide=body.width>body.height;const int cols=wide?4:3,rows=wide?3:4;
 const int gridTop=body.y+(wide?100:110),rowH=std::min(66,std::max(30,(body.height-(wide?210:240))/rows)),width=(body.width-(cols-1)*8)/cols;
 for(int month=1;month<=12;++month){std::snprintf(value,sizeof(value),"Tháng %d",month);control(s,w,fui::makeRect(body.x+((month-1)%cols)*(width+8),gridTop+((month-1)/cols)*(rowH+6),width,rowH),value,PickMonth,month,month==m.month);}
 const int half=(body.width-10)/2;button(s,w,fui::makeRect(body.x,body.bottom()-96,half,42),"Tra ngày dương",GoToSolar);button(s,w,fui::makeRect(body.x+half+10,body.bottom()-96,half,42),"Tra ngày âm",GoToLunar);button(s,w,fui::makeRect(body.x,body.bottom()-42,body.width,42),"Quay về lịch tháng",CloseCalendarPanel);
}
void holidays(toybox::Screen& s,const Model& m,Work& w,fui::Rect body){
 note(s,fui::makeRect(body.x,body.y,body.width,34),"Ngày lễ và ngày đáng nhớ",1,true);
 struct Event{int month,day;const char* name;bool lunar;};
 constexpr Event events[]={{1,1,"Tết Dương lịch",false},{1,1,"Tết Nguyên đán",true},{3,10,"Giỗ Tổ Hùng Vương",true},{4,30,"Ngày Thống nhất",false},{5,1,"Ngày Quốc tế Lao động",false},{9,2,"Quốc khánh",false},{8,15,"Tết Trung thu",true},{11,20,"Ngày Nhà giáo Việt Nam",false},{12,22,"Ngày thành lập QĐND Việt Nam",false}};
 const int cols=body.width>body.height?2:1,rows=(9+cols-1)/cols;const int rowH=std::max(28,(body.height-88)/rows),width=(body.width-(cols-1)*12)/cols;int n=0;for(const auto& e:events){char value[128];std::snprintf(value,sizeof(value),"%02d.%02d%s · %s",e.day,e.month,e.lunar?" âm":"",e.name);note(s,fui::makeRect(body.x+(n/rows)*(width+12),body.y+46+(n%rows)*rowH,width,rowH),value,0);++n;}
 note(s,fui::makeRect(body.x,body.bottom()-38,body.width,30),"Ngày kỷ niệm, không phải lịch nghỉ bù.",3);(void)m;(void)w;
}
void calendar(toybox::Screen& s,const Model& m,Work& w,fui::Rect body){
 if(m.dayDetail){dayDetails(s,m,w,body);return;}if(m.monthPicker){monthPicker(s,m,w,body);return;}
 const auto tabs=fui::makeRect(body.x,body.y,body.width,38);const int half=(body.width-8)/2;
 control(s,w,fui::makeRect(tabs.x,tabs.y,half,38),"Tháng",CalendarMonths,0,!m.holidays);control(s,w,fui::makeRect(tabs.x+half+8,tabs.y,half,38),"Ngày lễ",CalendarHolidays,0,m.holidays);
 body.y+=48;body.height-=48;
 if(m.holidays)holidays(s,m,w,body);else legacyCalendar(s,m,w,body);
}
void face(toybox::Screen& s, fui::Rect box, const Model& m) {
  const int size = std::min(int(box.width), int(box.height)), cx = box.x + box.width / 2, cy = box.y + box.height / 2,
            r = size / 2;
  toybox::ring(s, cx, cy, r, 3, fui::Color::Black, fui::Color::White);
  for (int n = 0; n < 60; ++n) {
    const float a = n * 3.14159265f / 30;
    const int outer = r - 8, inner = outer - (n % 5 ? std::max(5, size / 40) : size / 18);
    s.target().line({int16_t(cx + std::sin(a) * inner), int16_t(cy - std::cos(a) * inner)},
                    {int16_t(cx + std::sin(a) * outer), int16_t(cy - std::cos(a) * outer)}, n % 5 ? 1 : 3, ink);
  }
  const int n = size / 7, inset = size / 10;
  toolsdigits::draw(s, fui::makeRect(cx - n / 2, cy - r + inset, n, n), "12");
  toolsdigits::draw(s, fui::makeRect(cx + r - inset - n, cy - n / 2, n, n), "3");
  toolsdigits::draw(s, fui::makeRect(cx - n / 2, cy + r - inset - n, n, n), "6");
  toolsdigits::draw(s, fui::makeRect(cx - r + inset, cy - n / 2, n, n), "9");
  const float angles[] = {float((m.hour % 12) * 60 + m.minute) * 3.14159265f / 360, float(m.minute) * 3.14159265f / 30};
  for (int n = 0; n < 2; ++n)
    s.target().line({int16_t(cx), int16_t(cy)},
                    {int16_t(cx + std::sin(angles[n]) * size * (n ? .40f : .25f)),
                     int16_t(cy - std::cos(angles[n]) * size * (n ? .40f : .25f))},
                    n ? 4 : 7, ink);
  toybox::disc(s, cx, cy, std::max(5, size / 40), fui::Color::Black);
}
void clock(toybox::Screen& s, const Model& m, Work& w, fui::Rect body) {
  const bool wide = body.width > body.height;
  const int footer = wide ? 58 : 62;
  const auto content = fui::makeRect(body.x, body.y, body.width, body.height - footer - 18);
  const int weekday = readingstats::weekday(readingstats::dayIndex({m.year, m.month, std::max(1, m.day)}));
  char time[16], date[64];
  std::snprintf(time, sizeof(time), "%02d:%02d", m.twelveHour ? ((m.hour + 11) % 12 + 1) : m.hour, m.minute);
  dashboard::date(date, sizeof(date), m.year, m.month, m.day);
  fui::Rect info = content;
  if (m.analog) {
    if (wide) {
      const int side = std::min(int(content.height), int(content.width) * 48 / 100);
      face(s, fui::makeRect(content.x, content.y, side, content.height), m);
      info = fui::makeRect(content.x + side + 16, content.y, content.width - side - 16, content.height);
      const int valueH = std::min(104, info.height / 2), top = info.y + (info.height - valueH - 76) / 2;
      toolsdigits::draw(s, fui::makeRect(info.x + 12, top, info.width - 24, valueH), time);
      caps(s, w, fui::makeRect(info.x, top + valueH + 8, info.width, 40), I18N.get(weekdays[weekday]), 1);
      text(s, fui::makeRect(info.x, top + valueH + 44, info.width, 32), m.validTime ? date : tr(STR_READING_CLOCK), 0);
    } else {
      const int offset = std::min(18, std::max(0, int(content.height) - int(content.width) - 110));
      const int side = std::min(int(content.width), int(content.height) - 100 - offset);
      face(s, fui::makeRect(content.x, content.y + offset, content.width, side), m);
      toolsdigits::draw(s, fui::makeRect(content.x, content.y + side + offset + 12, content.width, 50), time);
      std::snprintf(w.label, sizeof(w.label), "%s · %s", I18N.get(weekdays[weekday]),
                    m.validTime ? date : tr(STR_READING_CLOCK));
      caps(s, w, fui::makeRect(content.x, content.y + side + offset + 66, content.width, 34), w.label, 0);
    }
  } else {
    const auto dateBand = fui::makeRect(content.x, content.bottom() - (wide ? 126 : 150), content.width, 76);
    const auto weekdayBand = fui::makeRect(content.x, content.y + (wide ? 0 : 48), content.width, 76);
    const auto clockLabel = [&](fui::Rect box, const char* label) {
      fui::TextStyle style;
      style.font = 4;
      const auto ink = s.target().inkBounds(4, label, style);
      const bool fits = s.target().measureText(4, label, style).width <= box.width - 16 && ink.height <= box.height;
      text(s, box, label, fits ? 4 : 1);
    };
    clockLabel(weekdayBand, I18N.get(weekdays[weekday]));
    toolsdigits::draw(s,
                      fui::makeRect(content.x + 8, weekdayBand.bottom() + 8, content.width - 16,
                                    dateBand.y - weekdayBand.bottom() - 16),
                      time);
    clockLabel(dateBand, m.validTime ? date : tr(STR_READING_CLOCK));
  }
  if (!m.analog) {
    const auto chip = fui::makeRect(content.x + (content.width - 136) / 2, content.bottom() - 42, 136, 34);
    button(s, w, chip, m.twelveHour ? (m.hour < 12 ? tr(STR_TOOLS_12_AM) : tr(STR_TOOLS_12_PM)) : tr(STR_TOOLS_24H),
           HourFormat);
  }
  const int half = (body.width - 12) / 2, y = body.bottom() - footer;
  control(s, w, fui::makeRect(body.x, y, half, footer), m.analog ? tr(STR_CLOCK_DIGITAL) : tr(STR_CLOCK_ANALOG),
          Appearance, 0, false, m.analog ? &extra_cards_32 : &extra_clock_32);
  control(s, w, fui::makeRect(body.x + half + 12, y, half, footer), tr(STR_QUICK_ROTATE), Rotate, 0, false,
          m.rotateRight ? &extra_rotate_cw_32 : &extra_rotate_ccw_32);
}
void counter(toybox::Screen& s, const Model& m, Work& w, fui::Rect body) {
  const bool wide = body.width > body.height;
  const auto number = wide ? fui::makeRect(body.x, body.y, body.width / 2 - 8, body.height)
                           : fui::makeRect(body.x, body.y + 38, body.width, body.height * 40 / 100);
  text(s, fui::makeRect(number.x, number.y, number.width, 42), tr(STR_COUNTER_CURRENT), 0);
  const auto display = fui::makeRect(number.x, number.y + 48, number.width, number.height - 48);
  dashboard::frame(s, display, pixelui::kBorder);
  std::snprintf(w.label, sizeof(w.label), "%lld", static_cast<long long>(m.count));
  toolsdigits::counter(s, display.inset(fui::Insets{24, 20, 24, 20}), w.label, wide ? 112 : 120);
  const auto controls =
      wide ? fui::makeRect(number.right() + 16, body.y, body.right() - number.right() - 16, body.height)
           : fui::makeRect(body.x, number.bottom() + 16, body.width, body.bottom() - number.bottom() - 16);
  const int gap = 12, h = (controls.height - 2 * gap) / 3, half = (controls.width - gap) / 2;
  control(s, w, fui::makeRect(controls.x, controls.y, controls.width, h), tr(STR_TOOLS_ADD), Add, 0, true);
  const int plusY = controls.y + h / 2;
  s.target().fill(fui::makeRect(controls.x + 28, plusY - 3, 40, 6), dashboard::paper);
  s.target().fill(fui::makeRect(controls.x + 45, plusY - 20, 6, 40), dashboard::paper);
  pixelui::text(s.target(), fui::makeRect(controls.right() - 60, controls.y, 48, h), "+1", 1, fui::Color::White,
                fui::TextAlign::Right);
  control(s, w, fui::makeRect(controls.x, controls.y + h + gap, half, h), tr(STR_TOOLS_SUBTRACT), Subtract);
  control(s, w, fui::makeRect(controls.x + half + gap, controls.y + h + gap, half, h), tr(STR_UNDO), Undo, 0, false,
          &extra_dashboard_undo_32);
  control(s, w, fui::makeRect(controls.x, controls.y + 2 * (h + gap), controls.width, h), tr(STR_COUNTER_RESET), Reset,
          0, false, &extra_rotate_cw_32);
}

}  // namespace
int calendarDayAt(const fui::DeviceContext& device, const Model& m, int x, int y) {
  auto body = pixelui::geometry(device).body;
  if (m.failed) {
    body.y += 50;
    body.height -= 50;
  }
  body.y+=48;body.height-=48;
  const auto layout = calendarGrid(body, m);
  const auto grid = layout.rect;
  if (x < grid.x || x >= grid.right() || y < grid.y || y >= grid.bottom()) return 0;
  const int col = ((x - grid.x + 1) * 7 - 1) / grid.width, row = ((y - grid.y + 1) * layout.rows - 1) / grid.height;
  if (!row) return 0;
  const int day = (row - 1) * 7 + col - layout.offset + 1;
  return day >= 1 && day <= layout.days ? day : 0;
}
void build(toybox::Screen& s, const Model& m, Work& w) {
  if(m.kind==Kind::Calendar&&m.dayDetail){
    const auto safe=s.device().safeRect();const auto paper=fui::makeRect(safe.x+6,safe.y+6,safe.width-12,safe.height-58);
    s.target().stroke(paper,ink,1);
    for(int x: {paper.x+paper.width/3,paper.x+2*paper.width/3})s.target().stroke(fui::makeRect(x-3,paper.y+6,6,6),ink,1,3);
    const auto body=fui::makeRect(safe.x+14,safe.y+24,safe.width-28,safe.height-30);
    backui::publish(true,fui::makeRect(body.x,body.bottom()-40,48,40));
    dayDetails(s,m,w,body);return;
  }
  pixelui::begin(s);
  pixelui::Header header;
  header.back = true;
  header.title = m.kind == Kind::Calendar ? tr(STR_CALENDAR_APP)
                 : m.kind == Kind::Clock  ? tr(STR_CLOCK)
                                          : tr(STR_COUNTER_APP);
  pixelui::header(s, header);
  backui::publish(true, pixelui::geometry(s.device()).back);
  auto body = s.body();
  if (m.failed) {
    text(s, fui::makeRect(body.x, body.y, body.width, 46), tr(STR_TOOLS_SAVE_FAILED), 3);
    body.y += 50;
    body.height -= 50;
  }
  switch (m.kind) {
    case Kind::Calendar:
      calendar(s, m, w, body);
      break;
    case Kind::Clock:
      clock(s, m, w, body);
      break;
    case Kind::Counter:
      if (m.confirm) {
        text(s, fui::makeRect(body.x, body.y, body.width, 110), tr(STR_COUNTER_CONFIRM), 0);
        button(s, w, fui::makeRect(body.x, body.y + 140, body.width, 64), tr(STR_RESET), ConfirmReset);
        button(s, w, fui::makeRect(body.x, body.y + 216, body.width, 64), tr(STR_CANCEL), CancelReset);
        break;
      }
      counter(s, m, w, body);
      break;
  }
}
}  // namespace toolsui
