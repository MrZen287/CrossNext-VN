#pragma once
#include "activities/Activity.h"
#include "PetState.h"
#include <FreeInkUI.h>
#include <array>
class BookPetActivity final:public Activity {
 public:
  BookPetActivity(GfxRenderer& r,MappedInputManager& i):Activity("BookPet",r,i){}
  static std::unique_ptr<Activity> create(GfxRenderer&,MappedInputManager&);
  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;
 private:
  enum View {Home,Game,Species,Toys,Diary};
  View view=Home;
  bookpet::PetEngine pet;
  std::array<freeink::ui::Rect,10> hits{};
  int selected=0,count=0,lane=0;
  uint32_t revealUntil=0,poseAt=0,revision=1;
  bool revealing=false,settled=false,won=false;
  const char* notice=nullptr;
  void action(int);
  void beginGame();
  uint32_t surfaceMeaning() const override {return revision;}
};
