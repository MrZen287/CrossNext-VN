#include "Environment.h"
#include "BookCoverPainter.h"
#include "ReadingDesk.h"
#include <cassert>
#include <vector>
std::string path(const RecentBook&book,int h){char s[160];std::snprintf(s,sizeof(s),"/.crosspoint/epub_%lu/thumb_%d.bmp",static_cast<unsigned long>(std::hash<std::string>{}(book.path)),h);return s;}
int main(){using namespace readingdesk;std::vector<uint8_t>pixels(48000,255);freeink::ui::DisplayTarget target(pixels.data(),480,800,60);GfxRenderer r{target};BookCoverPainter painter(r,target);auto device=target.deviceContext();device.safeArea={2,2,2,2};auto layout=screenLayout(device,3,true,true,true,false);RecentBook book{"/books/test.epub","Test","/.crosspoint/epub/test/thumb_[HEIGHT].bmp"};
 // Reproduce the prior lookup miss: only a 347px thumbnail exists, not a 365px one.
 Storage.files.insert(path(book,layout.cover.height));assert(layout.cover.height==347);assert(homecover::mainFace.bounds().height==365);
 painter.paintFace(book,homecover::mainFace,365);assert(r.writes==0);
 Storage.requests.clear();painter.paintFace(book,homecover::mainFace,layout.cover.height);assert(r.writes>0);assert(Storage.requests.back()==path(book,347));assert((r.clip==std::array<int,4>{0,0,480,800}));
 for(int n=0;n<3;++n){int height=layout.hits[Recent1+n].height;assert(height==117+n);assert(homecover::shelfFaces[n].bounds().height==118+n);Storage.files.insert(path(book,height));Storage.requests.clear();int before=r.writes;painter.paintFace(book,homecover::shelfFaces[n],height);assert(r.writes>before);assert(Storage.requests.back()==path(book,height));}
 // Ordinary library rendering must retain its own height-based cache key.
 Storage.files.insert(path(book,200));painter.paint(book,freeink::ui::makeRect(0,0,100,200));assert(Storage.requests.back()==path(book,200));
 puts("Actual BookCoverPainter: reproduced old cache miss; main/shelf covers use prepared images and restore clipping");}
