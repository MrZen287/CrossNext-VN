#pragma once
#include "activities/Activity.h"
#include "DictionaryUi.h"
#include "DictionaryStore.h"
#include <vector>
class ZenDictionaryActivity final:public Activity {
 public:
 ZenDictionaryActivity(GfxRenderer&r,MappedInputManager&i):Activity("ZenDictionary",r,i){}
 static std::unique_ptr<Activity> create(GfxRenderer&,MappedInputManager&);
 void onEnter()override;void onExit()override;void loop()override;void render(RenderLock&&)override;
 private:
 zendictionary::Store store;std::array<zendictionary::Entry,6> entries;int count=0,page=0;bool vi=false,telex=false,opened=false,detail=false,pending=false;
 bool keyboardOpen=false;
 int previewKey=0;
 std::string raw,query,status,heading;std::vector<std::string> lines;
 void changed();void lookup(int);void wrap(const std::string&);
};
