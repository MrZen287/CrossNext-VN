"""Pack the original CrossZen sprite sheet into ink and opaque silhouette masks."""
from pathlib import Path
from PIL import Image, ImageOps, ImageFilter
ROOT=Path(__file__).resolve().parents[1]
src=Image.open(ROOT/'assets_local/crosszen/mascots-source.png').convert('L')
lines=['#pragma once','#include "CompanionArt.h"','namespace zenart {','struct Mask {int width,height;const uint8_t *ink,*opaque;};']
preview=Image.new('RGB',(640,220),'white')
for n,(name,size) in enumerate([('Corner',(150,160)),('Duo',(170,108)),('WormHappy',(72,72)),('WormRest',(72,72))]):
 x,y=n%2,n//2
 tile=src.crop((x*src.width//2,y*src.height//2,(x+1)*src.width//2,(y+1)*src.height//2))
 box=tile.point(lambda p:255 if p<145 else 0).getbbox();assert box
 tile=ImageOps.contain(tile.crop(box),size,Image.Resampling.LANCZOS).point(lambda p:0 if p<145 else 255)
 w,h=tile.size;outside=Image.new('L',(w+2,h+2),255);outside.paste(tile,(1,1));ImageDraw=None
 from PIL import ImageDraw
 ImageDraw.floodfill(outside,(0,0),128,thresh=0)
 opaque=outside.crop((1,1,w+1,h+1)).point(lambda p:0 if p==128 else 255)
 def pack(im,predicate):
  out=bytearray(((w+7)//8)*h)
  for yy in range(h):
   for xx in range(w):
    if predicate(im.getpixel((xx,yy))):out[yy*((w+7)//8)+xx//8]|=0x80>>(xx%8)
  return out
 for suffix,raw in [('Ink',pack(tile,lambda p:p==0)),('Opaque',pack(opaque,lambda p:p!=0))]:
  lines.append('inline constexpr uint8_t '+name+suffix+'[]={'+','.join('0x%02x'%b for b in raw)+'};')
 lines.append(f'inline constexpr Mask {name}{{{w},{h},{name}Ink,{name}Opaque}};')
 preview.paste(tile.convert('RGB'),(n*160,20))
lines.append('''inline void draw(freeink::ui::DrawTarget& t,freeink::ui::Rect area,const Mask& m) {
  using namespace freeink::ui;
  if(area.empty())return;
  const int w=std::min(int(area.width),int(area.height)*m.width/m.height),h=w*m.height/m.width;
  if(w<=0||h<=0)return;
  const int left=area.x+(area.width-w)/2,top=area.bottom()-h,stride=(m.width+7)/8;
  for(int y=0;y<h;++y){
    int start=0,last=-1;
    for(int x=0;x<=w;++x){
      const int pos=(y*m.height/h)*stride+(x<w?x*m.width/w:0)/8;
      const int bit=0x80>>((x<w?x*m.width/w:0)%8);
      const int value=x==w?-1:(m.ink[pos]&bit)?1:(m.opaque[pos]&bit)?0:-1;
      if(value!=last){if(last>=0)t.fill(makeRect(left+start,top+y,x-start,1),Paint::solid(last?freeink::ui::Color::Black:freeink::ui::Color::White));start=x;last=value;}
    }
  }
}
inline void corner(freeink::ui::DrawTarget& t,freeink::ui::Rect r){draw(t,r,Corner);}
inline void duo(freeink::ui::DrawTarget& t,freeink::ui::Rect r){draw(t,r,Duo);}
inline void wormHappy(freeink::ui::DrawTarget& t,freeink::ui::Rect r){draw(t,r,WormHappy);}
inline void wormRest(freeink::ui::DrawTarget& t,freeink::ui::Rect r){draw(t,r,WormRest);}
}''')
(ROOT/'src/apps_local/ui/CrossZenArt.h').write_text('\n'.join(lines)+'\n')
(ROOT/'docs/crosshihi/crosszen-mascots.png').parent.mkdir(parents=True,exist_ok=True)
preview.save(ROOT/'docs/crosshihi/crosszen-mascots.png')
print('CrossZen ink/silhouette masks generated')
