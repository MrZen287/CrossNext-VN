#include <cassert>
#include <array>
#include "../../src/apps_local/home/HomeTheme.h"
#include "../../src/apps_local/home/ReadingDesk.h"
int main(){
 using namespace readingdesk;
 namespace fui=freeink::ui;
 std::array<int,3> counts{};
 for(unsigned r=0;r<1000;++r)++counts[hometheme::choose(r)];
 assert(counts[0]==400&&counts[1]==200&&counts[2]==400);
 for(unsigned theme=0;theme<3;++theme)for(int count=0;count<=(theme==1?4:3);++count){
  const auto base=layout(fui::makeRect(0,0,480,800),count,true,true,true,false);
  const auto g=themedLayout(base,theme,count,true,true,true);
  for(int n=0;n<count;++n){auto b=g.hits[Recent1+n];assert(!b.empty());
   assert(b.x>=0&&b.right()<=480&&b.y>=0&&b.bottom()<=800);
   assert(g.at(b.x+b.width/2,b.y+b.height/2)==Recent1+n);
  }
  auto b=g.cover;assert(g.at(b.x+b.width/2,b.y+b.height/2)==Resume);
  for(auto a:{Previous,Next,Menu}){b=g.hits[a];assert(!b.empty());assert(g.at(b.x+b.width/2,b.y+b.height/2)==a);}
  if(theme==1)for(auto a:{Apps,Library,Settings}){b=g.hits[a];assert(g.at(b.x+b.width/2,b.y+b.height/2)==a);}
  auto focus=None;bool reachedFourth=false;
  for(int n=0;n<12;++n){focus=g.move(focus,1);reachedFourth|=focus==Recent4;}
  if(count==4)assert(reachedFourth);
  auto open=themedLayout(layout(fui::makeRect(0,0,480,800),count,true,true,true,true),theme,count,true,true,true);
  assert(open.at(20,300)==Menu);
  for(auto a:{Library,Stats,Calendar,Transfer,Settings}){b=open.hits[a];assert(open.at(b.x+b.width/2,b.y+b.height/2)==a);}
 }
}
