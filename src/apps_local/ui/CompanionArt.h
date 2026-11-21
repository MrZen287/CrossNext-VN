#pragma once
#include <FreeInkUI.h>
#include <algorithm>
#include <cmath>

// Original code-native line artwork; no SD resources or extra framebuffer.
namespace companionart {
namespace fui = freeink::ui;
struct Pen {
  fui::DrawTarget& t;
  fui::Rect r;
  const fui::Paint ink = fui::Paint::solid(fui::Color::Black);
  const fui::Paint white = fui::Paint::solid(fui::Color::White);
  fui::Point p(int x, int y) const { return {int16_t(r.x + x * r.width / 100), int16_t(r.y + y * r.height / 100)}; }
  void line(int x, int y, int a, int b, int weight = 2) {
    t.line(p(x,y), p(a,b), std::max(1, weight * int(r.width) / 100), ink);
  }
  void oval(int x, int y, int w, int h, bool filled = false) {
    // Polygonal ellipses keep the same appearance on device and simulator.
    constexpr int n = 32;
    fui::Point points[n];
    for (int k=0;k<n;++k) {
      const float a = k * 6.2831853f / n;
      points[k] = p(x+w/2+int(std::cos(a)*w/2), y+h/2+int(std::sin(a)*h/2));
    }
    const auto center=p(x+w/2,y+h/2);
    for(int k=0;k<n;++k) t.triangle(center,points[k],points[(k+1)%n],filled?ink:white);
    if(!filled) for(int k=0;k<n;++k) t.line(points[k],points[(k+1)%n],std::max(1,int(r.width)/60),ink);
  }
};
inline void book(Pen& p) {
  p.t.triangle(p.p(10,65),p.p(50,72),p.p(7,91),p.white);
  p.line(10,65,50,72); p.line(50,72,90,65); p.line(90,65,96,91);
  p.line(96,91,50,97); p.line(50,97,4,91); p.line(4,91,10,65); p.line(50,72,50,97);
  p.line(15,75,43,80,1); p.line(15,83,43,88,1); p.line(57,80,85,75,1); p.line(57,88,85,83,1);
}
inline void worm(fui::DrawTarget& t,fui::Rect r) {
  Pen p{t,r};
  for(int i=0;i<4;++i) p.oval(88-i*13,62-i*2,18,22);
  p.oval(33,39,35,45); p.oval(19,16,49,43);
  p.line(30,18,23,5); p.line(23,5,15,5); p.oval(12,2,6,6,true);
  p.line(54,16,60,4); p.line(60,4,68,6); p.oval(66,3,6,6,true);
  p.oval(25,27,18,18); p.oval(46,27,18,18); p.line(43,34,46,34);
  p.oval(33,34,3,4,true); p.oval(53,34,3,4,true);
  p.line(38,49,43,52); p.line(43,52,49,49);
  book(p); p.oval(15,71,12,10); p.oval(72,70,12,10);
}
inline void cat(fui::DrawTarget& t,fui::Rect r,unsigned pose) {
  Pen p{t,r}; pose%=5;
  const bool sleep=pose==1, curious=pose==2, happy=pose==4;
  if(sleep) { p.oval(38,43,53,32); p.line(87,64,97,54); p.line(97,54,95,40); }
  else { p.oval(44,51,38,37); p.line(77,80,91,71); p.line(91,71,93,54); }
  // Pointed ears, head, asymmetric black patch, then expressive face.
  t.triangle(p.p(15,34),p.p(17,4),p.p(40,22),p.ink);
  t.triangle(p.p(56,20),p.p(82,6),p.p(79,40),p.ink);
  t.triangle(p.p(20,26),p.p(21,12),p.p(33,23),p.white);
  t.triangle(p.p(62,24),p.p(77,13),p.p(74,32),p.white);
  p.oval(13,20,70,49); p.oval(52,20,20,12,true);
  if(sleep||happy) {
    p.line(26,42,happy?32:31,happy?38:45); p.line(31,happy?38:45,36,42);
    p.line(56,42,61,happy?38:45); p.line(61,happy?38:45,67,42);
  } else { p.oval(28,37,6,9,true); p.oval(59,37,6,9,true); }
  p.line(44,49,48,49); p.line(48,49,46,52); p.line(46,52,42,56); p.line(46,52,51,56);
  p.line(22,51,3,47,1); p.line(21,56,2,58,1); p.line(72,51,94,47,1); p.line(73,56,95,58,1);
  const int pawY=sleep?64:pose==3?78:67;
  p.oval(16,pawY,20,13); p.oval(55,curious?79:pawY,20,13);
  if(pose==3) {p.line(66,91,84,87);p.line(84,87,91,76);}
  if(happy) {p.line(7,18,1,10);p.line(87,20,98,16);}
}
inline void logo(fui::DrawTarget& t,fui::Rect r) {
  cat(t,fui::makeRect(r.x,r.y,r.width*3/5,r.height*4/5),4);
  worm(t,fui::makeRect(r.x+r.width/2,r.y+r.height/5,r.width/2,r.height*4/5));
  Pen p{t,fui::makeRect(r.x+r.width/7,r.y+r.height/2,r.width*5/7,r.height/2)}; book(p);
}
} // namespace companionart
