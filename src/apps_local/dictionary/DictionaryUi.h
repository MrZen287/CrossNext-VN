#pragma once
#include <FreeInkUI.h>
#include "../ui/MenuCompanions.h"
#include <string>
namespace zendictionary {
namespace ui=freeink::ui;
struct Ui {
 ui::DrawTarget&t;
 static constexpr auto ink=ui::Paint::solid(ui::Color::Black),white=ui::Paint::solid(ui::Color::White);
 void txt(int x,int y,int w,int h,const char*s,int font=1,bool center=false,bool inverse=false){ui::TextStyle a;a.font=font;a.align=center?ui::TextAlign::Center:ui::TextAlign::Left;a.color=inverse?ui::Color::White:ui::Color::Black;a.maxLines=1;t.text(ui::makeRect(x,y,w,h),s,a);}
 void line(int x,int y,int xx,int yy,int width=1){t.line({int16_t(x),int16_t(y)},{int16_t(xx),int16_t(yy)},width,ink);}
 void box(int x,int y,int w,int h,bool filled=false,int rad=8){auto r=ui::makeRect(x,y,w,h);if(filled)t.fill(r,ink,rad);else t.stroke(r,ink,1,rad);}
 void key(int x,int y,int w,const char*s,bool fill=false){box(x,y,w,48,fill,7);txt(x,y+11,w,31,s,1,true,fill);}
 void header(bool vi,const char*query){txt(20,22,310,40,"Từ Điển",2);line(20,65,460,65);menucompanions::draw(t,344,65);
 for(int n=0;n<2;++n){int x=n?253:20;bool selected=n?vi:!vi;box(x,78,207,38,selected);txt(x+16,86,66,27,n?"Việt":"Anh",1,true,selected);txt(x+127,86,66,27,n?"Anh":"Việt",1,true,selected);auto p=selected?white:ink;t.line({int16_t(x+88),97},{int16_t(x+116),97},1,p);t.line({int16_t(x+111),92},{int16_t(x+116),97},1,p);t.line({int16_t(x+111),102},{int16_t(x+116),97},1,p);}
 box(20,130,440,56);t.stroke(ui::makeRect(34,147,16,16),ink,2,8);line(48,161,55,169,2);txt(68,145,344,32,*query?query:"Nhập từ cần tra",1);line(433,151,443,161);line(443,151,433,161);
 }
 void keyboard(bool telex,int previewKey){line(20,479,460,479);txt(20,489,218,24,telex?"VI · Telex":"EN · Tiếng Anh",0);txt(288,489,172,24,"Ẩn bàn phím",0,true);const char*rows[]={"qwertyuiop","asdfghjkl","zxcvbnm"};for(int row=0;row<3;++row){int n=std::char_traits<char>::length(rows[row]);int start=row==0?15:row==1?37:82;for(int i=0;i<n;++i){char c[]={rows[row][i],0};key(start+i*45,523+row*55,40,c);}}key(15,633,58,"Xóa hết");key(401,633,64,"Xóa");key(15,688,72,telex?"VI":"EN");key(95,688,214,"Khoảng trắng");key(317,688,148,"Tra từ",true);
 for(int row=0;row<3;++row){
   int start=row==0?15:row==1?37:82;
   for(int i=0;rows[row][i];++i){
     if(rows[row][i]!=previewKey) continue;
     const int x=start+i*45,y=523+row*55;
     char label[]={char(previewKey),0};
     key(x,y,40,label,true);
     const int left=x<11?0:x+51>480?418:x-11;
     box(left,y-54,62,50,true);
     t.fill(ui::makeRect(left+2,y-52,58,46),white,7);
     txt(left,y-50,62,40,label,2,true);
     line(x+14,y-5,x+20,y,2);
     line(x+20,y,x+26,y-5,2);
   }
 }
 }
 void footer(const char*clock,const char*right){line(0,753,480,753);line(22,775,30,767,2);line(22,775,30,783,2);line(22,775,42,775,2);txt(53,765,140,27,"Quay lại",0);txt(202,765,100,27,clock,0,true);txt(325,765,135,27,right,0,true);}
};
// A single geometry contract for the touch keyboard and its labels.
inline int keyAt(int x,int y){const char*rows[]={"qwertyuiop","asdfghjkl","zxcvbnm"};for(int row=0;row<3;++row){int start=row==0?15:row==1?37:82;for(int n=0;rows[row][n];++n)if(ui::makeRect(start+n*45,523+row*55,40,48).contains(x,y))return rows[row][n];}if(ui::makeRect(401,633,64,48).contains(x,y))return 8;if(ui::makeRect(15,633,58,48).contains(x,y))return 127;if(ui::makeRect(15,688,72,48).contains(x,y))return 256;if(ui::makeRect(95,688,214,48).contains(x,y))return ' ';if(ui::makeRect(317,688,148,48).contains(x,y))return 13;return 0;}
}
