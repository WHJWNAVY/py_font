#ifndef __LCD_GDI_H_
#define __LCD_GDI_H_

#include "type.h"

// #define CONFIG_USE_ROLL       1//使用滚屏功能
#define CONFIG_USE_PRTLOG       1//使用写入到文件功能
#define CONFIG_HZK16_PATH       "simsun.ttc-16.bin"
#define CONFIG_HZK64_PATH       "HZK64"
#define CONFIG_PRTLOG_PATH      "Screen.log"

// Size
#define	SCREEN_X     64
#define	SCREEN_Y     32
extern void   gdi_update(void);
extern void   gdi_get_screen_size(int32 *x, int32 *y);
extern void   gdi_set_orientation(int32 orientation);
extern void   gdi_set_point(int32  x, int32  y, int32 colour);
extern int32  gdi_get_point(int32 x, int32 y);
extern void   gdi_line(int32 x0, int32 y0, int32 x1, int32 y1, int32 colour);
extern void   gdi_lineto(int32  x, int32  y, int32 colour);
extern void   gdi_rectangle(int32 x1, int32 y1, int32 x2, int32 y2, int32 colour, int32 filled);
extern void   gdi_circle(int32  x, int32  y, int32  r, int32 colour, int32 filled);
extern void   gdi_ellipse(int32 cx, int32 cy, int32 xRadius, int32 yRadius, int32 colour, int32 filled);
extern void   gdi_putc(int32  x, int32  y, int32 c, int32 bgCol, int32 fgCol);
extern void   gdi_puts(int32  x, int32  y, const int8 *str, int32 bgCol, int32 fgCol);
extern void   lcd128x64putnum(int32  x, int32  y, int32 num, int32 bgCol, int32 fgCol);
extern void   gdi_putbmp(int32 x0, int32 y0, int32 with, int32 height, uint8* bmp);
extern void   gdi_clear(int32 colour);
extern int32  gdi_init(void);
extern int32  gdi_uninit(void);

extern void   gdi_hzk_putc(int32 x, int32 y, const int8 *hz, int32 bgCol, int32 fgCol);
extern void   gdi_hzk_puts(int32 x, int32 y, const int8 *str, int32 bgCol, int32 fgCol);

#endif
