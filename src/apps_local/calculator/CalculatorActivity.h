#pragma once
#include <memory>

#include "../../activities/Activity.h"
#include "../ui/ToyboxScreen.h"
#include "CalcExpression.h"

class CalculatorActivity final : public Activity {
 public:
  CalculatorActivity(GfxRenderer& renderer, MappedInputManager& input) : Activity("Calculator", renderer, input) {}
  static std::unique_ptr<Activity> create(GfxRenderer&, MappedInputManager&);
  void onEnter() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  calc::ExpressionEngine engine;
  toybox::Interactions interactions;
  bool ready = false;
};
