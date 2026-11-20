#include "CalculatorScreens.h"

#include <I18n.h>

#include <algorithm>
#include <cstring>

#include "../ui/Dashboard.h"
#include "../ui/FitText.h"

namespace calculatorui {
namespace fui = freeink::ui;
namespace {
fui::Rect rect(calc::Rect16 value) { return fui::makeRect(value.x, value.y, value.w, value.h); }
const char* errorLabel(const calc::ExpressionEngine& engine) {
  if (std::strcmp(engine.display(), "DIVIDE BY 0") == 0) return tr(STR_CALCULATOR_DIVIDE_ZERO);
  if (std::strcmp(engine.display(), "OVERFLOW") == 0) return tr(STR_CALCULATOR_OVERFLOW);
  return tr(STR_CALCULATOR_INVALID);
}
void result(toybox::Screen& screen, fui::Rect box, const calc::ExpressionEngine& engine) {
  if (engine.hasError()) {
    fui::TextStyle text;
    text.font = pixelui::kBody;
    text.align = fui::TextAlign::Right;
    text.maxLines = 2;
    screen.target().text(toybox::inkCentred(screen.target(), box, text), errorLabel(engine), text);
    return;
  }
  bool scalable = true;
  for (const unsigned char* p = reinterpret_cast<const unsigned char*>(engine.display()); *p; ++p)
    scalable = scalable && numerals::glyph(*p);
  if (scalable) {
    const int height = std::min(int(box.height), 92);
    numerals::draw(screen.target(), box, engine.display(), fui::Color::Black, numerals::activeFace(),
                   height * 1024 / numerals::digitHeight(), true);
  } else {
    pixelui::text(screen.target(), box, engine.display(), pixelui::kTitle, fui::Color::Black, fui::TextAlign::Right);
  }
}
}  // namespace
calc::PadGeom geometry(const fui::DeviceContext& device) {
  const auto body = pixelui::geometry(device).body;
  const bool wide = body.width > body.height;
  const int gap = wide ? 6 : 9;
  const int height = std::clamp(body.height - calc::kPad.rows * (44 + gap), 60, wide ? 96 : 226);
  return calc::padGeom(calc::kPad, {body.x, body.y, body.width, body.height}, gap, height);
}
int expressionDividerY(const calc::PadGeom& pad) { return pad.display.y + pad.display.h * 44 / 100; }

void build(toybox::Screen& screen, const calc::ExpressionEngine& engine) {
  pixelui::Header header;
  header.title = tr(STR_CALCULATOR_APP);
  header.back = true;
  pixelui::header(screen, header);
  const auto pad = geometry(screen.device());
  const auto display = rect(pad.display);
  lightui::surface(screen.target(), display, dashboard::paper);
  lightui::outline(screen.target(), display, dashboard::ink, pixelui::kBorder);
  const int split = expressionDividerY(pad);
  fui::TextStyle expression;
  expression.font = display.height >= 130 ? pixelui::kTitle : pixelui::kBody;
  expression.align = fui::TextAlign::Right;
  fittext::draw(screen.target(), fui::makeRect(display.x + 16, display.y + 3, display.width - 32, split - display.y - 6),
                engine.pending(), expression, 2);
  screen.target().fill(fui::makeRect(display.x + 16, split, display.width - 32, 1),
                       fui::Paint::dither(fui::Color::DarkGray));
  result(screen, fui::makeRect(display.x + 16, split + 3, display.width - 32, display.bottom() - split - 8), engine);
  for (int n = 0; n < calc::kPad.cols * calc::kPad.rows; ++n) {
    const auto& key = calc::kPad.keys[n];
    if (key.key == calc::Key::None) continue;
    const auto box = rect(calc::keyRect(calc::kPad, pad, n));
    const bool selected = key.key == calc::Key::Equals;
    const bool utility = n < 2 * calc::kPad.cols || calc::isBinaryOp(key.key);
    const auto background = selected ? dashboard::ink
                                     : utility ? fui::Paint::dither(fui::Color::LightGray) : dashboard::paper;
    screen.target().fill(box, background, 3);
    screen.target().stroke(box, dashboard::ink, 1, 3);
    if (key.key == calc::Key::Backspace) {
      const int cx = box.x + box.width / 2, cy = box.y + box.height / 2;
      const auto ink = dashboard::ink;
      screen.target().line({int16_t(cx - 14), int16_t(cy)}, {int16_t(cx - 5), int16_t(cy - 10)}, 2, ink);
      screen.target().line({int16_t(cx - 14), int16_t(cy)}, {int16_t(cx - 5), int16_t(cy + 10)}, 2, ink);
      screen.target().line({int16_t(cx - 5), int16_t(cy - 10)}, {int16_t(cx + 14), int16_t(cy - 10)}, 2, ink);
      screen.target().line({int16_t(cx - 5), int16_t(cy + 10)}, {int16_t(cx + 14), int16_t(cy + 10)}, 2, ink);
      screen.target().line({int16_t(cx + 14), int16_t(cy - 10)}, {int16_t(cx + 14), int16_t(cy + 10)}, 2, ink);
      screen.target().line({int16_t(cx - 1), int16_t(cy - 5)}, {int16_t(cx + 9), int16_t(cy + 5)}, 2, ink);
      screen.target().line({int16_t(cx - 1), int16_t(cy + 5)}, {int16_t(cx + 9), int16_t(cy - 5)}, 2, ink);
    } else if (calc::isBinaryOp(key.key)) {
      const int cx = box.x + box.width / 2, cy = box.y + box.height / 2;
      const auto ink = dashboard::ink;
      switch (key.key) {
        case calc::Key::Add:
          screen.target().line({int16_t(cx), int16_t(cy - 11)}, {int16_t(cx), int16_t(cy + 11)}, 3, ink);
          [[fallthrough]];
        case calc::Key::Sub:
          screen.target().line({int16_t(cx - 11), int16_t(cy)}, {int16_t(cx + 11), int16_t(cy)}, 3, ink);
          break;
        case calc::Key::Mul:
          screen.target().line({int16_t(cx - 9), int16_t(cy - 9)}, {int16_t(cx + 9), int16_t(cy + 9)}, 3, ink);
          screen.target().line({int16_t(cx - 9), int16_t(cy + 9)}, {int16_t(cx + 9), int16_t(cy - 9)}, 3, ink);
          break;
        case calc::Key::Div:
          screen.target().line({int16_t(cx - 11), int16_t(cy)}, {int16_t(cx + 11), int16_t(cy)}, 3, ink);
          screen.target().fill(fui::makeRect(cx - 2, cy - 11, 4, 4), ink, 2);
          screen.target().fill(fui::makeRect(cx - 2, cy + 8, 4, 4), ink, 2);
          break;
        default:
          break;
      }
    } else {
      pixelui::text(screen.target(), box.inset({3, 4, 3, 4}), key.label ? key.label : calc::kPlusMinus, pixelui::kBody,
                    selected ? fui::Color::White : fui::Color::Black, fui::TextAlign::Center, false,
                    pixelui::LabelCase::Preserve);
    }
    screen.frame().navigationGrid(box, 1, 1);
  }
}
}  // namespace calculatorui
