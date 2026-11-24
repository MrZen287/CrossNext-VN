#include "apps_local/dictionary/Telex.h"
#include "apps_local/home/CoverFace.h"
#include "apps_local/home/CatExpressions.h"
#include "util/ReadingGuides.h"
#include <cassert>
#include <cstdio>
int main(){
 const char*cases[][2]={{"hocj","học"},{"tieengs vieetj","tiếng việt"},{"dduwowngf","đường"},{"truowngf","trường"},{"quas","quá"},{"gias","giá"},{"hoaf","hòa"},{"dduowcj","được"},{"sachs","sách"},{"meof","mèo"},{"sawcs","sắc"},{"tooi","tôi"},{"vieetj nam","việt nam"},{"asaz","â"}};
 for(auto& c:cases){auto got=zentelex::convert(c[0]);if(got!=c[1]){printf("Telex %s: got %s expected %s\n",c[0],got.c_str(),c[1]);return 1;}}
 for(int height:{12,18,24,32,48})for(int gap=0;gap<30;++gap){int y=readingguides::guideY(100,100+gap,height);assert(gap<3?y==-1:y>100&&y<100+gap);}
 for(int gap=4;gap<=20;++gap){assert(readingguides::mark(4,0,gap));assert(!readingguides::mark(3,std::max(8,gap)-1,gap));}
 auto face=homecover::mainFace;auto b=face.bounds();int pixels=0;face.mapRect(b,b.x,b.y,b.width,b.height,[&](int x,int y,int w,int h){for(int xx=x;xx<x+w;++xx)for(int yy=y;yy<y+h;++yy){assert(face.contains(xx,yy));++pixels;}});assert(pixels>60000);assert(face.top(face.left)==face.topLeft&&face.top(face.right)==face.topRight);
 unsigned pose=0;for(unsigned n=0;n<24;++n){unsigned next=catexpressions::next(pose);assert(next!=pose&&next<6);pose=next;}
 puts("Telex, guide spacing, cover-plane clipping and six face states passed");
}
