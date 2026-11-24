"""Pack the approved 480x800 Home artwork; remove all variable content before embedding."""
from pathlib import Path
from PIL import Image,ImageDraw,ImageChops
root=Path(__file__).resolve().parents[1]
im=Image.open(root/'assets_local/crosszen/home-approved.png').convert('L')
assert im.size==(480,800)
# Darken only the kitten's fur. Keep paper, cover, clock and face highlights intact.
fur_mask=Image.new('L',im.size,0)
ImageDraw.Draw(fur_mask).polygon([(8,211),(15,167),(41,129),(78,110),(81,83),(91,74),(126,74),(160,63),(171,43),(181,37),(194,47),(209,74),(229,104),(240,130),(233,151),(243,164),(239,175),(218,182),(192,180),(187,165),(173,170),(167,181),(155,193),(127,190),(108,179),(102,174),(62,180),(49,191),(56,217),(53,242),(45,251),(56,271),(58,294),(51,311),(37,318),(19,308),(9,291),(9,275),(18,252),(9,236)],fill=255)
# Gentle tonal gain increases stippled fur/stripes without thickening the silhouette.
fur=im.point(lambda value: 255 if value>=245 else round(255*(value/255)**1.45))
im=Image.composite(fur,im,fur_mask)
fur_original=im.copy()
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

# Opaque paw silhouettes are redrawn over the warped cover, without repainting the book face.
original=fur_original.convert('1')
polys=[[(104,170),(112,164),(126,164),(143,164),(158,172),(160,181),(157,188),(151,191),(127,191),(115,187),(107,180)],[(192,164),(198,157),(214,155),(230,156),(237,163),(238,171),(235,176),(225,180),(203,180),(195,176)]]
code='#pragma once\n#include <FreeInkUIIcon.h>\nnamespace homepaws {\n'
for n,poly in enumerate(polys):
 x=min(p[0] for p in poly);y=min(p[1] for p in poly);w=max(p[0] for p in poly)-x+1;h=max(p[1] for p in poly)-y+1
 mask=Image.new('1',(w,h),1);ImageDraw.Draw(mask).polygon([(a-x,b-y) for a,b in poly],fill=0)
 black=Image.new('1',(w,h),1)
 for yy in range(h):
  for xx in range(w):
   if not mask.getpixel((xx,yy)) and not original.getpixel((x+xx,y+yy)):black.putpixel((xx,yy),0)
 for label,im in [('white',mask),('ink',black)]:
  bits=im.tobytes();code+=f'inline constexpr unsigned char {label}{n}[]={{'+','.join(str(v) for v in bits)+'};\n'
  code+=f'inline constexpr freeink::Icon {label}Icon{n}{{{w},{h},0,{label}{n}}};\n'
 code+=f'inline void paw{n}(freeink::ui::DrawTarget& t){{auto b=freeink::ui::makeRect({x},{y},{w},{h});t.bitmap(b,freeink::ui::bitmapFromIcon(whiteIcon{n}),freeink::ui::BitmapMode::Stretch,freeink::ui::Paint::solid(freeink::ui::Color::White));t.bitmap(b,freeink::ui::bitmapFromIcon(inkIcon{n}),freeink::ui::BitmapMode::Stretch);}}\n'
code+='inline void draw(freeink::ui::DrawTarget& t){paw0(t);paw1(t); }\n}\n'
(root/'src/apps_local/home/HomePaws.h').write_text(code)
