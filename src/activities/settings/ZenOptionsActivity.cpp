#include "util/ReadingGuides.h"
#include "ZenOptionsActivity.h"
#include "CrossPointSettings.h"
#include "apps_local/ui/PixelMenuTheme.h"
namespace {
constexpr const char* widgets[]={"Ngày và lịch âm","Đồng hồ","Thống kê đọc","Lịch tháng"};
constexpr const char* edges[]={"Tắt","Độ sáng","Độ ấm màu"};
constexpr const char* slots[]={"Widget trái · trang đầu","Widget phải · trang đầu","Widget rộng · trang bên kia"};
}
const char* ZenOptionsActivity::headerTitle()const{return kind==Kind::Guides?"Dòng kẻ khi đọc":kind==Kind::Widgets?"Đổi widget":"Vuốt mép khi đọc";}
uint8_t& ZenOptionsActivity::value(int n){
 if(kind==Kind::Edges)return n?SETTINGS.rightEdgeLightAction:SETTINGS.leftEdgeLightAction;
 return n==0?SETTINGS.zenWidgetLeft:n==1?SETTINGS.zenWidgetRight:SETTINGS.zenWidgetWide;
}
void ZenOptionsActivity::buildScreen(UiScreen& screen){
 pixelui::FontScope fonts(uiTarget,renderer,true);useCompactSettingsRows();pixelui::begin(screen,0);
 const int count=listCount();
 for(int n=0;n<count;++n){
  rows[n]={};rows[n].actionValue=n;
  if(kind==Kind::Guides){
    constexpr const char* labels[]={"Hiển thị dòng kẻ","Chấm mảnh · · · ·","Chấm đậm • • • •","Gạch ngắn – – –","Gạch chấm – · – ·","Nét liền mảnh ───"};
    rows[n].label=labels[n];GUI.setCheckboxRow(rows[n],n?SETTINGS.zenGuideStyle==n-1:SETTINGS.zenReadingGuides);
  }else if(slot>=0){rows[n].label=kind==Kind::Widgets?widgets[n]:edges[n];GUI.setCheckboxRow(rows[n],value(slot)==n);}
  else{rows[n].label=kind==Kind::Widgets?slots[n]:n?"Mép phải · vuốt lên/xuống":"Mép trái · vuốt lên/xuống";
   auto v=value(n);rows[n].value=kind==Kind::Widgets?widgets[std::min<int>(v,3)]:edges[std::min<int>(v,2)];}
  if(failed&&n==0)rows[n].subtitle="Không lưu được, hãy thử lại";
 }
 freeink::ui::ListProps props;props.items=rows;props.count=count;props.action=ACTION_ROW;props.inputMask=freeink::ui::InputTouch;
 props.labelText.maxLines=2;syncListViewport(screen,props);pixelui::list(screen,props);
}
void ZenOptionsActivity::activateIndex(int n){
 RenderLock lock;
 if(n<0||n>=listCount())return;
 if(kind==Kind::Guides){
  const auto enabled=SETTINGS.zenReadingGuides,style=SETTINGS.zenGuideStyle;
  if(n==0)SETTINGS.zenReadingGuides=!enabled;else SETTINGS.zenGuideStyle=n-1;
  failed=!SETTINGS.saveToFile();if(failed){SETTINGS.zenReadingGuides=enabled;SETTINGS.zenGuideStyle=style;}readingguides::enabled=SETTINGS.zenReadingGuides;readingguides::style=SETTINGS.zenGuideStyle;requestUpdate();return;
 }
 if(slot<0){slot=n;activeNav().reset();requestUpdate();return;}
 auto& field=value(slot);const auto before=field;field=n;
 failed=!SETTINGS.saveToFile();if(failed)field=before;else slot=-1;
 activeNav().reset();requestUpdate();
}
void ZenOptionsActivity::onBackButton(){RenderLock lock;if(slot>=0){slot=-1;failed=false;activeNav().reset();requestUpdate();}else finish();}
