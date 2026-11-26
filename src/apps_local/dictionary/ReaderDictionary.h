#pragma once

#include <FreeInkUI.h>
#include <Utf8.h>
#include <algorithm>
#include <vector>

#include "DictionaryStore.h"
#include "../ui/FitText.h"
#include "../ui/ToyboxTheme.h"
#include "activities/Activity.h"

class ReaderDictionary final : public Activity {
 public:
  ReaderDictionary(GfxRenderer& renderer, MappedInputManager& input, const std::string& selection)
      : Activity("ReaderDictionary", renderer, input) {
    const auto normalized = utf8ComposeNfc(selection);
    const auto* next = reinterpret_cast<const unsigned char*>(normalized.c_str());
    while (*next) {
      auto point = utf8NextCodepoint(&next);
      if ((point >= 'A' && point <= 'Z') || (point >= 0xc0 && point <= 0xde && point != 0xd7)) point += 32;
      else if (point == 0x102 || point == 0x110 || point == 0x1a0 || point == 0x1af ||
               (point >= 0x1ea0 && point <= 0x1ef8 && (point & 1) == 0)) ++point;
      utf8AppendCodepoint(point, term);
    }
  }

  void onEnter() override { Activity::onEnter(); load(); }
  void loop() override {
    using Button = MappedInputManager::Button;
    if (mappedInput.wasReleased(Button::Back)) { finish(); return; }
    int x = 0, y = 0;
    if (!mappedInput.wasScreenTapped(x, y)) return;
    const int height = renderer.getScreenHeight(), width = renderer.getScreenWidth();
    if (y < panelTop() || (y >= height - 44 && x < width / 3)) { finish(); return; }
    if (y >= panelTop() + 47 && y < panelTop() + 88) {
      const bool next = x >= width / 2;
      if (next != vietnamese) { vietnamese = next; load(); }
    } else if (y >= height - 44) {
      const int step = x >= width * 2 / 3 ? 1 : -1;
      page = std::clamp(page + step, 0, std::max(0, int(lines.size()) - rows()));
      requestUpdate();
    }
  }

  void render(RenderLock&&) override {
    namespace ui = freeink::ui;
    auto target = toybox::makeTarget(renderer);
    toybox::ensureFonts(renderer);
    target.setFont(0, toybox::kButtonFontId);
    target.setFont(1, toybox::kUiFontId);
    const int width = renderer.getScreenWidth(), height = renderer.getScreenHeight(), top = panelTop();
    const auto white = ui::Paint::solid(ui::Color::White), black = ui::Paint::solid(ui::Color::Black);
    const auto panel = ui::makeRect(7, top, width - 14, height - top - 4);
    target.fill(panel, white, 8);
    target.stroke(panel, black, 1, 8);
    auto label = [&](ui::Rect box, const char* value, int font, bool selected = false) {
      ui::TextStyle style;
      style.font = font;
      style.bold = selected;
      style.align = ui::TextAlign::Center;
      fittext::draw(target, box, value, style, 1);
    };
    label(ui::makeRect(18, top + 5, width - 36, 37), term.c_str(), 1, true);
    for (int n = 0; n < 2; ++n) {
      const auto tab = ui::makeRect(17 + n * (width / 2 - 10), top + 47, width / 2 - 19, 40);
      target.stroke(tab, black, n == int(vietnamese) ? 2 : 1, 6);
      label(tab, n ? "Việt → Việt" : "Anh → Việt", 0, n == int(vietnamese));
    }
    for (int n = 0; n < rows() && page + n < int(lines.size()); ++n) {
      ui::TextStyle style;
      style.font = 0;
      fittext::draw(target, ui::makeRect(20, top + 95 + n * 27, width - 40, 27), lines[page + n].c_str(), style, 1);
    }
    label(ui::makeRect(12, height - 44, width / 3 - 12, 36), "Đóng", 0);
    label(ui::makeRect(width / 3, height - 44, width / 3, 36), "‹", 1);
    label(ui::makeRect(width * 2 / 3, height - 44, width / 3 - 12, 36), "›", 1);
    renderer.displayBuffer(HalDisplay::FAST_REFRESH);
  }

 private:
  std::string term;
  std::vector<std::string> lines;
  bool vietnamese = false;
  int page = 0;
  int panelTop() const { return std::max(10, renderer.getScreenHeight() - 370); }
  int rows() const { return std::max(1, (renderer.getScreenHeight() - panelTop() - 142) / 27); }
  void load() {
    zendictionary::Store store;
    std::string definition;
    if (!store.openPath(vietnamese ? "viet-viet" : "anh-viet"))
      definition = vietnamese ? "Chưa có gói Việt → Việt trên thẻ nhớ." : "Chưa có gói Anh → Việt trên thẻ nhớ.";
    else if (!store.exact(term, definition))
      definition = "Không tìm thấy nghĩa cho mục từ này.";
    lines.clear();
    page = 0;
    toybox::ensureFonts(renderer);
    auto target = toybox::makeTarget(renderer);
    target.setFont(0, toybox::kButtonFontId);
    freeink::ui::TextStyle style;
    style.font = 0;
    style.maxLines = 16;
    const auto area = freeink::ui::makeRect(0, 0, std::max(1, renderer.getScreenWidth() - 40), 600);
    size_t start = 0;
    while (start < definition.size() && lines.size() < 200) {
      const size_t end = definition.find('\n', start);
      const auto paragraph = definition.substr(start, end == std::string::npos ? end : end - start);
      freeink::ui::layoutText(target, area, paragraph.c_str(), style, [&](const char* line, freeink::ui::Rect) {
        if (lines.size() < 200) lines.emplace_back(line);
      });
      if (end == std::string::npos) break;
      start = end + 1;
    }
    requestUpdate();
  }
};
