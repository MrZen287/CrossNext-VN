#include "BookCoverPainter.h"

#include <FsHelpers.h>
#include <GfxRenderer.h>
#include <Logging.h>
#include <Memory.h>

#include <algorithm>
#include <functional>
#include <optional>

#include "../../CrossPointSettings.h"
#include "../../RecentBooksStore.h"
#include "../../components/UITheme.h"
#include "CoverBoxSampler.h"
#include "CoverScanline.h"
#include "CoverSceneCache.h"
namespace fui = freeink::ui;
bool BookCoverPainter::paintBitmap(fui::Rect slot, int taper,bool stretch) {
  const int width = coverBitmap.getWidth(), height = coverBitmap.getHeight();
  if (width <= 0 || height <= 0 || slot.empty()) return false;
  int cropWidth = width, cropHeight = height;
  if (!stretch && !taper && int64_t(width) * slot.height > int64_t(height) * slot.width)
    cropWidth = std::max(1, int(int64_t(height) * slot.width / slot.height));
  else if (!stretch && !taper)
    cropHeight = std::max(1, int(int64_t(width) * slot.height / slot.width));
  const int sourceX = (width - cropWidth) / 2, sourceY = (height - cropHeight) / 2;
  const bool packed = coverBitmap.is1Bit();
  bool filtered = packed && cropWidth >= slot.width && cropHeight >= slot.height &&
                  (taper || cropWidth != slot.width || cropHeight != slot.height);
  const size_t decoded = packed ? 0 : (width + 3) / 4;
  const size_t columnsBytes = filtered ? slot.width * sizeof(homecover::BoxColumn) : 0;
  const size_t bandsBytes =
      filtered ? homecover::BoxSampler::bandCapacity(slot, taper) * sizeof(homecover::BoxBand) : 0;
  size_t filterBytes = filtered ? columnsBytes + bandsBytes + ((width + 7) / 8) * sizeof(uint16_t) : 0;
  constexpr size_t READ_AHEAD_BYTES = 4096;
  const size_t needed = filterBytes + decoded + coverBitmap.getRowBytes() + READ_AHEAD_BYTES;
  // One reusable row allocation: packed sampling scratch replaces 2bpp expansion.
  const auto reserve = [this](size_t bytes) {
    if (bytes <= rowCapacity) return true;
    auto next = makeUniqueNoThrow<uint8_t[]>(bytes);
    if (!next) {
      LOG_ERR("HOME", "OOM: cover row (%u bytes)", unsigned(bytes));
      return false;
    }
    row = std::move(next);
    rowCapacity = bytes;
    return true;
  };
  if (!reserve(needed)) {
    if (!filtered) return false;
    filtered = false;
    filterBytes = 0;
    if (!reserve(decoded + coverBitmap.getRowBytes() + READ_AHEAD_BYTES)) return false;
  }
  auto* source = row.get() + filterBytes + decoded;
  coverBitmap.setReadAheadBuffer(source + coverBitmap.getRowBytes(), READ_AHEAD_BYTES);
  std::optional<homecover::BoxSampler> sampler;
  if (filtered) {
    sampler.emplace(reinterpret_cast<homecover::BoxColumn*>(row.get()),
                    reinterpret_cast<homecover::BoxBand*>(row.get() + columnsBytes),
                    reinterpret_cast<uint16_t*>(row.get() + columnsBytes + bandsBytes), width, sourceX, cropWidth,
                    cropHeight, slot, taper);
  }
  const auto fill=[this](int x,int y,int w,int h,bool black){renderer.fillRect(x,y,w,h,black);};
  const auto draw = [this,slot,fill](int x,int y,int w,int h){
    if(warped)face.mapRect(slot,x,y,w,h,[fill](int a,int b,int c,int d){fill(a,b,c,d,true);});else fill(x,y,w,h,true);
  };
  if(warped)face.mapRect(slot,slot.x,slot.y,slot.width,slot.height,[fill](int a,int b,int c,int d){fill(a,b,c,d,false);});
  else renderer.fillRect(slot.x,slot.y,slot.width,slot.height,false);
  for (int line = 0; line < height; ++line) {
    const auto error = packed ? coverBitmap.readNext1BitRow(source) : coverBitmap.readNextRow(row.get(), source);
    if (error != BmpReaderError::Ok) return false;
    const int sourceLine = (coverBitmap.isTopDown() ? line : height - 1 - line) - sourceY;
    if (sourceLine < 0 || sourceLine >= cropHeight) continue;
    if (sampler)
      sampler->paint(source, sourceLine, coverBitmap.isTopDown(), draw);
    else if (packed)
      homecover::paint1BitRow(source, sourceX, cropWidth, cropHeight, sourceLine, slot, taper, draw);
    else
      homecover::paintRow(row.get(), sourceX, cropWidth, cropHeight, sourceLine, slot, taper, draw);
  }
  renderer.preserveImagePolarity(slot.x, slot.y, slot.width, slot.height);
  return true;
}
void BookCoverPainter::paint(const RecentBook& book, fui::Rect slot, int taper, Fit fit) {
  if(slot.empty())return;
  // Every path (bitmap, fallback, frame) must obey the caller's book face.
  const auto before=renderer.getClipRect();
  struct ClipRestore {GfxRenderer& r;std::array<int,4> clip;~ClipRestore(){r.setClipRect(clip[0],clip[1],clip[2],clip[3]);}} restore{renderer,before};
  const int left=std::max(before[0],int(slot.x)),top=std::max(before[1],int(slot.y));
  const int right=std::min(before[0]+before[2],int(slot.right())),bottom=std::min(before[1]+before[3],int(slot.bottom()));
  if(right<=left||bottom<=top)return;
  renderer.setClipRect(left,top,right-left,bottom-top);

  // Cache identity follows preparation, independently of the destination plane height.
  const int cacheHeight=preparedHeight>0?preparedHeight:slot.height;
  bool drawn = false;
  char thumb[112]{};
  const bool xtc = FsHelpers::hasXtcExtension(book.path);
  if (FsHelpers::hasEpubExtension(book.path) || FsHelpers::hasXtcExtension(book.path)) {
    std::snprintf(thumb, sizeof(thumb), "/.crosspoint/%s_%lu/thumb_%s%d.bmp", xtc ? "xtc" : "epub",
                  static_cast<unsigned long>(std::hash<std::string>{}(book.path)),
                  xtc && SETTINGS.xtcCoverRotation ? "ccw3_" : "", cacheHeight);
  }
  std::string fallback;
  const char* path = thumb;
  if (!*thumb || !Storage.exists(thumb)) {
    if (xtc) {
      std::snprintf(thumb, sizeof(thumb), "/.crosspoint/xtc_%lu/cover%s.bmp",
                    static_cast<unsigned long>(std::hash<std::string>{}(book.path)),
                    SETTINGS.xtcCoverRotation ? "_ccw3" : "");
      fallback = UITheme::getCoverThumbPath(thumb, cacheHeight);
    } else
      fallback = UITheme::getCoverThumbPath(book.coverBmpPath, cacheHeight);
    path = fallback.c_str();
  }
  if (*path && Storage.openFileForRead("HOME", path, coverFile)) {
    if (coverBitmap.parseHeaders() == BmpReaderError::Ok) {
      const int w = coverBitmap.getWidth(), h = coverBitmap.getHeight();
      if (fit == Fit::Contain && w > 0 && h > 0) {
        int width = slot.width, height = slot.height;
        if (int64_t(w) * height > int64_t(h) * width)
          height = std::max(1, int(int64_t(h) * width / w));
        else
          width = std::max(1, int(int64_t(w) * height / h));
        slot = fui::makeRect(slot.x + (slot.width - width) / 2, slot.y + (slot.height - height) / 2, width, height);
      }
      drawn = paintBitmap(slot, taper,fit==Fit::Stretch);
    }
    homecover::recordSource(path, coverFile, drawn);
    coverFile.close();
  } else {
    homecover::recordSource(path, coverFile, false);
  }
  lastBounds = slot;
  if(!drawn && warped){
    const auto white=fui::Paint::solid(fui::Color::White),ink=fui::Paint::solid(fui::Color::Black);
    face.mapRect(slot,slot.x,slot.y,slot.width,slot.height,[&](int x,int y,int w,int h){target.fill(fui::makeRect(x,y,w,h),white);});
    const auto inset=slot.inset({12,22,12,22});
    fui::TextStyle style;style.font=0;style.maxLines=3;style.align=fui::TextAlign::Center;
    target.text(inset,book.title.c_str(),style);
    (void)ink;
  }else if (!drawn) GUI.drawCoverPlaceholder(renderer, Rect{slot.x, slot.y, slot.width, slot.height});
  if(warped){
    const auto ink=fui::Paint::solid(fui::Color::Black);
    target.line({int16_t(face.left),int16_t(face.topLeft)},{int16_t(face.right),int16_t(face.topRight)},1,ink);
    target.line({int16_t(face.left),int16_t(face.bottomLeft)},{int16_t(face.right),int16_t(face.bottomRight)},1,ink);
    return;
  }
  if (!taper) {
    renderer.drawRect(slot.x, slot.y, slot.width, slot.height);
    return;
  }
  homecover::finishTaper(
      slot, taper,
      [this](int x, int y, int w, int h) {
        target.fill(fui::makeRect(x, y, w, h), fui::Paint::solid(fui::Color::White));
      },
      [this](int x, int y, int w, int h) {
        target.fill(fui::makeRect(x, y, w, h), fui::Paint::solid(fui::Color::Black));
      });
}

void BookCoverPainter::paintFace(const RecentBook& book,const homecover::CoverFace& plane,int sourceHeight){
 if(!plane.valid())return;
 preparedHeight=sourceHeight;warped=true;face=plane;paint(book,plane.bounds(),0,Fit::Stretch);warped=false;preparedHeight=0;
}
