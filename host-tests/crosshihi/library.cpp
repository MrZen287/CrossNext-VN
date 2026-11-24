#include <FreeInkUIDisplayTarget.h>
#include <cassert>
#include <iostream>
#include <vector>
#include "../../src/apps_local/library/LibraryMenu.h"
I18n& I18n::getInstance(){static I18n i;return i;}
const char* I18n::get(StrId)const{return "Book";}
int density(bool oldBlackBackground){
 namespace fui=freeink::ui;
 std::vector<uint8_t> pixels(60*800,255);fui::DisplayTarget target(pixels.data(),480,800,60);
 auto device=target.deviceContext();device.hasTouch=true;fui::InputSnapshot input;toybox::Interactions hits;
 toybox::Frame frame(target,device,input,hits,nullptr);toybox::Screen screen(frame);pixelui::begin(screen,0);
 fui::ListItem rows[10]{};for(auto& r:rows){r.label="A book on the card";r.value="epub";}
 fui::ListProps props;libraryui::styleBookRows(props,target);libraryui::lightRows(props);
 props.items=rows;props.count=10;props.selectedIndex=-1;
 if(oldBlackBackground)props.rowStyles.normal.background=fui::Paint::solid(fui::Color::Black);
 const auto body=screen.body();pixelui::list(screen,props);
 int black=0,total=0;
 for(int y=body.y;y<body.bottom();++y)for(int x=body.x;x<body.right();++x){++total;black+=!(pixels[y*60+x/8]&(0x80>>(x%8)));}
 assert(black>100);return black*100/total;
}
int main(){chrome::lightHeader=true;const int old=density(true),fixed=density(false);assert(old>70);assert(fixed<30);std::cout<<"Library render: old black="<<old<<"%, fixed black="<<fixed<<"%; book text remains visible\n";}
