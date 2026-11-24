#pragma once
#include <FreeInkUI.h>
namespace zenlock {
struct Layout { freeink::ui::Rect frame,body,footer; int divider; };
inline Layout layout(int width,int height) {
  using freeink::ui::makeRect;
  Layout g;
  g.frame=makeRect(8,8,width-16,height-16);
  g.footer=makeRect(g.frame.x+10,g.frame.bottom()-36,g.frame.width-20,28);
  g.divider=g.footer.y-6;
  g.body=makeRect(g.frame.x+10,g.frame.y+30,g.frame.width-20,g.frame.height-54);
  return g;
}
}
