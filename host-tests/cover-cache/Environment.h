#pragma once
#include <GfxRenderer.h>
#include <HalStorage.h>
#include <cstdio>
#include <string>
struct RecentBook{std::string path,title,coverBmpPath;};
struct SettingsStub{bool xtcCoverRotation=false;};inline SettingsStub SETTINGS;
struct Rect{int x,y,width,height;};
namespace UITheme{inline std::string getCoverThumbPath(std::string s,int h){auto p=s.find("[HEIGHT]");if(p!=std::string::npos)s.replace(p,8,std::to_string(h));return s;}}
struct GuiStub{int placeholders=0;void drawCoverPlaceholder(GfxRenderer&,Rect){++placeholders;}};inline GuiStub GUI;
namespace homecover{inline void recordSource(const char*,HalFile&,bool){}}
