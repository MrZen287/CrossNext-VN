#include "ChickenGame.h"

#include <HalStorage.h>
#include <I18n.h>
#include <Memory.h>
#include <esp_random.h>

#include <algorithm>
#include <cstdio>
#include <ctime>

#include "../Shelf.h"
#include "../pet/PetState.h"
#include "../ui/ChickenArt.h"
#include "../ui/PixelMenuTheme.h"
#include "components/SideButtonNavigation.h"

namespace fui = freeink::ui;

namespace {
constexpr char SAVE_A[] = "/.crosspoint/chicken-a.bin";
constexpr char SAVE_B[] = "/.crosspoint/chicken-b.bin";
constexpr uint32_t MAGIC = 0x314e4548;  // HEN1
enum Action : fui::ActionId { Back = 1, Warm, Feed, Play, Rest, Pick };
enum Note : uint8_t { NoNote, Fed, Found, Missed, Hungry, Tired, Full, StorageError };

uint32_t ageNow() {
  const auto t = std::time(nullptr);
  return t >= 1577836800 && uint64_t(t) < UINT32_MAX ? uint32_t(t) : 0;
}
}

std::unique_ptr<Activity> ChickenGame::create(GfxRenderer& renderer, MappedInputManager& input) {
  return makeUniqueNoThrow<ChickenGame>(renderer, input);
}

void ChickenGame::onEnter() {
  Activity::onEnter();
  toybox::ensureFonts(renderer);
  bool found = false;
  for (const char* path : {SAVE_A, SAVE_B}) {
    HalFile file;
    if (!Storage.exists(path)) continue;
    hadSave = true;
    Save candidate{};
    if (Storage.openFileForRead("HEN", path, file) && file.size() == sizeof(candidate) &&
        file.read(&candidate, sizeof(candidate)) == sizeof(candidate) && valid(candidate) &&
        (!found || int32_t(candidate.sequence - state.sequence) > 0)) {
      state = candidate;
      found = true;
    }
  }
  writable = found || !hadSave;
  if (!writable) note = StorageError;
  advance(state);
  requestUpdate();
}

bool ChickenGame::valid(const Save& value) {
  return value.magic == MAGIC && value.xp <= 100 && value.food <= 100 && value.energy <= 100 &&
         value.joy <= 100 && value.warmth <= 4 && value.resting <= 1 && value.reserved == 0 &&
         value.hash == pet::checksum(reinterpret_cast<const uint8_t*>(&value), offsetof(Save, hash));
}

bool ChickenGame::commit(Save value) {
  value.magic = MAGIC;
  value.sequence = state.sequence + 1;
  value.hash = pet::checksum(reinterpret_cast<const uint8_t*>(&value), offsetof(Save, hash));
  const auto* path = value.sequence & 1 ? SAVE_A : SAVE_B;
  if (!writable || !Storage.ensureDirectoryExists("/.crosspoint")) return false;
  {
    HalFile file;
    if (!Storage.openFileForWrite("HEN", path, file) || file.write(&value, sizeof(value)) != sizeof(value))
      return false;
    file.flush();
  }
  HalFile verify;
  Save candidate{};
  if (!Storage.openFileForRead("HEN", path, verify) || verify.read(&candidate, sizeof(candidate)) != sizeof(candidate) ||
      candidate.sequence != value.sequence || !valid(candidate))
    return false;
  state = candidate;
  return true;
}

void ChickenGame::advance(Save& value) {
  const auto now = ageNow();
  if (!now) return;
  if (!value.time || now < value.time || value.warmth < 4) {
    value.time = now;
    return;
  }
  constexpr uint32_t interval = 15 * 60;
  const unsigned steps = (now - value.time) / interval;
  if (!steps) return;
  value.time += steps * interval;
  value.food = std::max(0, int(value.food) - int(std::min(steps, 20u)) * 5);
  value.joy = std::max(0, int(value.joy) - int(std::min(steps, 25u)) * 4);
  if (value.resting) value.energy = std::min(100, int(value.energy) + int(std::min(steps, 5u)) * 20);
}

void ChickenGame::act(int action, int choice) {
  if (action == Back) {
    shelf::leave(renderer, mappedInput);
    return;
  }
  if (!writable) return;
  auto next = state;
  advance(next);
  note = NoNote;
  switch (action) {
    case Warm:
      if (next.warmth >= 4) return;
      ++next.warmth;
      break;
    case Feed:
      if (next.food >= 90) {
        note = Full;
        return;
      }
      next.food = std::min(100, next.food + 26);
      next.xp = std::min(100, next.xp + 1);
      note = Fed;
      break;
    case Play:
      if (next.food < 20) note = Hungry;
      else if (next.energy < 16 || next.resting) note = Tired;
      else {
        playing = true;
        targetSeed = esp_random() % 3;
      }
      return;
    case Rest:
      next.resting = !next.resting;
      if (next.resting) {
        next.energy = std::min(100, next.energy + 12);
        next.food = std::max(0, next.food - 5);
      }
      break;
    case Pick:
      if (!playing || next.energy < 16 || next.food < 20) {
        playing = false;
        note = Tired;
        return;
      }
      playing = false;
      note = choice == targetSeed ? Found : Missed;
      next.food -= 8;
      next.energy -= 16;
      next.joy = std::min(100, next.joy + (note == Found ? 26 : 14));
      next.xp = std::min(100, next.xp + (note == Found ? 3 : 1));
      break;
    default: return;
  }
  if (!commit(next)) note = StorageError;
}

