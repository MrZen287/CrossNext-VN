#include "ReadingDesk.h"
#include <algorithm>
namespace readingdesk {
namespace fui=freeink::ui;
using fui::makeRect;
namespace { constexpr Action menuActions[]={Stats,Calendar,Transfer,Settings}; }
Action Layout::at(int x,int y) const {
  if(!popup.empty()) {
    if(close.contains(x,y)||hits[Menu].contains(x,y))return Menu;
    for(auto a:menuActions)if(hits[a].contains(x,y))return a;
    return Menu; // Outside dismisses without activating the obscured screen.
  }
  if(hits[Menu].contains(x,y))return Menu;
  if(cat.contains(x,y))return Cat;
  for(int a=Recent1;a<=Next;++a)if(hits[a].contains(x,y))return static_cast<Action>(a);
  return reading.contains(x,y)&&!hits[Resume].empty()?Resume:None;
}
Action Layout::move(Action current,int direction) const {
  if(!popup.empty()) {
    for(int n=0;n<4;++n)if(menuActions[n]==current)return menuActions[(n+direction+4)%4];
    return direction<0?Settings:Stats;
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
  g.title=makeRect(copyX,g.cover.y+48,copyW,wide?48:76);
  g.author=makeRect(copyX,g.title.bottom()+4,copyW,wide?36:54);
  g.position=makeRect(copyX,g.author.bottom()+4,copyW,26);
  g.progress=makeRect(copyX,g.position.bottom()+4,copyW,30);
  if(hasBook)g.hits[Resume]=makeRect(copyX,g.cover.bottom()-46,copyW,44);
  const auto button=g.hits[Resume];
  g.duo=makeRect(copyX+4,button.y-78,copyW-8,100);
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
    g.popup=makeRect(g.shelf.right()-pw,std::max(safe.y+8,g.mascot.y-4*row-34),pw,4*row+32);
    g.close=makeRect(g.popup.right()-36,g.popup.y,36,30);
    for(int n=0;n<4;++n)g.hits[menuActions[n]]=makeRect(g.popup.x+6,g.popup.y+30+n*row,pw-12,row);
  }
  return g;
}

}
