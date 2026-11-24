#include "Almanac.h"
#include <cstdio>
#include <iterator>
#include "../reading/ReadingStatsCore.h"
namespace almanac {
namespace {
constexpr Day days[]={
#include "AlmanacDays.inc"
};
static_assert(std::size(days)==1096);
constexpr const char* activities[]={
#include "AlmanacActivities.inc"
};
constexpr const char* stems[]={"Giáp","Ất","Bính","Đinh","Mậu","Kỷ","Canh","Tân","Nhâm","Quý"};
constexpr const char* branches[]={"Tý","Sửu","Dần","Mão","Thìn","Tỵ","Ngọ","Mùi","Thân","Dậu","Tuất","Hợi"};
constexpr const char* spirits[]={"Thanh Long","Minh Đường","Thiên Hình","Chu Tước","Kim Quỹ","Kim Đường","Bạch Hổ","Ngọc Đường","Thiên Lao","Nguyên Vũ","Tư Mệnh","Câu Trần"};
constexpr const char* terms[]={"Đông chí","Tiểu hàn","Đại hàn","Lập xuân","Vũ thủy","Kinh trập","Xuân phân","Thanh minh","Cốc vũ","Lập hạ","Tiểu mãn","Mang chủng","Hạ chí","Tiểu thử","Đại thử","Lập thu","Xử thử","Bạch lộ","Thu phân","Hàn lộ","Sương giáng","Lập đông","Tiểu tuyết","Đại tuyết"};
constexpr const char* officers[]={"Kiến","Trừ","Mãn","Bình","Định","Chấp","Phá","Nguy","Thành","Thu","Khai","Bế"};
int mod(int n,int d){const int v=n%d;return v<0?v+d:v;}
bool good(int index){return index==0||index==1||index==4||index==5||index==7||index==10;}
int spiritIndex(int day,vietnameselunar::Date l){return mod(day+5-(l.month-1)*2,12);}
}
const Day* find(int day){const int at=day-readingstats::dayIndex({2026,1,1});return at>=0&&at<int(std::size(days))?days+at:nullptr;}
const char* activity(unsigned index){return index<std::size(activities)?activities[index]:"";}
unsigned activityCount(){return std::size(activities);}
int activityAt(const uint32_t* mask,unsigned ordinal){for(unsigned i=0;i<std::size(activities);++i)if(mask[i/32]&(uint32_t{1}<<(i%32))){if(!ordinal)return i;--ordinal;}return -1;}
unsigned activityTotal(const uint32_t* mask){unsigned n=0;for(unsigned i=0;i<std::size(activities);++i)n+=(mask[i/32]>>(i%32))&1;return n;}
const char* solarTerm(uint8_t i){return i<std::size(terms)?terms[i]:"—";}
const char* officer(uint8_t i){return i<std::size(officers)?officers[i]:"—";}
const char* stem(unsigned i){return stems[i%10];}
const char* branch(unsigned i){return branches[i%12];}
const char* daySpirit(int d,vietnameselunar::Date l){return spirits[spiritIndex(d,l)];}
bool auspiciousDay(int d,vietnameselunar::Date l){return good(spiritIndex(d,l));}
bool auspiciousHour(int d,int h){constexpr int offset[]={4,2,0,10,8,6,4,2,0,10,8,6};return good(mod(h+offset[mod(d+5,12)],12));}
void canChiDay(int d,char* out,size_t size){std::snprintf(out,size,"%s %s",stems[mod(d+7,10)],branches[mod(d+5,12)]);}
void canChiMonth(vietnameselunar::Date l,char* out,size_t size){std::snprintf(out,size,"%s %s",stems[mod(l.year*12+l.month+3,10)],branches[mod(l.month+1,12)]);}
void canChiYear(int y,char* out,size_t size){std::snprintf(out,size,"%s %s",stems[mod(y+6,10)],branches[mod(y+8,12)]);}
const char* avoidedDay(vietnameselunar::Date l){if(l.day==5||l.day==14||l.day==23)return "Nguyệt kỵ";if(l.day==3||l.day==7||l.day==13||l.day==18||l.day==22||l.day==27)return "Tam nương";return "Không thuộc Tam nương / Nguyệt kỵ";}
}
