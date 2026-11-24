from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[1]
s=(root/'src/apps_local/Shelf.cpp').read_text()
expected=json.loads((root/'host-tests/crosshihi/catalog.json').read_text())
for group,titles in expected.items():
 body=s.split('constexpr shelf::Item '+group+'[] = {',1)[1].split('\n};',1)[0]
 assert re.findall(r'\{"([^"]+)"',body)==titles, f'App registry differs from approved catalog: {group}'
print('Approved app/game stable identifiers verified:',sum(map(len,expected.values())))
# The only exposed built-in UI language is Vietnamese.
ini=(root/'platformio.ini').read_text()
assert 'custom_i18n_builtin_langs = vi' in ini
main=(root/'src/main.cpp').read_text()
assert 'I18N.setLanguage(Language::VI)' in main
quotes=json.loads((root/'docs/crosshihi/quote-sources.json').read_text())
assert len(quotes)==400 and len({x['text'] for x in quotes})==400
assert all(len(x['text'].split())>=7 for x in quotes)
assert 'static constexpr const char* english[]' not in (root/'src/apps_local/ui/VietnameseQuotes.cpp').read_text()
assert 'CHECKERS' not in expected['kGames']
print('Vietnamese-only UI; 400 unique quotes of at least seven words; Checkers removed')

assert len(re.findall(r"inline constexpr Sprite Cat[0-4]\{",(root/'src/apps_local/ui/CompanionSprites.h').read_text()))==5
print('Five distinct kitten pose masks and shared worm/logo art are embedded')

for path in ['lib/BlePageTurner/src/SdkRadio.cpp','lib/KOReaderSync/KOReaderSyncClient.cpp','lib/hal/HalSystem.cpp']:
 assert 'CrossNext' not in (root/path).read_text(), 'Legacy visible brand: '+path
print('Bluetooth, sync device name and system info use CrossZen')

assert "CrossHihi-X4PRO-OTA.bin" in (root/"src/network/FirmwareBoardTag.h").read_text()
assert "pinItemToFront" not in (root/"src/apps_local/ShelfFolderActivity.cpp").read_text()
assert "drawSingle" in (root/"src/apps_local/home/ReadingDesk.cpp").read_text()
print("CrossZen retains the legacy OTA alias; no icon long-press pinning")

pet=(root/"src/apps_local/bookpet/BookPetActivity.cpp").read_text()
assert 'PetSprite.h' not in pet and 'Mèo & Mọt' in pet
assert 'companionart::cat' in pet and 'zenart::worm' in pet
assert 'Fit::Stretch' in (root/"src/apps_local/home/ReadingDesk.cpp").read_text()
print("Cat/Worm art and fixed cover shell replace pixel species and variable cover bounds")
