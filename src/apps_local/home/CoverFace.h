#pragma once
#include <FreeInkUI.h>
#include <algorithm>
namespace homecover {
// Four corners of an upright cover plane, in logical screen pixels.
struct CoverFace {
 int left=0,right=0,topLeft=0,topRight=0,bottomLeft=0,bottomRight=0;
 bool valid()const{return right>left&&bottomLeft>topLeft&&bottomRight>topRight;}
 freeink::ui::Rect bounds()const{return freeink::ui::makeRect(left,std::min(topLeft,topRight),right-left+1,std::max(bottomLeft,bottomRight)-std::min(topLeft,topRight)+1);}
 int top(int x)const{return topLeft+(topRight-topLeft)*(x-left)/std::max(1,right-left);}
 int bottom(int x)const{return bottomLeft+(bottomRight-bottomLeft)*(x-left)/std::max(1,right-left);}
 bool contains(int x,int y)const{return valid()&&x>=left&&x<=right&&y>=top(x)&&y<=bottom(x);}
 template<class Fill>void mapRect(freeink::ui::Rect from,int x,int y,int w,int h,Fill fill)const{
  if(!valid()||from.empty()||w<=0||h<=0)return;
  const int first=std::max(left,x),last=std::min(right,x+w-1);
  for(int dx=first;dx<=last;++dx){
   const int t=top(dx),height=bottom(dx)-t+1;
   int a=t+(y-from.y)*height/from.height,b=t+(y+h-from.y)*height/from.height;
   a=std::clamp(a,t,bottom(dx)+1);b=std::clamp(b,t,bottom(dx)+1);
   if(b>a)fill(dx,a,1,b-a);
  }
 }
};
inline constexpr CoverFace mainFace{65,273,197,182,546,536};
inline constexpr CoverFace shelfFaces[]={{26,107,647,647,764,764},{134,217,646,646,764,764},{241,325,645,645,764,764}};
}
