#pragma once
#include <string>
#include <vector>
#include <cstdint>
namespace zentelex {
inline std::string utf8(uint32_t cp){std::string s;if(cp<128)s+=char(cp);else if(cp<2048){s+=char(0xc0|(cp>>6));s+=char(0x80|(cp&63));}else{s+=char(0xe0|(cp>>12));s+=char(0x80|((cp>>6)&63));s+=char(0x80|(cp&63));}return s;}
// Rows: unaccented, acute, grave, hook, tilde, dot. Codepoints keep NFC output.
inline constexpr uint32_t vowels[12][6]={
{'a',0xe1,0xe0,0x1ea3,0xe3,0x1ea1},{0x103,0x1eaf,0x1eb1,0x1eb3,0x1eb5,0x1eb7},{0xe2,0x1ea5,0x1ea7,0x1ea9,0x1eab,0x1ead},
{'e',0xe9,0xe8,0x1ebb,0x1ebd,0x1eb9},{0xea,0x1ebf,0x1ec1,0x1ec3,0x1ec5,0x1ec7},{'i',0xed,0xec,0x1ec9,0x129,0x1ecb},
{'o',0xf3,0xf2,0x1ecf,0xf5,0x1ecd},{0xf4,0x1ed1,0x1ed3,0x1ed5,0x1ed7,0x1ed9},{0x1a1,0x1edb,0x1edd,0x1edf,0x1ee1,0x1ee3},
{'u',0xfa,0xf9,0x1ee7,0x169,0x1ee5},{0x1b0,0x1ee9,0x1eeb,0x1eed,0x1eef,0x1ef1},{'y',0xfd,0x1ef3,0x1ef7,0x1ef9,0x1ef5}};
inline int vowel(uint32_t c){for(int r=0;r<12;++r)if(vowels[r][0]==c)return r;return -1;}
inline std::string word(const std::string& raw){std::vector<uint32_t> s;int tone=0;char lastModifier=0;for(char c:raw){
 int toneKey=c=='s'?1:c=='f'?2:c=='r'?3:c=='x'?4:c=='j'?5:c=='z'?0:-1;bool has=false;for(auto v:s)if(vowel(v)>=0)has=true;
 if(toneKey>=0&&has){if(c==lastModifier){s.push_back(c);tone=0;lastModifier=0;}else{tone=toneKey;lastModifier=c;}continue;}
 bool changed=false;
 if(c=='d'&&!s.empty()&&s.back()=='d'){s.back()=0x111;changed=true;}else if(c=='d'&&!s.empty()&&s.back()==0x111){s.back()='d';s.push_back('d');changed=true;}
 if(c=='w'){
  // uo+w becomes ươ; qu retains its consonantal u.
  for(int i=int(s.size())-1;i>=0;--i){int r=vowel(s[i]);if(r==6){s[i]=0x1a1;changed=true;if(i>0&&s[i-1]=='u'&&!(i>1&&s[i-2]=='q'))s[i-1]=0x1b0;break;}if(r==9){s[i]=0x1b0;changed=true;break;}if(r==0){s[i]=0x103;changed=true;break;}}
 }else if(c=='a'||c=='e'||c=='o')for(int i=int(s.size())-1;i>=0;--i){if(s[i]==uint32_t(c)){s[i]=c=='a'?0xe2:c=='e'?0xea:0xf4;changed=true;break;}if(s[i]==(c=='a'?0xe2u:c=='e'?0xeau:0xf4u)){s[i]=c;s.push_back(c);changed=true;break;}}
 if(!changed)s.push_back(c);if(c!=lastModifier)lastModifier=0;
 }
 std::vector<int> ids;for(int i=0;i<int(s.size());++i)if(vowel(s[i])>=0&&!(i==1&&s[0]=='q'&&s[i]=='u')&&!(i==1&&s[0]=='g'&&s[i]=='i'&&s.size()>2&&vowel(s[2])>=0))ids.push_back(i);
 if(!ids.empty()&&tone){int at=ids.back();for(int i:ids){int r=vowel(s[i]);if(r==1||r==2||r==4||r==7||r==8||r==10)at=i;}
 bool shaped=false;for(int i:ids){int r=vowel(s[i]);shaped|=r==1||r==2||r==4||r==7||r==8||r==10;}
 if(!shaped&&ids.size()==2&&ids.back()==int(s.size())-1)at=ids.front();else if(!shaped&&ids.size()==3)at=ids[1];s[at]=vowels[vowel(s[at])][tone];}
 std::string out;for(auto c:s)out+=utf8(c);return out;}
inline std::string convert(const std::string& raw){std::string out,w;for(char c:raw){if(c>='a'&&c<='z')w+=c;else{out+=word(w);w.clear();out+=c;}}return out+word(w);}
}
