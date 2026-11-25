"""Convert approved dragon templates into panel-sized 1-bit flash assets and foreground masks."""
from pathlib import Path
from PIL import Image, ImageDraw
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'src/apps_local/home/DragonHomeArt.h'
code='#pragma once\n#include <FreeInkUI.h>\nnamespace dragonart {\n'
def packed(name,im):
 global code
 raw=im.tobytes()
 code+=f'inline constexpr unsigned char {name}[]={{\n'
 code+='\n'.join(','.join(map(str,raw[i:i+32]))+',' for i in range(0,len(raw),32))+'\n};\n'
def sprite(name,im,polys):
 global code
 mask=Image.new('1',im.size,1);d=ImageDraw.Draw(mask)
 for p in polys:d.polygon(p,fill=0)
 points=[point for poly in polys for point in poly]
 bbox=(min(x for x,y in points),min(y for x,y in points),max(x for x,y in points)+1,max(y for x,y in points)+1)
 x,y,right,bottom=bbox;mask=mask.crop(bbox);source=im.crop(bbox)
 ink=Image.new('1',mask.size,1)
 for yy in range(mask.height):
  for xx in range(mask.width):
   if not mask.getpixel((xx,yy)) and not source.getpixel((xx,yy)):ink.putpixel((xx,yy),0)
 packed(name+'White',mask);packed(name+'Ink',ink)
 code+=f'''inline void {name}(freeink::ui::DrawTarget& t){{
 auto b=freeink::ui::makeRect({x},{y},{right-x},{bottom-y});
 t.bitmap(b,{{{name}White,{right-x},{bottom-y},freeink::ui::BitmapFormat::Mask1}},freeink::ui::BitmapMode::Stretch,freeink::ui::Paint::solid(freeink::ui::Color::White));
 t.bitmap(b,{{{name}Ink,{right-x},{bottom-y},freeink::ui::BitmapFormat::Mask1}},freeink::ui::BitmapMode::Stretch);
}}
'''
for name,file,polys in [
 ('cloud','background-source.png', [[(192,211),(203,214),(213,221),(211,233),(202,237),(191,235)],[(285,208),(299,209),(305,222),(298,231),(283,231)],[(178,271),(190,273),(199,280),(202,302),(190,313),(179,307)],[(286,274),(297,279),(302,289),(299,301),(287,312),(281,300)]]),
 ('coiled','coiled-source.png', [[(62,255),(80,257),(94,267),(101,279),(96,287),(92,285),(98,302),(88,297),(81,285),(73,288),(62,285)],[(260,164),(284,162),(302,175),(310,190),(299,204),(287,218),(282,216),(285,197),(277,206),(271,205),(274,188),(262,195)],[(286,317),(302,333),(308,362),(302,392),(287,420),(280,441),(263,463),(246,481),(220,497),(200,500),(176,493),(158,479),(155,465),(163,448),(177,435),(207,422),(238,409),(261,393),(275,370),(280,347)]])]:
 source=ROOT/'assets_local/crosszen/dragon'/file
 if not source.exists():source=source.with_name(f'{name}-native.png')
 im=Image.open(source).convert('L').resize((480,800),Image.Resampling.LANCZOS).convert('1')
 im.save(ROOT/'assets_local/crosszen/dragon'/f'{name}-native.png')
 packed(name,im);sprite(name+'Foreground',im,polys)
code+='''inline void background(freeink::ui::DrawTarget& t,unsigned theme){
 auto box=freeink::ui::makeRect(0,0,480,800);
 t.fill(box,freeink::ui::Paint::solid(freeink::ui::Color::White));
 t.bitmap(box,{theme==1?cloud:coiled,480,800,freeink::ui::BitmapFormat::Mask1},freeink::ui::BitmapMode::Stretch);
}
inline void foreground(freeink::ui::DrawTarget& t,unsigned theme){if(theme==1)cloudForeground(t);else coiledForeground(t);}
}
'''
OUT.write_text(code)
print(OUT)
