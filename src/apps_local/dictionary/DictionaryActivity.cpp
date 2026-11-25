#include "DictionaryActivity.h"
#include "Telex.h"
#include "apps_local/ui/PixelMenuTheme.h"
#include "activities/ActivityManager.h"
#include <HalClock.h>
#include <Memory.h>
#include <algorithm>
std::unique_ptr<Activity> ZenDictionaryActivity::create(GfxRenderer&r,MappedInputManager&i){return makeUniqueNoThrow<ZenDictionaryActivity>(r,i);}
void ZenDictionaryActivity::onEnter(){Activity::onEnter();toybox::ensureFonts(renderer);opened=store.open(vi);changed();}
void ZenDictionaryActivity::onExit(){store.close();Activity::onExit();}
void ZenDictionaryActivity::changed(){query=telex?zentelex::convert(raw):raw;detail=false;count=0;pending=true;status=opened?"":"Chưa có dữ liệu. Chép gói từ điển vào thẻ.";requestUpdate();}
void ZenDictionaryActivity::wrap(const std::string&text){lines.clear();std::string line,word;auto flush=[&]{if(line.size()+word.size()>500||(!line.empty()&&renderer.getTextWidth(toybox::kUiFontId,(line+" "+word).c_str())>430)){lines.push_back(line);line.clear();}if(!line.empty())line+=' ';line+=word;word.clear();};for(char c:text){if(c==' '||c=='\n'||c=='\r'){flush();if(c=='\n'&&!line.empty()){lines.push_back(line);line.clear();}}else word+=c;}flush();if(!line.empty())lines.push_back(line);}
void ZenDictionaryActivity::lookup(int n){if(n<0||n>=count)return;std::string text;if(!store.definition(entries[n],text)){status="Không đọc được dữ liệu trên thẻ.";requestUpdate();return;}heading=entries[n].word;wrap(text);page=0;detail=true;keyboardOpen=false;requestUpdate();}
void ZenDictionaryActivity::loop(){RenderLock lock;using Button=MappedInputManager::Button;
 if(pending){pending=false;if(opened&&!store.suggestions(query,entries,count)){status="Lỗi đọc dữ liệu. Kiểm tra thẻ nhớ.";count=0;}else if(opened&&!query.empty()&&!count)status="Không tìm thấy từ phù hợp.";else if(opened)status.clear();requestUpdate();}
 if(mappedInput.wasReleased(Button::Back)){if(detail){detail=false;requestUpdate();}else if(keyboardOpen){keyboardOpen=false;previewKey=0;requestUpdate();}else activityManager.goHome();return;}
 int sx=0,sy=0;auto swipe=mappedInput.wasSwipe(sx,sy);if(detail){int step=mappedInput.wasReleased(Button::NavNext)||swipe==MappedInputManager::SwipeDir::Left?1:mappedInput.wasReleased(Button::NavPrevious)||swipe==MappedInputManager::SwipeDir::Right?-1:0;if(step){page=std::clamp(page+step,0,std::max(0,(int(lines.size())-1)/14));requestUpdate();}}
 int x=0,y=0;
 if(keyboardOpen && !detail){
   int held=0;
   if(mappedInput.isScreenTouchHeld(x,y)){
     const if(!keyboardOpen)return;
 if(y>=479&&y<517&&x>=280){keyboardOpen=false;previewKey=0;requestUpdate();return;}
 int key=zendictionary::keyAt(x,y);
     if(key>='a'&&key<='z') held=key;
   }
   if(previewKey!=held){previewKey=held;requestUpdate();}
 }
 if(!mappedInput.wasScreenTapped(x,y))return;
 if(y>=753){if(x<195){if(detail){detail=false;requestUpdate();}else activityManager.goHome();}else if(detail){page=(page+1)%std::max(1,(int(lines.size())+13)/14);requestUpdate();}return;}
 if(y>=78&&y<116){bool next=x>=240;if(vi!=next){vi=next;telex=vi;raw.clear();opened=store.open(vi);changed();}return;}
 if(y>=130&&y<186){if(x>415)raw.clear();keyboardOpen=true;changed();return;}
 if(detail)return;
 if(y>=236&&y<(keyboardOpen?464:692)){int n=(y-236)/76;if(n<count)lookup(n);return;}
 if(!keyboardOpen)return;
 if(y>=479&&y<517&&x>=280){keyboardOpen=false;previewKey=0;requestUpdate();return;}
 int key=zendictionary::keyAt(x,y);if(key==8){if(!raw.empty())raw.pop_back();changed();}else if(key==127){raw.clear();changed();}else if(key==256){telex=!telex;changed();}else if(key==13){lookup(0);}else if(key>0&&key<128&&raw.size()<80){raw+=char(key);changed();}
}
void ZenDictionaryActivity::render(RenderLock&&){renderer.clearScreen();auto target=toybox::makeTarget(renderer);pixelui::bindFonts(target,renderer,true);zendictionary::Ui ui{target};ui.header(vi,query.c_str());
 if(detail){ui.txt(22,207,436,40,heading.c_str(),2);for(int n=0;n<14&&page*14+n<int(lines.size());++n)ui.txt(22,260+n*33,436,32,lines[page*14+n].c_str(),1);}
 else {ui.txt(22,204,436,26,status.empty()?(query.empty()?"Nhập từ để bắt đầu tra offline":"GỢI Ý TỪ"):status.c_str(),0);for(int n=0;n<std::min(count,keyboardOpen?3:6);++n){int y=240+n*76;ui.txt(28,y,398,28,entries[n].word.c_str(),1);ui.txt(28,y+29,398,25,entries[n].preview.c_str(),0);ui.line(28,y+63,452,y+63);}if(keyboardOpen)ui.keyboard(telex,previewKey);}
 char clock[16]="--:--",position[32];halClock.formatTime(clock,sizeof(clock),false);std::snprintf(position,sizeof(position),"%d / %d",page+1,std::max(1,(int(lines.size())+13)/14));ui.footer(clock,detail?position:"Offline");renderer.displayBuffer(HalDisplay::FAST_REFRESH);
}
