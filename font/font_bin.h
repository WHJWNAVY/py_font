#ifndef __FONT_BIN_H__
#define __FONT_BIN_H__

#include <stdint.h>

typedef struct font_bin_s
{
    int32_t     flag;           //file flag,  "FONT"
    int32_t     width;          //font width,  64
    int32_t     height;         //font height, 64
    int32_t     start;          //font start,  0x4E00
    int32_t     end;            //font end,    0x9FA5
    int32_t     mode;           //font mode,   0
} font_bin_t;

#endif