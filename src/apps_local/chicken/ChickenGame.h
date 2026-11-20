#pragma once

#include "activities/Activity.h"
#include "../ui/ToyboxScreen.h"

class ChickenGame final : public Activity {
 public:
  ChickenGame(GfxRenderer& renderer, MappedInputManager& input) : Activity("ChickenGame", renderer, input) {}
  static std::unique_ptr<Activity> create(GfxRenderer&, MappedInputManager&);
  void onEnter() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  struct Save {
    uint32_t magic = 0;
    uint32_t sequence = 0;
    uint32_t time = 0;
    uint8_t xp = 0, food = 75, energy = 75, joy = 75;
    uint8_t warmth = 0, resting = 0;
    uint16_t reserved = 0;
    uint32_t hash = 0;
  };
  static_assert(sizeof(Save) == 24);
  static bool valid(const Save&);
  static void advance(Save&);
  bool commit(Save);
  void act(int action, int choice);

  Save state{};
  toybox::Interactions interactions;
  bool ready = false, hadSave = false, writable = true, playing = false;
  int targetSeed = 0;
  uint8_t note = 0;
};

