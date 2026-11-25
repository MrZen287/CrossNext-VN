from pathlib import Path
import subprocess
r=Path(__file__).resolve().parents[2];out=r/'.crosshihi-test/dragon-preview';out.mkdir(parents=True,exist_ok=True)
s=(r/'src/apps_local/home/DragonHome.h').read_text()
for header in ['<GfxRenderer.h>','<HalClock.h>','"BookCoverPainter.h"','"../../CrossPointSettings.h"','"../../RecentBooksStore.h"','"../reading/ReadingStatsStore.h"']:
 s=s.replace('#include '+header,'')
(out/'DragonPreview.h').write_text(s)
subprocess.run(['c++','-std=c++20','-O1','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-missing-field-initializers','-Isrc/apps_local/home','-Ilib/EpdFont','-Ifreeink-sdk/libs/assets/Icons/include','-Ifreeink-sdk/libs/ui/FreeInkUI/include',str(r/'host-tests/dragon-home/render.cpp'),'src/apps_local/home/ReadingDeskLayout.cpp','src/apps_local/reading/ReadingStatsCore.cpp','src/apps_local/tools/VietnameseLunar.cpp','freeink-sdk/libs/ui/FreeInkUI/src/FreeInkUI.cpp','-o',str(out/'render')],cwd=r,check=True)
subprocess.run([str(out/'render'),str(out)],cwd=r,check=True)
from PIL import Image
for file in out.glob('*.pbm'):Image.open(file).save(file.with_suffix('.png'))
