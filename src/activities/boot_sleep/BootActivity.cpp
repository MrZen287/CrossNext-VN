#include "BootActivity.h"
#include <GfxRenderer.h>
#include <I18n.h>
#include "apps_local/ui/CompanionArt.h"
#include "apps_local/ui/PixelMenuTheme.h"
#include "fontIds.h"
void BootActivity::onEnter() {
  Activity::onEnter();renderer.clearScreen();toybox::ensureFonts(renderer);
  auto t=toybox::makeTarget(renderer);pixelui::bindFonts(t,renderer);
  const int w=renderer.getScreenWidth(),h=renderer.getScreenHeight(),side=std::min(w*2/3,h/3);
  companionart::logo(t,freeink::ui::makeRect((w-side)/2,h/2-side,side,side));
  renderer.drawCenteredText(toybox::kSerifTitleFontId,h/2+20,"CrossZen");
  renderer.drawCenteredText(SMALL_FONT_ID,h/2+65,CROSSPOINT_VERSION);
  renderer.drawCenteredText(SMALL_FONT_ID,h-70,tr(STR_BOOTING));
  renderer.displayBuffer();
}
