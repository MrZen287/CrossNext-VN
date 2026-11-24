#pragma once
#include <FreeInkUIDisplayTarget.h>
#include <array>
struct GfxRenderer{freeink::ui::DisplayTarget&t;std::array<int,4>clip{0,0,480,800};int writes=0;std::array<int,4>getClipRect(){return clip;}void setClipRect(int x,int y,int w,int h){clip={x,y,w,h};t.setClipRect(freeink::ui::makeRect(x,y,w,h));}void fillRect(int x,int y,int w,int h,bool black){++writes;t.fill(freeink::ui::makeRect(x,y,w,h),freeink::ui::Paint::solid(black?freeink::ui::Color::Black:freeink::ui::Color::White));}void preserveImagePolarity(int,int,int,int){}void drawRect(int x,int y,int w,int h){t.stroke(freeink::ui::makeRect(x,y,w,h),freeink::ui::Paint::solid(freeink::ui::Color::Black),1);}};
