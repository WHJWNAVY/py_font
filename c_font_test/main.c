#include <stdio.h>
#include "type.h"
#include "gdi.h"

//#define CONFIG_HZK16_PATH       /*"F:\\My_Program\\c_program\\led_scroll\\*/"HZK16F"

int main(void)
{

    gdi_init();
    gdi_hzk_puts(0, 0, "我的祖国\r\n繁荣昌盛", 0, 1);
    // gdi_hzk_puts(0, 0, "Hello\r\nWorld!", 0, 1);
    gdi_update();
    gdi_uninit();
    getchar();
    return 0;
}
