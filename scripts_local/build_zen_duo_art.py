"""Encode opaque white silhouettes plus transparent surroundings for native 1-bit rendering."""
from pathlib import Path
from PIL import Image
root=Path(__file__).resolve().parents[1]
out='#pragma once\n#include "CrossZenArt.h"\nnamespace zencompanions {\n'
for name,filename,w in [('Idle','idle-duo-source.png',370),('Header','dictionary-mascots-source.png',112)]:
 im=Image.open(root/'assets_local/crosszen'/filename).convert('RGBA');im=im.resize((w,round(im.height*w/im.width)),Image.Resampling.LANCZOS);h=im.height;stride=(w+7)//8;ink=bytearray(stride*h);opaque=bytearray(stride*h)
 for y in range(h):
  for x in range(w):
   r,g,b,a=im.getpixel((x,y));i=y*stride+x//8;bit=128>>(x%8)
   if a>=128:
    opaque[i]|=bit
    if 299*r+587*g+114*b<150000:ink[i]|=bit
 for suffix,data in [('Ink',ink),('Opaque',opaque)]:out+=f'inline constexpr unsigned char {name}{suffix}[]={{'+','.join(map(str,data))+'};\n'
 out+=f'inline constexpr zenart::Mask {name}{{{w},{h},{name}Ink,{name}Opaque}};\n'
out+='}\n';(root/'src/apps_local/ui/ZenCompanions.h').write_text(out)
