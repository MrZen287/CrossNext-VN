#pragma once
#include <HalStorage.h>
#include <array>
#include <algorithm>
#include <string>
#include <cstdint>
namespace zendictionary {
struct Entry{std::string word,preview;uint32_t definition=0,length=0;};
class Store {
 HalFile index,data;uint32_t count=0;
 static uint32_t u32(const uint8_t*p){return uint32_t(p[0])|(uint32_t(p[1])<<8)|(uint32_t(p[2])<<16)|(uint32_t(p[3])<<24);}
 bool read(uint32_t n,Entry&e){uint8_t r[16];if(n>=count||!index.seek(12+n*16)||index.read(r,16)!=16)return false;auto pos=u32(r),len=u32(r+4);if(!len||len>256||!data.seek(pos))return false;char word[257];if(data.read(reinterpret_cast<uint8_t*>(word),len)!=int(len))return false;word[len]=0;e.word=word;e.definition=u32(r+8);e.length=u32(r+12);return e.definition<=data.size()&&e.length<=data.size()-e.definition;}
 public:
 void close(){index.close();data.close();count=0;}
 bool open(bool vi){close();const char*base=vi?"/dictionaries/CrossZen/viet-anh":"/dictionaries/CrossZen/anh-viet";std::string p=base;uint8_t head[12];if(!Storage.openFileForRead("ZDICT",p+".zidx",index)||index.read(head,12)!=12||std::string(reinterpret_cast<char*>(head),8)!="ZDIC0001"||!Storage.openFileForRead("ZDICT",p+".zdat",data)){close();return false;}count=u32(head+8);if(!count||count>(index.size()-12)/16){close();return false;}return true;}
 bool suggestions(const std::string&q,std::array<Entry,3>&entries,int&total){total=0;if(!count||q.empty())return true;uint32_t lo=0,hi=count;Entry e;while(lo<hi){uint32_t mid=lo+(hi-lo)/2;if(!read(mid,e))return false;if(e.word<q)lo=mid+1;else hi=mid;}
 for(uint32_t n=lo;n<count&&total<3;++n){if(!read(n,e))return false;if(e.word.compare(0,q.size(),q))break;char text[121];size_t len=std::min<uint32_t>(120,e.length);if(!data.seek(e.definition)||data.read(reinterpret_cast<uint8_t*>(text),len)!=int(len))return false;text[len]=0;if(len==120){size_t cut=len;while(cut&&text[cut-1]!=' '&&text[cut-1]!='\n')--cut;if(cut)text[cut-1]=0;}for(size_t i=0;i<len;++i)if(text[i]=='\n'){text[i]=0;break;}e.preview=text;entries[total++]=std::move(e);}return true;}
 bool definition(const Entry&e,std::string&text){if(e.length>65536)return false;auto len=e.length;text.resize(len);return data.seek(e.definition)&&data.read(reinterpret_cast<uint8_t*>(text.data()),len)==int(len);}
};
}
