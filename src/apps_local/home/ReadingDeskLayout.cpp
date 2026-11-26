#include "ReadingDesk.h"
#include <algorithm>
namespace readingdesk {
namespace fui=freeink::ui;
using fui::makeRect;
namespace { constexpr Action menuActions[]={Library,Stats,Calendar,Transfer,Settings}; }
Action Layout::at(int x,int y) const {
  if(!popup.empty()) {
    if(close.contains(x,y))return Menu;
    for(auto a:menuActions)if(hits[a].contains(x,y))return a;
    return Menu; // Outside dismisses without activating the obscured screen.
  }
  if(hits[Menu].contains(x,y))return Menu;
  if(theme)for(auto a:{Apps,Library,Settings})if(hits[a].contains(x,y))return a;
  if(quote.contains(x,y))return Quote;
  if(cat.contains(x,y))return Cat;
  for(int a=Recent1;a<=Next;++a)if(hits[a].contains(x,y))return static_cast<Action>(a);
  return reading.contains(x,y)&&!hits[Resume].empty()?Resume:None;
}
Action Layout::move(Action current,int direction) const {
  if(!popup.empty()) {
    for(int n=0;n<5;++n)if(menuActions[n]==current)return menuActions[(n+direction+5)%5];
    return direction<0?Settings:Library;
  }
  constexpr Action order[]={Resume,Recent1,Recent2,Recent3,Recent4,Apps,Library,Settings,Menu};
  constexpr int size=sizeof(order)/sizeof(order[0]);
  int start=direction>0?-1:size;
  for(int n=0;n<size;++n)if(order[n]==current)start=n;
  for(int n=0;n<size;++n) {start=(start+direction+size)%size; if(!hits[order[start]].empty())return order[start];}
  return Menu;
}
Layout layout(fui::Rect safe,int count,bool hasBook,bool prev,bool next,bool menu) {
  Layout g;
  const bool wide=safe.width>safe.height;
  const int m=wide?16:18,x=safe.x+m,w=safe.width-2*m;
  g.status=makeRect(x+w*55/100,safe.y+16,w*45/100,96);
  g.heading=makeRect(g.status.x+4,g.status.y+44,g.status.width-8,48);
  const int shelfH=wide?120:174;
  g.shelf=makeRect(x,safe.bottom()-shelfH-12,w,shelfH);
  const int quoteH=48;
  g.quote=makeRect(x,g.shelf.y-quoteH-18,w,quoteH);
  const int top=safe.y+(wide?80:130);
  g.reading=makeRect(x,top,w,g.quote.y-top-12);
  const int coverH=std::max(100,int(g.reading.height)-8);
  const int coverW=std::min(w*49/100,coverH*2/3);
  g.cover=makeRect(x,g.reading.y,coverW,coverH);
  const int copyX=g.cover.right()+14,copyW=x+w-copyX;
  g.title=makeRect(copyX,g.cover.y+42,copyW,wide?40:64);
  g.author=makeRect(copyX,g.title.bottom()+4,copyW,28);
  g.position=makeRect(copyX,g.author.bottom()+4,copyW,26);
  g.progress=makeRect(copyX,g.position.bottom()+4,copyW,30);
  g.remaining=makeRect(copyX,g.progress.bottom()+2,copyW,54);
  if(hasBook)g.hits[Resume]=makeRect(copyX,g.cover.bottom()-46,copyW,44);
  const auto button=g.hits[Resume];
  g.duo=makeRect(copyX+4,button.y-38,copyW-8,60);
  g.cat=catForCover(g.cover);
  const int perch=wide?60:68;
  g.mascot=makeRect(g.shelf.right()-perch,g.shelf.bottom()-perch,perch,perch);
  g.hits[Menu]=makeRect(g.mascot.x-4,g.mascot.y-4,g.mascot.width+8,g.mascot.height+8);
  const int booksW=g.shelf.width-perch-22;
  const int col=booksW/3;
  for(int n=0;n<count&&n<3;++n) {
    const int cw=std::min(col-10,(shelfH-16)*2/3),ch=cw*3/2;
    g.hits[Recent1+n]=makeRect(x+12+n*col+(col-cw)/2,g.shelf.bottom()-ch-4,cw,ch);
  }
  if(prev)g.hits[Previous]=makeRect(x-12,g.shelf.y+20,26,g.shelf.height-40);
  if(next)g.hits[Next]=makeRect(g.shelf.right()-perch-12,g.shelf.y+20,26,g.shelf.height-perch-20);
  if(menu) {
    const int row=wide?38:48,pw=std::min(w,wide?240:264);
    g.popup=makeRect(g.shelf.right()-pw,std::max(safe.y+8,g.mascot.y-5*row-34),pw,5*row+32);
    g.close=makeRect(g.popup.right()-36,g.popup.y,36,30);
    for(int n=0;n<5;++n)g.hits[menuActions[n]]=makeRect(g.popup.x+6,g.popup.y+30+n*row,pw-12,row);
  }
  if(safe.width==480 && safe.height==800){
    g.status=makeRect(272,20,188,122);g.heading=makeRect(286,79,166,47);
    g.reading=makeRect(27,170,448,395);g.cover=makeRect(63,194,213,347);
    g.cat=makeRect(5,35,245,159);
    g.title=makeRect(296,198,177,65);g.author=makeRect(296,267,177,34);
    g.position=makeRect(296,306,177,28);g.progress=makeRect(296,334,177,29);
    g.remaining=makeRect(296,363,177,63);g.hits[Resume]=hasBook?makeRect(290,475,181,70):fui::Rect{};
    g.duo=makeRect(308,417,145,63);g.quote=makeRect(18,573,444,51);
    g.shelf=makeRect(5,627,470,168);g.mascot=makeRect(354,706,96,69);g.hits[Menu]=g.mascot;
    const fui::Rect covers[]={makeRect(24,644,86,117),makeRect(132,643,88,118),makeRect(239,642,89,119)};
    for(int n=0;n<count&&n<3;++n)g.hits[Recent1+n]=covers[n];
    if(menu){g.popup=makeRect(205,433,264,267);g.close=makeRect(433,433,36,28);
      for(int n=0;n<5;++n)g.hits[menuActions[n]]=makeRect(211,463+n*47,252,47);}
  }
  return g;
}

}
namespace readingdesk {
Layout themedLayout(Layout g,unsigned theme,int count,bool hasBook,bool prev,bool next){
  if(!theme || g.cover.x!=63 || g.cover.y!=194)return g;
  g.theme=theme;
  const auto modalHits=g.hits;
  // Reset cat-only targets. A theme is stable for the entire Home visit.
  for(auto a:{Resume,Recent1,Recent2,Recent3,Recent4,Previous,Next,Library,Apps,Settings})g.hits[a]={};
  if(theme==1){
    g.status=makeRect(180,24,134,58);g.heading=g.status;
    g.cover=makeRect(207,192,80,113);g.cat=makeRect(117,90,165,95);
    g.reading=makeRect(80,85,345,329);g.title=makeRect(134,374,216,36);
    g.author={};g.position={};g.remaining={};g.progress=makeRect(126,418,235,14);
    g.shelf=makeRect(5,442,470,150);g.quote=makeRect(124,709,235,30);
    g.mascot=makeRect(173,744,120,55);g.hits[Menu]=g.mascot;
    const fui::Rect covers[]={makeRect(51,452,76,104),makeRect(157,455,77,101),makeRect(266,455,72,101),makeRect(374,455,65,101)};
    for(int n=0;n<count&&n<4;++n)g.hits[Recent1+n]=covers[n];
    g.hits[Apps]=makeRect(35,599,114,105);g.hits[Library]=makeRect(180,599,124,105);g.hits[Settings]=makeRect(349,599,108,105);
    if(hasBook)g.hits[Resume]=g.cover;
  }else{
    g.status=makeRect(290,32,168,94);g.heading=makeRect(296,79,154,47);
    g.cover=makeRect(86,188,188,276);g.cat=makeRect(5,5,268,170);
    g.reading=makeRect(54,180,421,320);g.title=makeRect(323,176,150,58);
    g.author=makeRect(323,237,150,35);g.position=makeRect(323,277,150,25);
    g.progress=makeRect(323,305,150,26);g.remaining=makeRect(323,333,150,51);
    g.shelf=makeRect(4,588,472,196);g.quote=makeRect(20,523,440,45);
    g.mascot=makeRect(352,650,121,109);g.hits[Menu]=g.mascot;
    const fui::Rect covers[]={makeRect(27,605,86,126),makeRect(140,605,85,126),makeRect(254,605,84,126)};
    for(int n=0;n<count&&n<3;++n)g.hits[Recent1+n]=covers[n];
    if(hasBook)g.hits[Resume]=makeRect(320,445,148,44);
  }
  if(prev)g.hits[Previous]=makeRect(0,g.shelf.y,20,110);
  if(next)g.hits[Next]=makeRect(460,g.shelf.y,20,110);
  if(!g.popup.empty())for(auto a:menuActions)g.hits[a]=modalHits[a];
  // Popup remains a modal layer with the original five actions and outside-dismiss handling.
  return g;
}
}
