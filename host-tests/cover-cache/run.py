from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[2];out=root/'.crosshihi-test/cover-cache';out.mkdir(parents=True,exist_ok=True)
s=(root/'src/apps_local/home/BookCoverPainter.cpp').read_text()
# Substitute only board/application dependencies; compile the production painter methods verbatim.
for header in ['../../CrossPointSettings.h','../../RecentBooksStore.h','../../components/UITheme.h']:
 needle='#include "'+header+'"';assert s.count(needle)==1;s=s.replace(needle,'')
(out/'BookCoverPainter.cpp').write_text(s)
subprocess.run(['c++','-std=c++20','-O1','-Ihost-tests/cover-cache/stubs','-Ihost-tests/cover-cache','-Isrc/apps_local/home','-Ifreeink-sdk/libs/ui/FreeInkUI/include','-include','host-tests/cover-cache/Environment.h',str(out/'BookCoverPainter.cpp'),'src/apps_local/home/ReadingDeskLayout.cpp','host-tests/cover-cache/test.cpp','freeink-sdk/libs/ui/FreeInkUI/src/FreeInkUI.cpp','-o',str(out/'test')],cwd=root,check=True)
subprocess.run([str(out/'test')],check=True)
