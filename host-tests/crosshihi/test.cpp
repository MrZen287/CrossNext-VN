#include <cassert>
#include <iostream>
#include "../../src/activities/boot_sleep/ZenLockLayout.h"
#include "../../src/network/CrossHihiVersion.h"
#include "../../src/apps_local/home/ReadingDesk.h"
int main(){
 using namespace crosshihi;
 assert(newerVersion("1.1.1.4","1.1.1.3"));
 for(auto size:{freeink::ui::Size{480,800},freeink::ui::Size{600,800},freeink::ui::Size{800,480}}){
  const auto g=zenlock::layout(size.width,size.height);
  assert(g.frame.x==size.width-g.frame.right());
  assert(g.body.x==size.width-g.body.right());
  assert(g.body.bottom()<g.divider&&g.divider<g.footer.y);
  assert(g.footer.x>g.frame.x&&g.footer.right()<g.frame.right());
  assert(g.footer.bottom()<g.frame.bottom());
  assert(g.frame.y>0&&g.frame.bottom()<size.height);
 }

 assert(newerVersion("v1.1.1.3","1.1.1.2"));
 assert(newerVersion("v1.1.1.3","1.1.1.1"));
 assert(!newerVersion("1.1.1.1","1.1.1.1"));
 assert(!newerVersion("1.1.1.0","1.1.1.1"));
 assert(newerVersion("1.1.1.1","1.1.1.1-rc"));
 assert(!newerVersion("garbage","1.1.1.1"));
 assert(!parseVersion("1.1.1.1.7").valid);
 assert(!parseVersion("1.2").valid);
 assert(!parseVersion("1.1.100000000000000000000").valid);
 assert(newerVersion("1.2.0","1.1.9.9"));
 using namespace readingdesk;
 for(int count=0;count<=3;++count){
  auto normal=layout(freeink::ui::makeRect(0,0,480,800),count,true,true,true,false);
  auto open=layout(freeink::ui::makeRect(0,0,480,800),count,true,true,true,true);
  assert(normal.mascot.y==open.mascot.y&&normal.mascot.x==open.mascot.x);
  assert(normal.quote.y==open.quote.y&&normal.quote.x==open.quote.x&&normal.quote.width==open.quote.width);
  assert(normal.cover.y==open.cover.y&&normal.cover.height==open.cover.height);
  auto worm=normal.hits[Menu];assert(normal.at(worm.x+worm.width/2,worm.y+worm.height/2)==Menu);
  assert(open.at(20,300)==Menu); // Outside click dismisses, never launches the background book.
  assert(normal.at(normal.cover.x+5,normal.cover.y+normal.cover.height/2)==Resume);
  for(auto a:{Library,Stats,Calendar,Apps,Transfer,Settings}){
   auto hit=open.hits[a];assert(hit.height>=44);
   assert(open.at(hit.x+hit.width/2,hit.y+hit.height/2)==a);
  }
  for(int n=0;n<count;++n)assert(normal.hits[Recent1+n].right()<normal.mascot.x);
  assert(normal.shelf.bottom()<=800);
  assert(normal.cat.y>=0);
  assert(normal.shelf.bottom()<normal.status.y);
  assert(normal.heading.y==normal.status.y);
  assert(open.popup.bottom()<open.mascot.y);
  assert(normal.cover.bottom()<normal.quote.y);
 }
 for(auto cover : {freeink::ui::makeRect(18,137,199,340),freeink::ui::makeRect(18,156,199,301),freeink::ui::makeRect(18,205,199,199),freeink::ui::makeRect(50,137,135,340)}) {
  auto cat=catForCover(cover);
  assert(cat.y<cover.y);
  assert(cat.right()>cover.right());
  assert(cat.width>=100);
 }
 std::cout<<"CrossHihi version, overlay, tap targets and stationary mascot checks passed\n";
}
