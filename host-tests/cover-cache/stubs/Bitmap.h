#pragma once
#include "HalStorage.h"
#include <cstdint>
enum class BmpReaderError{Ok,Invalid};
class Bitmap{HalFile&f;public:explicit Bitmap(HalFile&f):f(f){}int getWidth(){return 8;}int getHeight(){return 8;}bool is1Bit(){return true;}bool isTopDown(){return true;}int getRowBytes(){return 1;}void setReadAheadBuffer(uint8_t*,int){}BmpReaderError parseHeaders(){return f.opened?BmpReaderError::Ok:BmpReaderError::Invalid;}BmpReaderError readNext1BitRow(uint8_t*p){*p=0;return BmpReaderError::Ok;}BmpReaderError readNextRow(uint8_t*p,uint8_t*q){*p=*q=0;return BmpReaderError::Ok;}};
