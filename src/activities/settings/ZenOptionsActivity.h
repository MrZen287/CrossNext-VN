#pragma once
#include "activities/UiListActivity.h"
class ZenOptionsActivity final:public UiListActivity {
 public:
  enum class Kind {Widgets,Edges,Guides};
  ZenOptionsActivity(GfxRenderer& r,MappedInputManager& i,Kind kind):UiListActivity("ZenOptions",r,i,false,true),kind(kind){}
 private:
  Kind kind;
  int slot=-1;
  bool failed=false;
  freeink::ui::ListItem rows[6]{};
  int listCount() const override {return kind==Kind::Guides?6:slot>=0?(kind==Kind::Widgets?4:3):(kind==Kind::Widgets?3:2);}
  const char* headerTitle() const override;
  void buildScreen(UiScreen&) override;
  void activateIndex(int) override;
  void onBackButton() override;
  uint8_t& value(int);
};
