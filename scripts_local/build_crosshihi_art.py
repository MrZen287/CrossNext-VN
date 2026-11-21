"""Convert the original 4x2 sprite sheet to deterministic firmware 1-bit masks."""
from pathlib import Path
from PIL import Image,ImageOps
ROOT=Path(__file__).resolve().parents[1]
source=Image.open(ROOT/'assets_local/crosshihi/mascots-source.png').convert('RGBA')
canvas=Image.new('RGBA',source.size,'white');canvas.alpha_composite(source)
gray=canvas.convert('L')
items=[]
for n in range(7):
 col,row=n%4,n//4
 tile=gray.crop((round(col*gray.width/4),round(row*gray.height/2),round((col+1)*gray.width/4),round((row+1)*gray.height/2)))
 mask=tile.point(lambda p:255 if p<145 else 0)
 box=mask.getbbox();assert box
 tile=tile.crop(box)
 size=(96,96) if n<5 else (72,72) if n==5 else (240,190)
 fitted=ImageOps.contain(tile,size,Image.Resampling.LANCZOS).point(lambda p:0 if p<145 else 255)
 w,h=fitted.size;stride=(w+7)//8;raw=bytearray(stride*h)
 for y in range(h):
  for x in range(w):
   if fitted.getpixel((x,y))==0:raw[y*stride+x//8]|=0x80>>(x%8)
 name='Cat'+str(n) if n<5 else 'Worm' if n==5 else 'Logo'
 items.append((name,w,h,raw,fitted))
lines=['#pragma once','#include <cstdint>','namespace companionart {','struct Sprite { uint16_t width,height; const uint8_t* bits; };']
for name,w,h,raw,_ in items:
 lines.append('inline constexpr uint8_t '+name+'Bits[] = {')
 for i in range(0,len(raw),20):lines.append('  '+','.join('0x%02x'%b for b in raw[i:i+20])+',')
 lines+=['};',f'inline constexpr Sprite {name}{{{w},{h},{name}Bits}};']
lines+=['inline constexpr Sprite Cats[]={Cat0,Cat1,Cat2,Cat3,Cat4};','}']
(ROOT/'src/apps_local/ui/CompanionSprites.h').write_text('\n'.join(lines)+'\n')
preview=Image.new('RGB',(1000,340),'white')
for n,(_,w,h,_,im) in enumerate(items):
 x=20+(n%7)*140;y=30
 shown=ImageOps.contain(im.convert('RGB'),(120,240),Image.Resampling.NEAREST)
 preview.paste(shown,(x,y))
preview.save(ROOT/'docs/crosshihi/firmware-mascots.png')
print('Sprite mask bytes:',sum(len(x[3]) for x in items))
