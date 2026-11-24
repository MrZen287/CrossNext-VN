#pragma once
#include <algorithm>
namespace readingguides {
inline bool enabled=true;
inline unsigned style=0;
inline bool mark(int style,int offset,int pitch){
 pitch=std::max(4,pitch);const int phase=offset%pitch;
 return style==4||(style==3?(phase<pitch/2||phase==pitch-1):style==2?phase<pitch/2:phase==0);
}
}
