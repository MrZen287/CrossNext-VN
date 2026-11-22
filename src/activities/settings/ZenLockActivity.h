#pragma once
#include "activities/UiListActivity.h"
class ZenLockActivity final : public UiListActivity {
 public:
  ZenLockActivity(GfxRenderer& r,MappedInputManager& i):UiListActivity("ZenLock",r,i,false,true){}
 private:
  int listCount() const override {return 3;}
  const char* headerTitle() const override;
  void buildScreen(UiScreen&) override;
  void activateIndex(int) override;
  freeink::ui::ListItem rows[3]{};
};
