#pragma once
#include <array>
#include <cstdint>
namespace crosshihi {
struct Version { std::array<uint32_t,4> parts{}; bool valid=false; bool prerelease=false; };
inline Version parseVersion(const char* s) {
  Version v;if(!s)return v;if(*s=='v'||*s=='V')++s;
  int n=0;
  for(;n<4;++n) {
    if(*s<'0'||*s>'9')return v;
    uint32_t value=0;
    do {if(value>100000000u)return v;value=value*10+unsigned(*s-'0');++s;}while(*s>='0'&&*s<='9');
    v.parts[n]=value;
    if(*s!='.')break;
    ++s;
  }
  if(n<2||n>3||(*s&&*s!='-'&&*s!='+'))return v;
  v.prerelease=*s=='-';v.valid=true;return v;
}
inline bool newerVersion(const char* latest,const char* current) {
  const auto l=parseVersion(latest),c=parseVersion(current);
  if(!l.valid||!c.valid)return false;
  if(l.parts!=c.parts)return l.parts>c.parts;
  return c.prerelease&&!l.prerelease;
}
}
