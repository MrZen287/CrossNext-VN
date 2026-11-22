#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
using std::min;
inline uint32_t fakeMillis=0;
inline uint32_t millis(){return fakeMillis;}
