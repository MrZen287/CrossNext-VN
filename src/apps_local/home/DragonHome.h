#pragma once
#include <FreeInkUIIcon.h>
#include <GfxRenderer.h>
#include <HalClock.h>
#include "DragonHomeArt.h"
#include "WidgetDigits.h"
#include "ReadingDesk.h"
#include "BookCoverPainter.h"
#include "../ui/FitText.h"
#include "../../CrossPointSettings.h"
#include "../../RecentBooksStore.h"
#include "../reading/ReadingStatsStore.h"
#include "../tools/VietnameseLunar.h"
namespace dragonhome {
namespace fui=freeink::ui;
inline void draw(GfxRenderer& r,fui::DrawTarget& t,const readingdesk::Layout& g,const RecentBook* book,
 const std::vector<RecentBook>& shelf,int percent,const char* chapter,const char* remaining,const char* estimate,
 const char* quote,int battery,int today,readingdesk::Action focus){
 using fui::makeRect;
 const auto ink=fui::Paint::solid(fui::Color::Black);
 dragonart::background(t,g.theme);
 auto text=[&](fui::Rect box,const char* value,int font=0,int lines=1,fui::TextAlign align=fui::TextAlign::Left){
   if(!box.empty())fittext::draw(t,box,value,font,lines,align);
 };
 BookCoverPainter painter(r,t);
 if(book)painter.paint(*book,g.cover,0,BookCoverPainter::Fit::Stretch);
 else text(g.cover,"Mở sách từ Thư viện",0,3,fui::TextAlign::Center);
 for(int n=0;n<int(shelf.size())&&n<(g.theme==1?4:3);++n)
   painter.paint(shelf[n],g.hits[readingdesk::Recent1+n],0,BookCoverPainter::Fit::Stretch);
 // Foreground masks preserve claws and the coiled tail over live cover pixels.
 dragonart::foreground(t,g.theme);
 char s[80]{};
 if(g.theme==1){
   text(makeRect(163,354,157,18),"Đang đọc",0,1,fui::TextAlign::Center);
   text(g.title,book?book->title.c_str():"Chưa có sách",6,1,fui::TextAlign::Center);
   // Cover art contains an empty progress track; fill its interior with live progress.
   if(percent>=0){t.fill(makeRect(128,421,190*std::clamp(percent,0,100)/100,5),ink,2);
     std::snprintf(s,sizeof(s),"%d%%",percent);text(makeRect(326,414,50,22),s);}
   text(makeRect(39,685,104,23),"Tiên đan",0,1,fui::TextAlign::Center);
   text(makeRect(182,685,121,23),"Thư viện",0,1,fui::TextAlign::Center);
   text(makeRect(349,685,104,23),"Cài đặt",0,1,fui::TextAlign::Center);
   if(battery>=0){std::snprintf(s,sizeof(s),"%d%%",battery);text(makeRect(316,749,48,24),s,1,1,fui::TextAlign::Center);}
 }else{
   text(g.title,book?book->title.c_str():"Chưa có sách",6,2);
   text(g.author,book?book->author.c_str():"",0,2);text(g.position,chapter);
   if(percent>=0){const int width=g.progress.width-42;
     t.stroke(makeRect(g.progress.x,g.progress.y+9,width,6),ink,1,3);
     t.fill(makeRect(g.progress.x+1,g.progress.y+10,(width-2)*std::clamp(percent,0,100)/100,4),ink,2);
     std::snprintf(s,sizeof(s),"%d%%",percent);text(makeRect(g.progress.right()-40,g.progress.y,40,24),s);}
   text(makeRect(g.remaining.x,g.remaining.y,g.remaining.width,25),remaining);
   text(makeRect(g.remaining.x,g.remaining.y+25,g.remaining.width,26),estimate,3,1);
   text(makeRect(338,451,114,30),"Đọc tiếp",6,1,fui::TextAlign::Center);
   halClock.formatTime(s,sizeof(s),SETTINGS.clockFormat==1);
   int hour=0,minute=0;
   if(std::sscanf(s,"%d:%d",&hour,&minute)==2){
     const int digits[]={hour/10%10,hour%10,minute/10%10,minute%10};
     for(int n=0;n<4;++n)t.bitmap(makeRect(314+n*28+(n>=2?8:0),31,25,40),fui::bitmapFromIcon(*widgetdigits::digits[digits[n]]),fui::BitmapMode::Contain,ink);
     t.fill(makeRect(370,43,3,3),ink,1);t.fill(makeRect(370,57,3,3),ink,1);
   }
   if(battery>=0){std::snprintf(s,sizeof(s),"%d%%",battery);text(makeRect(391,607,61,32),s,6,1,fui::TextAlign::Center);}
 }
 if(today){
   const auto d=readingstats::dateFromDay(today);
   constexpr const char* days[]={"T2","T3","T4","T5","T6","T7","CN"};
   std::snprintf(s,sizeof(s),"%s · %02d.%02d",days[readingstats::weekday(today)],d.day,d.month);
   const auto dateBox=g.theme==1?makeRect(180,29,136,23):makeRect(296,79,154,23);
   text(dateBox,s,0,1,fui::TextAlign::Center);
   const auto l=vietnameselunar::fromCivilDay(today,d.year);
   if(l.day){std::snprintf(s,sizeof(s),"Âm %d.%d",l.day,l.month);
     text(makeRect(dateBox.x,dateBox.y+23,dateBox.width,23),s,0,1,fui::TextAlign::Center);}
 }
 fui::TextStyle qs;qs.font=0;qs.italic=true;qs.align=fui::TextAlign::Center;qs.maxLines=g.theme==1?1:2;
 fittext::draw(t,g.quote,quote?quote:"",qs,qs.maxLines);
 // All text and covers above are live; no sample titles are baked into the image.
 if(focus!=readingdesk::None && g.popup.empty()){const auto b=g.hits[focus];if(!b.empty())t.stroke(b.inset({-2,-2,-2,-2}),ink,2,3);}
}
}
