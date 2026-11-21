#include "ReadingDesk.h"
#include <algorithm>
namespace readingdesk {
namespace fui=freeink::ui;
using fui::makeRect;
namespace { constexpr Action menuActions[]={Library,Stats,Calendar,Apps,Transfer,Settings}; }
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
    for(int n=0;n<6;++n)if(menuActions[n]==current)return menuActions[(n+direction+6)%6];
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
  g.status=makeRect(x,safe.y+3,w,38);
  g.heading=makeRect(x,g.status.bottom()+4,w,48);
  const int shelfH=wide?134:182;
  g.shelf=makeRect(x,safe.bottom()-shelfH-10,w,shelfH);
  const int quoteH=wide?58:66;
  g.quote=makeRect(x,g.shelf.y-quoteH-8,w,quoteH);
  g.reading=makeRect(x,g.heading.bottom()+44,w,g.quote.y-g.heading.bottom()-50);
  const int coverH=std::min(int(g.reading.height)-10,wide?170:340);
  const int coverW=std::min(w*45/100,coverH*2/3);
  g.cover=makeRect(x,g.reading.y,coverW,coverH);
  const int copyX=g.cover.right()+18,copyW=x+w-copyX;
  g.title=makeRect(copyX,g.cover.y+10,copyW,wide?55:86);
  g.author=makeRect(copyX,g.title.bottom()+4,copyW,wide?40:56);
  g.position=makeRect(copyX,g.author.bottom()+10,copyW,32);
  g.progress=makeRect(copyX,g.position.bottom()+10,copyW,32);
  if(hasBook)g.hits[Resume]=makeRect(copyX,std::max(g.progress.bottom()+8,g.cover.bottom()-48),copyW,46);
  g.cat=makeRect(g.cover.right()-68,g.cover.y-78,84,84);
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
    g.popup=makeRect(g.shelf.right()-pw,std::max(g.status.bottom()+4,g.mascot.y-6*row-34),pw,6*row+32);
    g.close=makeRect(g.popup.right()-36,g.popup.y,36,30);
    for(int n=0;n<6;++n)g.hits[menuActions[n]]=makeRect(g.popup.x+6,g.popup.y+30+n*row,pw-12,row);
  }
  return g;
}

}
