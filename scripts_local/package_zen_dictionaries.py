"""Build SD streaming dictionaries. External licensed data never enters firmware flash."""
from pathlib import Path
import argparse,sqlite3,struct,gzip,unicodedata,re,html,zipfile,hashlib,json
from collections import defaultdict
p=argparse.ArgumentParser();p.add_argument('--database',required=True);p.add_argument('--vnedict',required=True);p.add_argument('--output',required=True);p.add_argument('--viwiktionary');a=p.parse_args();out=Path(a.output);root=out/'dictionaries/CrossZen';root.mkdir(parents=True,exist_ok=True)
def plain(s):
 s=re.sub(r'<(?:br\s*/?|/?(?:li|ul|p|div|h[1-6]))\b[^>]*>','\n',s,flags=re.I);s=re.sub(r'<[^>]*>','',s);return '\n'.join(x.strip() for x in html.unescape(s).splitlines() if x.strip())
def key(s):return unicodedata.normalize('NFC',s.strip().lower())
def pack(name,entries):
 entries={key(k):v for k,v in entries.items() if key(k) and len(key(k).encode())<=256};data=bytearray();idx=bytearray(b'ZDIC0001'+struct.pack('<I',len(entries)))
 for word in sorted(entries,key=lambda s:s.encode('utf8')):
  w=word.encode();text=entries[word].encode();pos=len(data);data+=w;offset=len(data);data+=text;idx+=struct.pack('<IIII',pos,len(w),offset,len(text))
 (root/(name+'.zidx')).write_bytes(idx);(root/(name+'.zdat')).write_bytes(data);return {'entries':len(entries),'index_bytes':len(idx),'data_bytes':len(data)}
c=sqlite3.connect(a.database);en=defaultdict(list)
for word,meaning in c.execute("select w.word,m.meaning_text from words w join meanings m on m.word_id=w.id join sources s on s.id=m.source_id where s.name='FVDP (GPL)' order by w.id,m.id"):
 en[key(word)].append(plain(meaning))
en={w:'\n\n'.join(dict.fromkeys(v)) for w,v in en.items()}
base=Path(a.vnedict);raw=(base/'vnedict.idx').read_bytes();defs=gzip.decompress((base/'vnedict.dict.dz').read_bytes());vi=defaultdict(list);pos=0
while pos<len(raw):
 end=raw.index(0,pos);word=raw[pos:end].decode('utf8');offset,size=struct.unpack('>II',raw[end+1:end+9]);pos=end+9;vi[key(word)].append(plain(defs[offset:offset+size].decode('utf8')))
vi={w:'\n\n'.join(dict.fromkeys(v)) for w,v in vi.items()}
manifest={'format':'ZDIC0001','input_sha256':{Path(a.database).name:hashlib.sha256(Path(a.database).read_bytes()).hexdigest(),'vnedict.idx':hashlib.sha256((base/'vnedict.idx').read_bytes()).hexdigest(),'vnedict.dict.dz':hashlib.sha256((base/'vnedict.dict.dz').read_bytes()).hexdigest()},'anh-viet':pack('anh-viet',en),'viet-anh':pack('viet-anh',vi),'sources':{'anh-viet':'https://github.com/tecxmate/vnme-data/releases/tag/v1.0.0 (FVDP GPL records only; AI source excluded)','viet-anh':'https://github.com/cer28/stardict-vnedict (Paul Denisowski VNEDICT, CC BY 3.0)'},'changes':'Converted HTML to plain text, normalized NFC/lowercase headwords, grouped duplicate definitions; no reverse-derived entries.'}
if a.viwiktionary:
 vv=defaultdict(list)
 with gzip.open(a.viwiktionary,'rt',encoding='utf-8') as source:
  for line in source:
   item=json.loads(line)
   if item.get('lang_code')!='vi' or not item.get('word'):continue
   for sense in item.get('senses',[]):
    if 'no-gloss' in sense.get('tags',[]):continue
    vv[key(item['word'])].extend(g.strip() for g in sense.get('glosses',[]) if g.strip())
 entries={w:'\n'.join(dict.fromkeys(glosses)) for w,glosses in vv.items() if glosses}
 if len(entries)<1000:raise ValueError('Vietnamese source extraction returned fewer than 1,000 definitions')
 manifest['viet-viet']=pack('viet-viet',entries)
 manifest['input_sha256']['viwiktionary.jsonl.gz']=hashlib.sha256(Path(a.viwiktionary).read_bytes()).hexdigest()
 manifest['sources']['viet-viet']='Vietnamese Wiktionary contributors, extracted with Wiktextract by Kaikki.org. CC BY-SA 4.0 and GFDL. https://kaikki.org/viwiktionary/rawdata.html'
(out/'manifest-dictionaries.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2))
(root/'ATTRIBUTION.txt').write_text('Anh–Việt: Hồ Ngọc Đức / Free Vietnamese Dictionary Project. Mirror tecxmate/vnme-data; only FVDP (GPL) records, excluding AI_Generated_EN. GPL-3.0.\nViệt–Anh: VNEDICT by Paul Denisowski. CC BY 3.0. http://www.denisowski.org/Vietnamese/vnedict_readme.htm\nConversion by CrossZen: NFC/lowercase, plain text, streaming index. See manifest and bundled source links for reproducibility.\n')
if a.viwiktionary:
 with (root/'ATTRIBUTION.txt').open('a',encoding='utf-8') as attribution:
  attribution.write('Việt–Việt: contributors to Wiktionary tiếng Việt; extract by Kaikki.org (Wiktextract). From https://kaikki.org/viwiktionary/rawdata.html. Converted to normalized plain-text indexed definitions by CrossZen. Licensed CC BY-SA 4.0 https://creativecommons.org/licenses/by-sa/4.0/ and GFDL https://www.gnu.org/licenses/fdl-1.3.html. Source entries: https://vi.wiktionary.org/wiki/<mục từ>.\n')
(root/'LICENSE-FVDP-GPL3.txt').write_bytes((base/'LICENSE-FVDP-GPL3.txt').read_bytes());(root/'VNEDICT-README.md').write_bytes((base/'VNEDICT-README.md').read_bytes())
(out/'HUONG-DAN.txt').write_text('Giải nén để thư mục dictionaries nằm ngay ở gốc thẻ nhớ. Giữ nguyên các thư mục khác. Cần CrossZen 1.1.1.9 trở lên. APP > Từ Điển hoặc vuốt phải từ Home mèo. Anh → Việt: bàn phím EN. Việt → Anh: Telex; z bỏ dấu, Xóa sửa ký tự, chạm ô tìm kiếm để quay lại. Chạm gợi ý để mở nghĩa; vuốt ngang để chuyển trang. OTA không ghi dữ liệu thẻ nhớ.\n')
with zipfile.ZipFile(out/'CrossZen-Tu-Dien-Anh-Viet.zip','w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
 for f in sorted(out.rglob('*')):
  if f.is_file() and f.suffix!='.zip':z.write(f,f.relative_to(out))
print(json.dumps(manifest,ensure_ascii=False,indent=2))
