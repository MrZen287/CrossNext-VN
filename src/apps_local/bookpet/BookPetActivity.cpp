#include "BookPetActivity.h"
#include "apps_local/ui/CompanionArt.h"
#include "apps_local/ui/CrossZenArt.h"
#include "apps_local/Shelf.h"
#include "apps_local/ui/PixelMenuTheme.h"
#include "apps_local/ui/FitText.h"
#include "activities/boot_sleep/LunarSleepCalendar.h"
#include <GfxRenderer.h>
#include <I18n.h>
#include <Memory.h>
#include <esp_random.h>
#include <cstdio>
namespace fui=freeink::ui;
using fui::makeRect;
namespace {
const auto ink=fui::Paint::solid(fui::Color::Black);
void copy(fui::DrawTarget& t,fui::Rect r,const char* text,int font=0,bool bold=false){
 fui::TextStyle s;s.font=font;s.bold=bold;s.maxLines=1;s.align=fui::TextAlign::Center;fittext::draw(t,r,text,s,1);
}
}
std::unique_ptr<Activity> BookPetActivity::create(GfxRenderer& r,MappedInputManager& i){return makeUniqueNoThrow<BookPetActivity>(r,i);}
void BookPetActivity::onEnter(){Activity::onEnter();pet.begin();poseAt=millis()+15000;requestUpdate();}
void BookPetActivity::onExit(){pet.save();Activity::onExit();}
void BookPetActivity::beginGame(){view=Game;selected=0;lane=esp_random()%3;revealing=true;settled=false;revealUntil=millis()+1800;pet.apply(bookpet::PetAction::Play);++revision;}
void BookPetActivity::action(int n){
 const bool vi=I18N.getLanguage()==Language::VI;notice=nullptr;
 if(view==Home){switch(n){
  case 0:if(!pet.state().food)notice=vi?"Hết thức ăn? Dùng xu để mua thêm.":"Use coins to buy more food.";pet.apply(bookpet::PetAction::Feed);expression=1;break;
  case 1:beginGame();break;
  case 2:pet.apply(bookpet::PetAction::Clean);expression=2;break;
  case 3:if(pet.state().sleeping)pet.wake();else pet.apply(bookpet::PetAction::Rest);break;
  case 4:if(!pet.buyFood())notice=vi?"Cần 3 xu để mua thức ăn.":"You need 3 coins for food.";break;
  case 5:pet.wake();expression=(expression+1)%3;notice=vi?"Mèo dụi đầu, Mọt vẫy tay!":"Cat nuzzles, Worm waves!";break;
  case 6:view=Toys;selected=0;break;
  case 7:view=Diary;selected=0;break;
 }}else if(view==Game){
  if(revealing)return;
  if(settled){if(n==0)beginGame();else {view=Home;selected=0;}}
  else {won=n==lane;pet.completePageCatch(won);settled=true;selected=0;}
 }else if(view==Toys){if(n<4&&!pet.equipToy(n))notice=vi?"Chơi 5 ván để mở thêm đồ chơi.":"Play 5 rounds to unlock a toy.";else {view=Home;selected=0;}}
 else {view=Home;selected=0;}
 ++revision;requestUpdate();
}
void BookPetActivity::loop(){
 RenderLock lock(RenderLock::Mode::Try);if(!lock.ownsLock())return;
 const auto now=millis();
 if(view==Game&&revealing&&int32_t(now-revealUntil)>=0){revealing=false;++revision;requestUpdate();}
 if(pet.tick(now))requestUpdate();
 if(view==Home&&int32_t(now-poseAt)>=0){pet.awakeMoment();expression=pet.state().idleVariant%3;poseAt=now+15000;requestUpdate();}
 if(mappedInput.wasReleased(MappedInputManager::Button::Back)){if(view==Home)shelf::leave(renderer,mappedInput);else {view=Home;selected=0;++revision;requestUpdate();}return;}
 if(mappedInput.wasPressed(MappedInputManager::Button::NavNext)){selected=(selected+1)%std::max(1,count);requestUpdate();}
 if(mappedInput.wasPressed(MappedInputManager::Button::NavPrevious)){selected=(selected+std::max(1,count)-1)%std::max(1,count);requestUpdate();}
 if(mappedInput.wasReleased(MappedInputManager::Button::Confirm)&&surfaceRevealed()){action(selected);return;}
 int x=0,y=0;if(mappedInput.wasScreenTapped(x,y)&&surfaceRevealed()){
  if(x<52&&y<56){if(view==Home)shelf::leave(renderer,mappedInput);else {view=Home;selected=0;++revision;requestUpdate();}return;}
  for(int n=0;n<count;++n)if(hits[n].contains(x,y)){selected=n;action(n);return;}
 }
}
void BookPetActivity::render(RenderLock&&){
 renderer.clearScreen();auto t=toybox::makeTarget(renderer);pixelui::bindFonts(t,renderer);
 const int w=renderer.getScreenWidth(),h=renderer.getScreenHeight();const bool vi=I18N.getLanguage()==Language::VI;
 copy(t,makeRect(5,10,42,36),"<",1);copy(t,makeRect(50,10,w-100,40),vi?"Mèo & Mọt":"Cat & Worm",1,true);
 hits.fill({});count=0;
 auto button=[&](int n,fui::Rect r,const char* label){hits[n]=r;count=std::max(count,n+1);t.stroke(r,ink,n==selected?2:1,4);copy(t,r,label,0,n==selected);};
 const auto& s=pet.state();char buf[160];
 if(view==Home){
  const int artH=std::clamp(h/4,140,200),artTop=70;
  const int catW=std::min(w*43/100,180),wormW=std::min(w*30/100,130);
  const int left=(w-catW-wormW-16)/2;
  // Shared care profile, two companions in one room. No external sprite pack.
  const bool sleepy=s.sleeping||s.energy<25;
  unsigned pose=sleepy?1:expression==1?3:expression==2||pet.mood()==bookpet::PetMood::Happy?4:0;
  companionart::cat(t,makeRect(left,artTop,catW,artH),pose);
  const auto worm=makeRect(left+catW+16,artTop+artH-wormW,wormW,wormW);
  if(sleepy)zenart::wormRest(t,worm);else if(expression||s.idleVariant%2)zenart::wormHappy(t,worm);else companionart::worm(t,worm);
  t.line({int16_t(left-8),int16_t(artTop+artH+4)},{int16_t(w-left+8),int16_t(artTop+artH+4)},1,ink);
  int y=artTop+artH+12;
  std::snprintf(buf,sizeof(buf),vi?"Cấp %u · %u xu · %u thức ăn":"Level %u · %u coins · %u food",s.level,s.pageBites,s.food);copy(t,makeRect(12,y,w-24,32),buf,0,true);y+=38;
  const char* namesVi[]={"No bụng","Vui vẻ","Năng lượng","Sạch sẽ"};const char* namesEn[]={"Fullness","Happiness","Energy","Cleanliness"};int values[]={s.fullness,s.happiness,s.energy,s.cleanliness};
  for(int n=0;n<4;++n){std::snprintf(buf,sizeof(buf),"%s %d%%",vi?namesVi[n]:namesEn[n],values[n]);copy(t,makeRect(10,y,w/2-10,26),buf);t.stroke(makeRect(w/2,y+8,w/2-24,8),ink,1);t.fill(makeRect(w/2,y+8,(w/2-24)*values[n]/100,8),ink);y+=30;}
  const int bh=std::clamp((h-y-120)/2,38,56),bw=(w-40)/2;
  const char* labels[]={vi?"Cho ăn":"Feed",vi?"Chơi":"Play",vi?"Vệ sinh":"Clean",s.sleeping?(vi?"Thức dậy":"Wake"):(vi?"Nghỉ":"Rest")};
  for(int n=0;n<4;++n)button(n,makeRect(14+(n%2)*(bw+10),y+(n/2)*(bh+8),bw,bh),labels[n]);y+=2*(bh+8)+5;
  const char* extra[]={vi?"Mua ăn":"Buy food",vi?"Vuốt ve":"Pet them",vi?"Đồ chơi":"Toys",vi?"Nhật ký":"Diary"};
  for(int n=0;n<4;++n)button(n+4,makeRect(8+n*(w-16)/4,y,(w-16)/4-4,34),extra[n]);
  copy(t,makeRect(12,h-72,w-24,28),notice?notice:(vi?"Lớn lên nhờ chăm sóc và chơi.":"Grow through care and play."));
 }else if(view==Game){
  copy(t,makeRect(15,100,w-30,45),vi?"Nhớ vị trí trang sách":"Remember the page",1,true);
  copy(t,makeRect(15,155,w-30,40),revealing?(vi?"Nhìn kỹ vị trí có hình sách":"Look for the book"):settled?(won?(vi?"Đúng rồi! +1 xu":"Correct! +1 coin"):(vi?"Chưa đúng, thử lại nhé":"Not quite. Try again")):(vi?"Chạm vị trí vừa nhìn thấy":"Tap the spot you saw"));
  if(!settled)for(int n=0;n<3;++n)button(n,makeRect(15+n*(w-30)/3,230,(w-30)/3-8,100),revealing&&n==lane?"[BOOK]":"?");
  else {button(0,makeRect(20,240,w-40,60),vi?"Chơi tiếp":"Play again");button(1,makeRect(20,320,w-40,60),vi?"Về nhà":"Home");}
 }else {
  const int n=view==Toys?4:3;
  const char* toysVi[]={"Bóng","Chuông","Khối chữ","Diều"};
  for(int i=0;i<n;++i){bool unlocked=view==Toys?pet.toyUnlocked(i):true;
   const char* label=view==Toys?(vi?toysVi[i]:pet.toyName(i)):(vi?"Một kỷ niệm chăm sóc thú.":pet.diaryLine(i));
   std::snprintf(buf,sizeof(buf),"%s%s",label,unlocked?"":(vi?" · Chưa mở":" · Locked"));button(i,makeRect(20,100+i*80,w-40,65),buf);
  }
  button(n,makeRect(20,110+n*80,w-40,50),vi?"Quay lại":"Back");
  if(notice)copy(t,makeRect(15,h-85,w-30,35),notice);
 }
 lunarSleepCalendar::footer(renderer);renderer.displayBuffer(HalDisplay::FAST_REFRESH);
}
