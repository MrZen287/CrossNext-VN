#pragma once
#include <cstdint>
namespace hometheme {
inline constexpr unsigned zodiacCount = 12;
inline constexpr unsigned nextZodiac(unsigned index) { return (index + 1) % zodiacCount; }
// 0: original cat, 1: cloud dragon, 2: coiled dragon. Choose only on Home entry.
inline unsigned choose(uint32_t random){unsigned roll=random%10;return roll<4?0:roll<8?2:1;}
}
