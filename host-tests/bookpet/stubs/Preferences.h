#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>
class Preferences {
 inline static std::map<std::string,std::vector<uint8_t>> data;
 std::string ns;
 public:
 bool begin(const char* n,bool){ns=n;return true;}
 void end(){}
 size_t getBytesLength(const char* k){return data[ns+k].size();}
 size_t getBytes(const char* k,void* out,size_t n){const auto& b=data[ns+k];if(b.size()<n)return 0;std::memcpy(out,b.data(),n);return n;}
 size_t putBytes(const char* k,const void* in,size_t n){const auto* b=static_cast<const uint8_t*>(in);data[ns+k]=std::vector<uint8_t>(b,b+n);return n;}
};
