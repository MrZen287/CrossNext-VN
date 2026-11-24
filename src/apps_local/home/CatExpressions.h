#pragma once
#include <FreeInkUI.h>
namespace catexpressions {
namespace fui=freeink::ui;
inline constexpr unsigned count=6;
inline unsigned next(unsigned current){return (current+1)%count;}
inline void draw(fui::DrawTarget& t,unsigned expression){
 const unsigned e=expression%count;if(!e)return;
 const auto ink=fui::Paint::solid(fui::Color::Black),white=fui::Paint::solid(fui::Color::White);
 // Only skin inside the face is repainted. Head outline, ears, body and paws never move.
 t.fill(fui::makeRect(137,113,40,24),white,12);t.fill(fui::makeRect(188,91,36,24),white,12);
 t.fill(fui::makeRect(169,118,34,23),white,10);
 auto line=[&](int x,int y,int a,int b,int w=2){t.line({int16_t(x),int16_t(y)},{int16_t(a),int16_t(b)},w,ink);};
 auto closed=[&](int x,int y){line(x,y+6,x+5,y+1);line(x+5,y+1,x+12,y);line(x+12,y,x+20,y+3);};
 auto eye=[&](int x,int y,bool wide){int d=wide?13:10;t.stroke(fui::makeRect(x,y,d,d+2),ink,1,d/2);t.fill(fui::makeRect(x+4,y+3,4,7),ink,2);};
 if(e==1||e==4){eye(146,117,e==4);eye(199,96,e==4);}
 else if(e==2){closed(142,120);eye(200,96,false);}
 else if(e==3){line(143,125,162,120);line(198,105,216,100);}
 else {closed(142,119);closed(195,97);}
 if(e==3||e==4)t.stroke(fui::makeRect(181,126,e==4?8:11,e==4?10:12),ink,1,5);
 else {line(182,123,182,127,1);line(182,127,177,131,1);line(177,131,172,129,1);line(182,127,188,129,1);line(188,129,194,125,1);}
 if(e==5)for(int i=0;i<4;++i){line(143+i*3,138,144+i*3,141,1);line(207+i*3,118,208+i*3,121,1);}
}
}
