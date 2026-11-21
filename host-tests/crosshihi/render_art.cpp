#include <FreeInkUI.h>
#include <iostream>
#include <cstring>
#include "../../src/apps_local/ui/CompanionArt.h"
using namespace freeink::ui;
class Svg : public DrawTarget {
 const char* color(Paint p){return p.color==Color::White?"white":"black";}
public:
 Size measureText(FontId,const char* s,TextStyle)const override{return {int16_t(std::strlen(s)*10),20};}
 int16_t lineHeight(FontId)const override{return 20;}
 void fill(Rect r,Paint p,uint8_t radius,uint8_t)override{
 std::cout<<"<rect x='"<<r.x<<"' y='"<<r.y<<"' width='"<<r.width<<"' height='"<<r.height<<"' rx='"<<int(radius)<<"' fill='"<<color(p)<<"'/>\n";}
 void stroke(Rect r,Paint p,uint8_t w,uint8_t radius,uint8_t)override{
 std::cout<<"<rect x='"<<r.x<<"' y='"<<r.y<<"' width='"<<r.width<<"' height='"<<r.height<<"' rx='"<<int(radius)<<"' fill='none' stroke='"<<color(p)<<"' stroke-width='"<<int(w)<<"'/>\n";}
 void line(Point a,Point b,uint8_t w,Paint p)override{
 std::cout<<"<path d='M"<<a.x<<' '<<a.y<<'L'<<b.x<<' '<<b.y<<"' fill='none' stroke='"<<color(p)<<"' stroke-width='"<<int(w)<<"' stroke-linecap='round'/>\n";}
 void triangle(Point a,Point b,Point c,Paint p)override{
 std::cout<<"<path d='M"<<a.x<<' '<<a.y<<'L'<<b.x<<' '<<b.y<<'L'<<c.x<<' '<<c.y<<"Z' fill='"<<color(p)<<"'/>\n";}
 void text(Rect,const char*,TextStyle)override{}
 void bitmap(Rect,BitmapRef,BitmapMode,Paint,Rotation)override{}
};
int main(){
 std::cout<<"<svg xmlns='http://www.w3.org/2000/svg' width='1000' height='340' viewBox='0 0 1000 340'><rect width='1000' height='340' fill='white'/>\n";
 Svg t;companionart::logo(t,makeRect(24,25,190,180));
 std::cout<<"<text x='115' y='245' text-anchor='middle' font-family='sans-serif' font-size='28'>CrossHihi</text>\n";
 for(int n=0;n<5;++n){int x=250+n*145;companionart::cat(t,makeRect(x,55,120,120),n);std::cout<<"<text x='"<<x+60<<"' y='205' text-anchor='middle' font-family='sans-serif' font-size='16'>Pose "<<n+1<<"</text>\n";}
 std::cout<<"<text x='500' y='300' text-anchor='middle' font-family='sans-serif' font-size='18'>Native vector artwork used by the firmware (not a device screenshot)</text></svg>";
}
