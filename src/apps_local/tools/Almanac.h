#pragma once
#include <cstddef>
#include <cstdint>
#include "VietnameseLunar.h"
namespace almanac {
struct Day { uint32_t yi[4],ji[4];uint8_t term,officer; };
const Day* find(int civilDay);
const char* activity(unsigned index);
unsigned activityCount();
int activityAt(const uint32_t* mask,unsigned ordinal);
unsigned activityTotal(const uint32_t* mask);
const char* solarTerm(uint8_t index);
const char* officer(uint8_t index);
const char* stem(unsigned index);
const char* branch(unsigned index);
const char* daySpirit(int civilDay,vietnameselunar::Date lunar);
bool auspiciousDay(int civilDay,vietnameselunar::Date lunar);
bool auspiciousHour(int civilDay,int hourBranch);
void canChiDay(int civilDay,char* out,size_t size);
void canChiMonth(vietnameselunar::Date lunar,char* out,size_t size);
void canChiYear(int year,char* out,size_t size);
const char* avoidedDay(vietnameselunar::Date lunar);
}
