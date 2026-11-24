#pragma once
#include <FreeInkUI.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
namespace readingcharts {
namespace fui=freeink::ui;
inline uint64_t ceiling(uint64_t value) {
  uint64_t step=1;while(value/step>10&&step<1000000000000ULL)step*=10;
  return std::max<uint64_t>(2,((value+step-1)/step)*step);
}
inline void label(fui::DrawTarget& t,fui::Rect r,const char* value,bool bold=false,fui::TextAlign align=fui::TextAlign::Center){
 fui::TextStyle s;s.font=3;s.maxLines=1;s.bold=bold;s.align=align;t.text(r,value,s);
}
// Seconds are kept exact; unit conversion only affects axis labels.
inline void bars(fui::DrawTarget& t,fui::Rect r,const char* title,const uint64_t* values,int count,
                 const char* const* labels,bool hours=false,int selected=-1,bool trend=false) {
 if(r.height<96||r.width<110||count<1)return;
 const auto ink=fui::Paint::solid(fui::Color::Black),rule=fui::Paint::dither(fui::Color::LightGray);
 label(t,fui::makeRect(r.x,r.y,r.width-56,26),title,true,fui::TextAlign::Left);
 const uint64_t unit=hours?3600:60;
 uint64_t peak=0;for(int n=0;n<count;++n)peak=std::max(peak,values[n]);
 const uint64_t maxUnits=ceiling((peak+unit-1)/unit);
 const int left=r.x+42,top=r.y+55,base=r.bottom()-26,h=std::max(4,base-top),w=r.right()-left-4;
 label(t,fui::makeRect(r.right()-48,r.y+3,48,20),hours?"giờ":"phút",false,fui::TextAlign::Left);
 for(int n=0;n<3;++n){const int y=base-h*n/2;char s[24];std::snprintf(s,sizeof(s),"%llu",(unsigned long long)(maxUnits*n/2));label(t,fui::makeRect(r.x,y-9,36,18),s,false,fui::TextAlign::Right);t.line({int16_t(left),int16_t(y)},{int16_t(r.right()-4),int16_t(y)},1,n?rule:ink);}
 fui::Point previous{};
 for(int n=0;n<count;++n){
  const int x=left+w*n/count,next=left+w*(n+1)/count,cx=(x+next)/2;
  const int bh=std::clamp(int(float(values[n])/float(maxUnits*unit)*h),0,h);
  if(trend){const fui::Point p{int16_t(cx),int16_t(base-bh)};if(n)t.line(previous,p,2,ink);t.fill(fui::makeRect(cx-3,base-bh-3,6,6),ink,3);previous=p;}
  else if(bh){const auto b=fui::makeRect(x+3,base-bh,std::max(2,next-x-6),bh);t.fill(b,n==selected?ink:fui::Paint::dither(fui::Color::DarkGray));t.stroke(b,ink,1);}
  label(t,fui::makeRect(x,base+3,next-x,22),labels[n],n==selected);
  if(count<=7&&bh>0){char s[24];std::snprintf(s,sizeof(s),"%llu",(unsigned long long)(values[n]/unit));label(t,fui::makeRect(x,base-bh-22,next-x,20),s);}
 }
}
}
