"""Pack the approved 480x800 Home artwork; remove all variable content before embedding."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[1]
im=Image.open(root/'assets_local/crosszen/home-approved.png').convert('L')
assert im.size==(480,800)
d=ImageDraw.Draw(im)
# Clock, live reading information, quote, battery, and replaceable cover fronts.
for box in [(286,35,451,123),(293,198,476,410),(319,488,451,530),(16,578,464,609),(390,645,450,675),
            (63,194,275,540),(24,644,109,760),(132,643,219,760),(239,642,327,760)]:d.rectangle(box,fill=255)
# Error diffusion preserves the approved illustration's fine shadows on a 1-bit panel.
bw=im.convert('1');raw=bw.tobytes();assert len(raw)==48000
p=root/'src/apps_local/home/ApprovedHomeArt.h'
s='#pragma once\n#include <FreeInkUI.h>\nnamespace approvedhome {\ninline constexpr unsigned char bits[]={\n'
s+='\n'.join(','.join(str(v) for v in raw[i:i+32])+',' for i in range(0,len(raw),32))
s+='\n};\ninline void draw(freeink::ui::DrawTarget& t,freeink::ui::Rect box){freeink::ui::BitmapRef b{bits,480,800,freeink::ui::BitmapFormat::Mask1};t.fill(box,freeink::ui::Paint::solid(freeink::ui::Color::White));t.bitmap(box,b,freeink::ui::BitmapMode::Stretch);}\n}\n'
p.write_text(s);bw.save(root/'assets_local/crosszen/home-template.png');print(len(raw))