void ChickenGame::loop() {
  if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
    if (!playing) return shelf::leave(renderer, mappedInput);
    playing = false;
    requestUpdate();
    return;
  }
  int x = 0, y = 0;
  if (!ready || !interactions.routable() || !sidebuttons::poll(interactions, mappedInput, x, y)) return;
  fui::InputSnapshot input{};
  input.touchReleased = sidebuttons::hasActivation(x, y);
  input.touchX = x;
  input.touchY = y;
  const auto event = sidebuttons::route(interactions, input, mappedInput, *this);
  if (event.action) {
    ready = false;
    act(event.action, event.value);
    requestUpdate();
  }
}

void ChickenGame::render(RenderLock&&) {
  ready = false;
  renderer.clearScreen();
  auto target = toybox::makeTarget(renderer);
  pixelui::bindFonts(target, renderer);
  const fui::InputSnapshot input{};
  toybox::Frame frame(target, target.deviceContext(), input, interactions);
  toybox::Screen screen(frame);
  const auto safe = target.deviceContext().safeRect().inset({12, 12, 12, 12});
  const bool wide = safe.width > safe.height;
  const auto area = wide ? fui::makeRect(safe.x + safe.width / 2, safe.y + 55, safe.width / 2, safe.height - 55)
                         : fui::makeRect(safe.x, safe.y + safe.height / 2, safe.width, safe.height / 2);
  fui::TextStyle title;
  title.font = pixelui::kBody;
  title.bold = true;
  title.align = fui::TextAlign::Center;
  auto copy = title;
  copy.bold = false;
  const auto text = [&](int y, int h, const char* value, const auto& style) {
    target.text(fui::makeRect(safe.x, y, safe.width, h), value, style);
  };
  text(safe.y, 37, tr(STR_CHICKEN_GAME), title);
  const auto button = [&](fui::Rect box, const char* caption, int action, int value = 0) {
    fui::ButtonProps props{};
    props.styles = pixelui::buttonStyles();
    props.action = action;
    props.value = value;
    props.label = caption;
    props.text = copy;
    screen.button(props, box);
  };
  button(fui::makeRect(safe.x, safe.y, 78, 40), "‹", Back);

  const unsigned stage = state.warmth < 4 ? 0 : state.xp < 10 ? 1 : state.xp < 25 ? 2 : state.xp < 48 ? 3 : 4;
  const auto image = wide ? fui::makeRect(safe.x, safe.y + 46, safe.width / 2, safe.height - 54)
                          : fui::makeRect(safe.x + 8, safe.y + 64, safe.width - 16, safe.height / 2 - 70);
  chickenart::draw(target, image, stage, state.resting ? 3 : state.food < 20 ? 2 : note == Found ? 1 : 0);
  static constexpr StrId stages[] = {StrId::STR_PET_EGG, StrId::STR_PET_NEWBORN, StrId::STR_PET_BABY,
                                    StrId::STR_PET_JUVENILE, StrId::STR_PET_ADULT};
  target.text(fui::makeRect(image.x, safe.y + 37, image.width, 26), I18N.get(stages[stage]), copy);
  const int w = (area.width - 12) / 2;
  const int row = std::clamp((int(area.height) - 88) / 3, 44, 59);
  char copyBuffer[80];
  if (!stage)
    std::snprintf(copyBuffer, sizeof(copyBuffer), tr(STR_CHICKEN_EGG_COUNT), unsigned(state.warmth));
  else
    std::snprintf(copyBuffer, sizeof(copyBuffer), tr(STR_CHICKEN_STATS),
                  unsigned(state.food), unsigned(state.joy), unsigned(state.energy));
  auto detail = copy;
  detail.font = pixelui::kSmall;
  target.text(fui::makeRect(area.x, area.y, area.width, 30), copyBuffer, detail);
  static constexpr StrId notices[] = {StrId::STR_NONE_OPT, StrId::STR_CHICKEN_FED, StrId::STR_CHICKEN_FOUND,
                                      StrId::STR_CHICKEN_MISSED, StrId::STR_CHICKEN_HUNGRY, StrId::STR_CHICKEN_TIRED,
                                      StrId::STR_CHICKEN_FULL, StrId::STR_CHICKEN_SAVE_ERROR};
  if (note || playing)
    target.text(fui::makeRect(area.x, area.y + 34, area.width, 38),
                playing ? tr(STR_CHICKEN_PICK) : I18N.get(notices[note]), detail);
  const int y = area.y + 80;
  if (playing) {
    const int col = (area.width - 16) / 3;
    for (int n = 0; n < 3; ++n) {
      static constexpr const char* bowls[] = {"1", "2", "3"};
      button(fui::makeRect(area.x + n * (col + 8), y, col, row), bowls[n], Pick, n);
    }
  } else if (!stage) {
    button(fui::makeRect(area.x, y, area.width, row), tr(STR_CHICKEN_WARM), Warm);
  } else {
    button(fui::makeRect(area.x, y, w, row), tr(STR_CHICKEN_FEED), Feed);
    button(fui::makeRect(area.x + w + 12, y, w, row), tr(STR_CHICKEN_PLAY), Play);
    button(fui::makeRect(area.x, y + row + 8, area.width, row),
           state.resting ? tr(STR_CHICKEN_WAKE) : tr(STR_CHICKEN_REST), Rest);
  }
  renderer.displayBuffer(HalDisplay::FAST_REFRESH);
  ready = true;
}

