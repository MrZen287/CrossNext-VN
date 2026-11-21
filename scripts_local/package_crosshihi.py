"""Verify and package the X4 Pro app with the unchanged licensed SD resources."""
from pathlib import Path, PurePosixPath
import argparse, hashlib, json, shutil, struct, zipfile
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--resources',required=True);p.add_argument('--output',default='dist/CrossHihi');args=p.parse_args()
version='1.1.1.1'
firmware=ROOT/'.pio/build/gh_release_x4pro/firmware.bin'
data=firmware.read_bytes()
assert data[0]==0xe9 and struct.unpack_from('<H',data,12)[0]==9, 'Not an ESP32-S3 application'
assert len(data)<=0x7f0000, 'Firmware exceeds X4 Pro OTA slot'
assert b'CROSSPOINT-BOARD-V1:x4pro;' in data, 'Missing board identity'
# Verify the ESP application segment checksum and optional SHA-256 footer.
segments=data[1];offset=24;checksum=0xef
for _ in range(segments):
 _,size=struct.unpack_from('<II',data,offset);offset+=8
 assert offset+size<=len(data),'Truncated ESP segment'
 for byte in data[offset:offset+size]:checksum^=byte
 offset+=size
checksum_at=(offset//16)*16+15
assert data[checksum_at]==checksum,'Bad ESP image XOR checksum'
if data[23]:
 assert data[checksum_at+1:checksum_at+33]==hashlib.sha256(data[:checksum_at+1]).digest(),'Bad ESP application SHA256'
out=Path(args.output);out.mkdir(parents=True,exist_ok=True)
name=f'CrossHihi-X4Pro-v{version}.bin'
(out/name).write_bytes(data);(out/'CrossHihi-X4PRO-OTA.bin').write_bytes(data)
archive=out/f'CrossHihi-X4Pro-v{version}.zip'
with zipfile.ZipFile(args.resources) as old,zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
 for item in old.infolist():
  path=PurePosixPath(item.filename)
  assert not path.is_absolute() and '..' not in path.parts
  if not path.parts or item.is_dir():continue
  if len(path.parts)==1 and path.suffix=='.bin':continue
  z.writestr(item.filename,old.read(item.filename))
 z.writestr(name,data)
 z.writestr('HUONG-DAN-CrossHihi.txt',(ROOT/'docs/crosshihi/release-1.1.1.1.md').read_text())
 for file in ['LICENSE','THIRD-PARTY-NOTICES.md']:
  z.writestr(file,(ROOT/file).read_bytes())
rows=[]
for f in sorted(out.iterdir()):
 if f.suffix in ('.bin','.zip'):rows.append({'file':f.name,'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
(out/'SHA256SUMS.txt').write_text(''.join(f"{x['sha256']}  {x['file']}\n" for x in rows))
(out/'manifest.json').write_text(json.dumps({'version':version,'board':'X4PRO','files':rows},indent=2)+'\n')
print(json.dumps({'version':version,'verified_esp_image':True,'files':rows},indent=2))
