// Compile the production dragon drawing body with host substitutes only for board services and cover IO.
#include <FreeInkUIDisplayTarget.h>
#include <vector>
#include <string>
#include <cstdio>
#include <cassert>
#include <cstring>
#include "../../src/apps_local/ui/fonts/RobotoFonts.h"
#include "../../src/apps_local/reading/ReadingStatsCore.h"
struct RecentBook{std::string title,author;};
class GfxRenderer{};
struct {int clockFormat=0;} SETTINGS;
struct Clock{void formatTime(char* out,size_t size,bool){std::snprintf(out,size,"18:51");}}halClock;
struct BookCoverPainter {
 enum class Fit{Stretch};freeink::ui::DrawTarget& t;
 BookCoverPainter(GfxRenderer&,freeink::ui::DrawTarget& t):t(t){}
 void paint(const RecentBook& book,freeink::ui::Rect slot,int,Fit){
  namespace fui=freeink::ui;auto black=fui::Paint::solid(fui::Color::Black);
  t.fill(slot,fui::Paint::solid(fui::Color::White));t.stroke(slot,black,1);
  fui::TextStyle style;style.font=0;style.align=fui::TextAlign::Center;style.maxLines=4;
  auto text=slot.inset({3,10,3,4});t.text(text,book.title.c_str(),style);
  for(int n=0;n<5;++n)t.line({slot.x,int16_t(slot.bottom()-15-n*4)},{int16_t(slot.x+slot.width*(n+2)/7),int16_t(slot.bottom()-28-n*4)},1,black);
 }
};
#include "../../.crosshihi-test/dragon-preview/DragonPreview.h"
namespace fui=freeink::ui;
struct Font {
 std::vector<fui::FontGlyph> glyphs;fui::BitmapFont font;
 Font(const EpdFontData& source,bool digitsOnly=false):glyphs(0x203b-32){
  for(unsigned cp=32;cp<0x203b;++cp){unsigned index=31;for(unsigned i=0;i<source.intervalCount;++i){auto r=source.intervals[i];if((!digitsOnly||cp<128)&&cp>=r.first&&cp<=r.last){index=r.offset+cp-r.first;break;}}
   const auto g=source.glyph[index];assert(g.dataOffset<65536);glyphs[cp-32]={uint16_t(g.dataOffset),g.width,g.height,uint8_t(fp4::toPixel(g.advanceX)),int8_t(g.left),int8_t(-g.top)};
  }
  font={source.bitmap,glyphs.data(),32,0x203a,uint8_t(source.advanceY),uint8_t(source.ascender),100,100,1};
 }
};
int main(int argc,char**argv){assert(argc==2);Font small{sans10},body{sans12},bold{sans13b},title{sans18b},tiny{sans08},huge{sans28b,true};
 for(unsigned theme:{1,2})for(bool empty:{false,true}){
  std::vector<uint8_t> bits(48000,255);fui::DisplayTarget t(bits.data(),480,800,60);GfxRenderer r;
  t.setFont(0,small.font);t.setFont(1,body.font);t.setFont(2,title.font);t.setFont(3,tiny.font);t.setFont(4,small.font);t.setFont(6,bold.font);t.setFont(5,huge.font);
  RecentBook book{"Thiên Thần Quyết","Thái Nhất Sinh Thủy"};std::vector<RecentBook> shelf={{"Thiên Thần Quyết",""},{"Phương pháp tư duy ngược",""},{"Thiết lập con đường thành công",""},{"108 Triết lý về trí tuệ",""}};
  auto g=readingdesk::themedLayout(readingdesk::layout(fui::makeRect(0,0,480,800),empty?0:theme==1?4:3,!empty,true,true,false),theme,empty?0:theme==1?4:3,!empty,true,true);
  if(empty)shelf.clear();
  dragonhome::draw(r,t,g,empty?nullptr:&book,shelf,empty?-1:32,"Chương 7 / 3602","Còn 3595 chương","Chưa đủ dữ liệu","Bổ đồng thì lấy bạc thích trung.",85,readingstats::dayIndex({2026,11,25}),readingdesk::None);
  auto path=std::string(argv[1])+"/theme-"+std::to_string(theme)+(empty?"-empty":"")+".pbm";
  FILE* f=std::fopen(path.c_str(),"wb");assert(f);fprintf(f,"P4\n480 800\n");for(auto v:bits)fputc(v^255,f);fclose(f);
 }
}
