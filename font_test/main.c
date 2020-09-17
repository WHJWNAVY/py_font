#include <stdio.h>
#include "type.h"
#include "lcd_gdi.h"

//#define CONFIG_HZK16_PATH       /*"F:\\My_Program\\c_program\\led_scroll\\*/"HZK16F"

int main(void)
{

    gdi_init();
    gdi_hzk16_puts(0, 0, "ÎÒµÄ×æ¹ú\r\n·±ÈÙ²ýÊ¢", 0, 1);
    gdi_update();
    gdi_uninit();
    getchar();
    return 0;
}
