"""Pack the approved original calendar illustration for the 1-bit e-ink renderer."""
from pathlib import Path
from PIL import Image
root=Path(__file__).resolve().parents[1]
image=Image.open(root/'assets_local/crosszen/calendar-source.png').convert('L').resize((432,172),Image.Resampling.LANCZOS)
raw=bytearray([255]*((432//8)*172))
for y in range(172):
 for x in range(432):
  if image.getpixel((x,y))<170:raw[y*54+x//8]&=~(128>>(x%8))
text='#pragma once\n#include <FreeInkUI.h>\n#include <FreeInkUIIcon.h>\nnamespace calendarart {\ninline constexpr uint8_t bits[]={\n'
text+='\n'.join('  '+','.join(f'0x{b:02x}' for b in raw[i:i+24])+',' for i in range(0,len(raw),24))
text+='\n};\ninline constexpr freeink::Icon icon{432,172,86,bits};\ninline void draw(freeink::ui::DrawTarget& t,freeink::ui::Rect r){t.bitmap(r,freeink::ui::bitmapFromIcon(icon),freeink::ui::BitmapMode::Stretch);}\n}\n'
(root/'src/apps_local/ui/CalendarIllustration.h').write_text(text)
print('Calendar art packed:',len(raw),'bytes')
