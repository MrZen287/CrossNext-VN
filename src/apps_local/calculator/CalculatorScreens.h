#pragma once
#include "../ui/ToyboxScreen.h"
#include "CalcExpression.h"
#include "CalcLayout.h"

namespace calculatorui {
calc::PadGeom geometry(const freeink::ui::DeviceContext& device);
int expressionDividerY(const calc::PadGeom& pad);
void build(toybox::Screen& screen, const calc::ExpressionEngine& engine);
}  // namespace calculatorui
