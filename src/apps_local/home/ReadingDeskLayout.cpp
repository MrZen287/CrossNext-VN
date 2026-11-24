#include "ReadingDesk.h"
#include <algorithm>
namespace readingdesk {
namespace fui=freeink::ui;
using fui::makeRect;
namespace { constexpr Action menuActions[]={Library,Stats,Calendar,Transfer,Settings}; }
Action Layout::at(int x,int y) const {
  if(!popup.empty()) {
    if(close.contains(x,y)||hits[Menu].contains(x,y))return Menu;
    for(auto a:menuActions)if(hits[a].contains(x,y))return a;
    return Menu; // Outside dismisses without activating the obscured screen.
  }
  if(hits[Menu].contains(x,y))return Menu;
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
  constexpr Action order[]={Resume,Recent1,Recent2,Recent3,Menu};
  int start=direction>0?-1:5;
  for(int n=0;n<5;++n)if(order[n]==current)start=n;
  for(int n=0;n<5;++n) {start=(start+direction+5)%5; if(!hits[order[start]].empty())return order[start];}
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
  const int quoteH=34;
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
  const int col=booksW/shelfSize;
  for(int n=0;n<count&&n<shelfSize;++n) {
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
    g.duo=makeRect(308,417,145,63);g.quote=makeRect(18,582,444,38);
    g.shelf=makeRect(5,627,470,168);g.mascot=makeRect(354,706,96,69);g.hits[Menu]=g.mascot;
    const fui::Rect covers[]={makeRect(24,644,86,117),makeRect(132,643,88,118),makeRect(239,642,89,119)};
    for(int n=0;n<count&&n<shelfSize;++n)g.hits[Recent1+n]=covers[n];
    if(menu){g.popup=makeRect(205,433,264,267);g.close=makeRect(433,433,36,28);
      for(int n=0;n<5;++n)g.hits[menuActions[n]]=makeRect(211,463+n*47,252,47);}
  }
  return g;
}

}
