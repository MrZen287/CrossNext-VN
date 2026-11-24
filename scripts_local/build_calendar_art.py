"""Pack the approved original calendar illustration for the 1-bit e-ink renderer."""
from pathlib import Path
from PIL import Image, ImageDraw
import math
root=Path(__file__).resolve().parents[1]
image=Image.open(root/'assets_local/crosszen/calendar-source.png').convert('L').resize((432,172),Image.Resampling.LANCZOS)
# Native-size linework stays legible after the 1-bit conversion.
d=ImageDraw.Draw(image)
# Sun in the upper-right corner, clear of the large date numerals.
cx,cy=400,27
d.ellipse((cx-11,cy-11,cx+11,cy+11),outline=0,width=2)
for n in range(12):
 a=n*math.pi/6
 d.line((round(cx+16*math.cos(a)),round(cy+16*math.sin(a)),round(cx+21*math.cos(a)),round(cy+21*math.sin(a))),fill=0,width=2)
# Roof tiles, door panels, window sill and short garden/field textures.
for x in range(352,389,7):d.line((x,132,x+5,126),fill=0,width=1)
d.line((352,159,365,159),fill=0,width=2)
d.line((376,153,376,161),fill=0,width=1)
d.point((379,157),fill=0)
for x,y in [(332,165),(340,164),(410,166),(417,165),(40,154),(65,158),(85,163)]:
 d.line((x,y,x+3,y-3),fill=0,width=1)
 d.line((x+3,y-3,x+5,y),fill=0,width=1)
# Small leaf veins and tree-canopy texture, restricted to the illustration margins.
for x,y in [(24,9),(39,17),(62,26),(46,38),(55,48)]:d.line((x,y,x+4,y+3),fill=0,width=1)
for x,y in [(389,105),(399,101),(412,108),(404,114)]:d.arc((x-3,y-2,x+3,y+2),190,340,fill=0,width=1)
raw=bytearray([255]*((432//8)*172))
for y in range(172):
 for x in range(432):
  if image.getpixel((x,y))<170:raw[y*54+x//8]&=~(128>>(x%8))
text='#pragma once\n#include <FreeInkUI.h>\n#include <FreeInkUIIcon.h>\nnamespace calendarart {\ninline constexpr uint8_t bits[]={\n'
text+='\n'.join('  '+','.join(f'0x{b:02x}' for b in raw[i:i+24])+',' for i in range(0,len(raw),24))
text+='\n};\ninline constexpr freeink::Icon icon{432,172,86,bits};\ninline void draw(freeink::ui::DrawTarget& t,freeink::ui::Rect r){t.bitmap(r,freeink::ui::bitmapFromIcon(icon),freeink::ui::BitmapMode::Stretch);}\n}\n'
(root/'src/apps_local/ui/CalendarIllustration.h').write_text(text)
print('Calendar art packed:',len(raw),'bytes')
