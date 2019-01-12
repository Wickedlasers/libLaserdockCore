#ifndef LASERDOCKLIB_LASERDOCKSAMPLE_H
#define LASERDOCKLIB_LASERDOCKSAMPLE_H

#include <laserdocklib/Laserdocklib.h>

#include <stdint.h>

struct LASERDOCKLIB_EXPORT LaserdockSample
{
    uint16_t rg;      //lower byte is red, top byte is green
    uint16_t b;       //lower byte is blue
    uint16_t x;		  // 0..4095
    uint16_t y;       // 0..4095
};

#endif //LASERDOCKLIB_LASERDOCKSAMPLE_H
