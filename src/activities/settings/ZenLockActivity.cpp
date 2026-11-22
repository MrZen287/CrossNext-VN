#include "ZenLockActivity.h"
#include <I18n.h>
#include <Memory.h>
#include <cstring>
#include "CrossPointSettings.h"
#include "activities/home/FileBrowserActivity.h"
#include "apps_local/ui/PixelMenuTheme.h"
#include "components/UITheme.h"
const char* ZenLockActivity::headerTitle() const {return I18N.getLanguage()==Language::VI?"Màn hình khóa":"Lock screen";}
void ZenLockActivity::buildScreen(UiScreen& screen) {
  pixelui::FontScope fonts(uiTarget,renderer,I18N.getLanguage()!=Language::EN);
  useCompactSettingsRows();pixelui::begin(screen,0);
  const bool vi=I18N.getLanguage()==Language::VI;
  rows[0].label=vi?"Lịch":"Calendar";rows[1].label=vi?"Ảnh từ thư mục":"Image from folder";
  rows[2].label=vi?"Chọn ảnh":"Choose image";
  rows[2].subtitle=SETTINGS.zenLockImagePath[0]?SETTINGS.zenLockImagePath:(vi?"Chưa chọn ảnh PNG/BMP":"No PNG/BMP image selected");
  for(int n=0;n<3;++n)rows[n].actionValue=n;
  GUI.setCheckboxRow(rows[0],!SETTINGS.zenLockImage);GUI.setCheckboxRow(rows[1],SETTINGS.zenLockImage);
  freeink::ui::ListProps props;props.items=rows;props.count=3;props.action=ACTION_ROW;props.inputMask=freeink::ui::InputTouch;
  props.labelText=screen.theme().smallText;props.labelText.maxLines=2;syncListViewport(screen,props);pixelui::list(screen,props);
}
void ZenLockActivity::activateIndex(int index) {
  if(index==0){SETTINGS.zenLockImage=0;SETTINGS.saveToFile();requestUpdate();return;}
  if(index==1&&SETTINGS.zenLockImagePath[0]){SETTINGS.zenLockImage=1;SETTINGS.saveToFile();requestUpdate();return;}
  auto picker=makeUniqueNoThrow<FileBrowserActivity>(renderer,mappedInput,"/",FileBrowserActivity::Mode::PickImage);
  if(!picker)return;
  startActivityForResult(std::move(picker),[this](const ActivityResult& result){
    if(result.isCancelled)return;
    const auto& path=std::get<FilePathResult>(result.data).path;
    if(path.size()>=sizeof(SETTINGS.zenLockImagePath))return;
    std::strcpy(SETTINGS.zenLockImagePath,path.c_str());SETTINGS.zenLockImage=1;SETTINGS.saveToFile();requestUpdate();
  });
}
