#include <cassert>
#include <iostream>
#include "../../src/apps_local/home/HomeLauncher.h"
#include "../../src/util/EdgeLightGesture.h"
#include "../../src/util/HomeButtonInput.h"
int main(){
 namespace h=homelauncher;
 for(auto size:{freeink::ui::Size{480,800},freeink::ui::Size{480,720}}){
  auto g=h::layout(freeink::ui::makeRect(0,0,size.width,size.height));
  assert(g.widgets.width==g.date.width+g.stats.width+12);
  assert(g.grid.bottom()<g.dock.y);assert(g.dock.bottom()<size.height);
  assert(g.at(g.date.x+10,g.date.y+10,0,12)==h::Calendar);
  assert(g.at(g.stats.x+10,g.stats.y+10,0,12)==h::Stats);
  assert(g.at(g.stats.x+10,g.stats.y+10,1,10)==h::Calendar);
  int expected[]={h::Library,h::Stats,h::Calendar,h::Settings};
  for(int n=0;n<4;++n)assert(g.at(g.shortcuts[n].x+10,g.shortcuts[n].y+10,1,10)==expected[n]);
  for(int n=0;n<12;++n)assert(g.at(g.cells[n].x+10,g.cells[n].y+10,0,12)==h::App+n);
  assert(g.at(g.cells[10].x+10,g.cells[10].y+10,1,10)==h::None);
 }
 using A=HomeButtonAction;HomeButtonInput home;
 assert(home.update(1000,true,false,false,false,A::NextPage,A::Home,A::ReaderMenu)==A::Ignore);
 assert(home.update(1351,false,false,false,false,A::NextPage,A::Home,A::ReaderMenu)==A::NextPage);
 assert(home.update(2000,true,false,false,false,A::NextPage,A::Home,A::ReaderMenu)==A::Ignore);
 assert(home.update(2200,true,false,false,false,A::NextPage,A::Home,A::ReaderMenu)==A::Home);
 assert(home.update(2600,false,false,false,false,A::NextPage,A::Home,A::ReaderMenu)==A::Ignore);
 edgelight::Drag d;
 d.begin(10,400,480,800,1,2);assert(d.move(11,380).action==edgelight::Action::None);
 auto e=d.move(12,330);assert(e.action==edgelight::Action::Brightness&&e.step>0&&d.claimed());
 assert(d.move(70,300).action==edgelight::Action::None);assert(d.move(10,250).action==edgelight::Action::None);
 d.reset();d.begin(470,400,480,800,1,2);e=d.move(465,470);assert(e.action==edgelight::Action::Warmth&&e.step<0);
 d.reset();d.begin(240,400,480,800,1,2);assert(d.move(240,200).action==edgelight::Action::None);
 d.reset();d.begin(10,400,480,800,0,2);assert(d.move(10,200).action==edgelight::Action::None);
 std::cout<<"Launcher widgets/dock/apps, Home tap arbitration and side-edge drag checks passed\n";
}
