#include <cassert>
#include "../../src/util/ReadingGuides.h"
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
  assert(g.at(g.stats.x+10,g.stats.y+10,0,12)==h::Clock);
  assert(g.at(g.stats.x+10,g.stats.y+10,1,10)==h::Stats);
  for(unsigned a=0;a<4;++a)for(unsigned b=0;b<4;++b)for(unsigned c=0;c<4;++c){
   std::array<h::Widget,3> types{h::widget(a),h::widget(b),h::widget(c)};
   assert(g.at(g.date.x+10,g.date.y+10,0,12,types)==h::action(types[0]));
   assert(g.at(g.stats.x+10,g.stats.y+10,0,12,types)==h::action(types[1]));
   assert(g.at(g.stats.x+10,g.stats.y+10,1,12,types)==h::action(types[2]));
  }
  assert(g.at(g.picker.x+10,g.picker.y+10,0,12)==h::WidgetSettings);
  int expected[]={h::Library,h::Stats,h::Calendar,h::Settings};
  for(int n=0;n<4;++n)assert(g.at(g.shortcuts[n].x+10,g.shortcuts[n].y+10,1,10)==expected[n]);
  for(int n=0;n<12;++n)assert(g.at(g.cells[n].x+10,g.cells[n].y+10,0,12)==h::App+n);
  assert(g.at(g.cells[10].x+10,g.cells[10].y+10,1,10)==h::None);
 }
 auto twelve=h::clockPoint(100,100,60,0);auto three=h::clockPoint(100,100,60,15);
 auto six=h::clockPoint(100,100,60,30);auto nine=h::clockPoint(100,100,60,45);
 assert(twelve.x==100&&twelve.y==40&&three.x==160&&three.y==100);
 assert(six.x==100&&six.y==160&&nine.x==40&&nine.y==100);
 auto halfPastThree=h::clockPoint(100,100,60,17.5);assert(halfPastThree.x<160&&halfPastThree.y>100);
 for(int p:{4,6,10,16}){int dots=0,dashes=0,solid=0;for(int x=0;x<p*3;++x){dots+=readingguides::mark(0,x,p);dashes+=readingguides::mark(2,x,p);solid+=readingguides::mark(4,x,p);}assert(dots==3&&dashes==p/2*3&&solid==p*3);}
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
