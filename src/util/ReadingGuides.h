#pragma once
#include <algorithm>
namespace readingguides {
inline bool enabled=true;
inline unsigned style=0;
// bottom is exclusive; keep one clear pixel before the next line's ink.
inline int guideY(int bottom,int nextTop,int height){
 const int gap=nextTop-bottom;if(gap<3)return -1;
 return bottom+std::min(std::max(1,height/12),std::max(1,(gap-1)/3));
}
inline bool mark(int style,int offset,int pitch){
 pitch=std::max(style==3?8:4,pitch);const int phase=offset%pitch;
 return style==4||(style==3?(phase<std::max(1,pitch*3/8)||phase==pitch*3/4):style==2?phase<pitch/2:phase==0);
}
}
