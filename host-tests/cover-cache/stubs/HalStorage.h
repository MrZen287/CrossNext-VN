#pragma once
#include <string>
#include <set>
#include <vector>
class HalFile{public:bool opened=false;void close(){opened=false;}};
struct TestStorage{std::set<std::string>files;std::vector<std::string>requests;bool exists(const char*p){return files.count(p);}bool openFileForRead(const char*,const char*p,HalFile&f){requests.push_back(p);return f.opened=exists(p);}};inline TestStorage Storage;
