#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>

namespace edgelight {
enum class Action : uint8_t { None, Brightness, Warmth };
struct Event {
  Action action = Action::None;
  int8_t step = 0;
};
// Side-band drag semantics inspired by CrossInk's EdgeSlide. No dependency on its input stack.
class Drag {
 public:
  void begin(int x,int y,int width,int height,uint8_t left,uint8_t right) {
    reset();band=std::max(32,width*8/100);w=width;h=height;
    sx=x;sy=last=y;side=x>=0&&x<band?1:x>=width-band&&x<width?2:0;
    const unsigned value=side==1?left:side==2?right:0;
    if(value>0 && value<=uint8_t(Action::Warmth))action=Action(value);
  }
  Event move(int x,int y) {
    if(action==Action::None || cancelled)return {};
    const bool inside=x>=0&&x<w&&y>=0&&y<h&&(side==1?x<band:x>=w-band);
    if(!inside){cancelled=true;return {};}
    const int distance=y-sy;
    if(!owned) {
      if(std::abs(distance)<std::max(60,h*6/100) || std::abs(distance)*2<std::abs(x-sx)*3)return {};
      owned=true;
    }
    const int stepSize=std::max(20,h*3/100);
    const int steps=(last-y)/stepSize;
    if(!steps)return {};
    last-=steps*stepSize;
    return {action,int8_t(std::clamp(steps*5,-100,100))};
  }
  bool claimed() const{return owned;}
  void reset(){action=Action::None;owned=false;cancelled=false;side=0;}
 private:
  Action action=Action::None;
  bool owned=false,cancelled=false;
  int sx=0,sy=0,last=0,band=0,w=0,h=0,side=0;
};
inline Event classify(int sx, int sy, int ex, int ey, int width, int height, uint8_t left, uint8_t right) {
  if (width <= 0 || height <= 0 || sx < 0 || sx >= width || sy < 0 || sy >= height || std::abs(ey - sy) < 32 ||
      std::abs(ey - sy) <= std::abs(ex - sx) * 2)
    return {};
  const int edge = std::max(20, width * 5 / 100);
  const unsigned action = sx < edge ? left : sx >= width - edge ? right : 0;
  if (!action || action > uint8_t(Action::Warmth)) return {};
  return {Action(action), int8_t(ey < sy ? 10 : -10)};
}
}  // namespace edgelight
