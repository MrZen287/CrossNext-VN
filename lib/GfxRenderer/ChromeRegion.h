#pragma once
// The render task records a logical full-width header band for output-polarity compensation.
namespace chrome {
inline int headerHeight = 0;
inline bool lightHeader = true;
inline bool framebufferLightHeader = false;
inline void beginFrame() {
  headerHeight = 0;
  framebufferLightHeader = lightHeader;
}
inline bool invertHeaderForOutput(bool invertedDisplay) { (void)invertedDisplay; return false; }
}  // namespace chrome
