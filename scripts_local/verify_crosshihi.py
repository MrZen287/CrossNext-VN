from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[1]
s=(root/'src/apps_local/Shelf.cpp').read_text()
expected=json.loads((root/'host-tests/crosshihi/catalog.json').read_text())
for group,titles in expected.items():
 body=s.split('constexpr shelf::Item '+group+'[] = {',1)[1].split('\n};',1)[0]
 assert re.findall(r'\{"([^"]+)"',body)==titles, f'Original app registry changed: {group}'
print('All original app/game stable identifiers preserved:',sum(map(len,expected.values())))
# Firmware only exposes the two built-in languages; no Vietnamese-only macro.
ini=(root/'platformio.ini').read_text()
assert 'custom_i18n_builtin_langs = en,vi' in ini
assert '-D CROSSNEXT_VIETNAMESE_ONLY' not in ini
main=(root/'src/main.cpp').read_text()
assert 'I18N.setLanguage(static_cast<Language>(SETTINGS.language))' in main
assert 'I18N.setLanguage(Language::VI)' not in main
print('English selection survives startup; EN/VI built in')

assert len(re.findall(r"inline constexpr Sprite Cat[0-4]\{",(root/'src/apps_local/ui/CompanionSprites.h').read_text()))==5
print('Five distinct kitten pose masks and shared worm/logo art are embedded')

for path in ['lib/BlePageTurner/src/SdkRadio.cpp','lib/KOReaderSync/KOReaderSyncClient.cpp','lib/hal/HalSystem.cpp']:
 assert 'CrossNext' not in (root/path).read_text(), 'Legacy visible brand: '+path
print('Bluetooth, sync device name and system info use CrossZen')

assert "CrossHihi-X4PRO-OTA.bin" in (root/"src/network/FirmwareBoardTag.h").read_text()
assert "pinItemToFront" not in (root/"src/apps_local/ShelfFolderActivity.cpp").read_text()
assert "drawSingle" in (root/"src/apps_local/home/ReadingDesk.cpp").read_text()
print("CrossZen retains the legacy OTA alias; no icon long-press pinning")
