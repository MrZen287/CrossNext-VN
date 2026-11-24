#pragma once
#include <cstdio>
#include <cstdint>
#include <string>
#include <cstdlib>
class HalFile{FILE*f=nullptr;public:~HalFile(){close();}void close(){if(f)fclose(f);f=nullptr;}bool open(const char*p){close();f=fopen(p,"rb");return f;}bool seek(size_t p){return f&&!fseek(f,p,SEEK_SET);}int read(uint8_t*p,size_t n){return f?int(fread(p,1,n,f)):0;}size_t size(){if(!f)return 0;auto p=ftell(f);fseek(f,0,SEEK_END);auto n=ftell(f);fseek(f,p,SEEK_SET);return n;}};
struct StorageStub{bool openFileForRead(const char*,const std::string&p,HalFile&f){return f.open((std::string(getenv("ZEN_DICT_ROOT"))+p).c_str());}};inline StorageStub Storage;
