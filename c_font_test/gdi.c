#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "font.h"
#include "gdi.h"

// Software copy of the framebuffer
static uint8 SCREEN_BUFF[SCREEN_Y][SCREEN_X];
#ifdef CONFIG_USE_ROLL
static uint8 SCREEN_BUFF_ROLL[SCREEN_Y][SCREEN_X];
static uint8 rollON = FALSE;
#endif

static int32 maxX = SCREEN_X, maxY = SCREEN_Y;
static int32 lastX = 0, lastY = 0;
static int32 mirrorX = 0, mirrorY = 0;

typedef struct
{
    uint32 flag;   // file flag, "FONT"
    uint32 width;  // font width, 64
    uint32 height; // font height, 64
    uint32 start;  // font start, 0x4E00
    uint32 end;    // font end, 0x9FA5
    uint32 mode;   // font mode, 1
} hzk_t;

#define HZK_BUF_MAX (64 * (64 / 8))
static FILE *fpHZK = NULL;
static hzk_t HZK = {0};

#ifdef CONFIG_USE_PRTLOG
static FILE *fpPRTLOG = NULL;
#endif

typedef enum _gdi_col
{
    COL_BLACK = '@',
    COL_WHITE = ' '
} GDI_COL;

/*
 * gdi_update:
 *	Copy our software version to the real display
 *******************************************************************************
 */
void gdi_update(void)
{
    int8 x = 0, y = 0;
    system("cls");
#ifdef CONFIG_USE_PRTLOG
    fseek(fpPRTLOG, 0, SEEK_SET);
#endif
    for (y = 0; y < (SCREEN_Y); y++)
    {
        for (x = 0; x < (SCREEN_X); x++)
        {
#ifdef CONFIG_USE_ROLL
            if (rollON != TRUE)
            {
                putchar(SCREEN_BUFF[y][x]);
#ifdef CONFIG_USE_PRTLOG
                fprintf(fpPRTLOG, "%c", SCREEN_BUFF[y][x]);
#endif
            }
            else
            {
                putchar(SCREEN_BUFF_ROLL[y][x]);
#ifdef CONFIG_USE_PRTLOG
                fprintf(fpPRTLOG, "%c", SCREEN_BUFF_ROLL[y][x]);
#endif
            }
#else
            putchar(SCREEN_BUFF[y][x]);
#ifdef CONFIG_USE_PRTLOG
            fprintf(fpPRTLOG, "%c", SCREEN_BUFF[y][x]);
#endif
#endif
        }
        putchar('+');
        putchar('\n');
#ifdef CONFIG_USE_PRTLOG
        fprintf(fpPRTLOG, "+\n");
#endif
    }
    for (y = 0; y <= (SCREEN_X); y++)
    {
        putchar('+');
#ifdef CONFIG_USE_PRTLOG
        fprintf(fpPRTLOG, "+");
#endif
    }
    putchar('\n');
#ifdef CONFIG_USE_PRTLOG
    fprintf(fpPRTLOG, "\n");
#endif
}

/*
 * gdi_clear:
 *	Clear the display to the given colour.
 *******************************************************************************
 */
void gdi_clear(int32 colour)
{
    int32 x = 0, y = 0;
    int8 col = 0;

    col = (colour) ? (COL_BLACK) : (COL_WHITE);

    for (y = 0; y < (SCREEN_Y); y++)
    {
        for (x = 0; x < (SCREEN_X); x++)
        {
            SCREEN_BUFF[y][x] = col;
        }
    }
}

/*
 * gdi_set_orientation:
 *	Set the display orientation:
 *	0: Normal, the display is portrait mode, 0,0 is top left
 *	1: Mirror x
 *	2: Mirror y
 *	3: Mirror x and y
 *******************************************************************************
 */
void gdi_set_orientation(int32 orientation)
{
    switch (orientation)
    {
    case 0:
        mirrorX = 0;
        mirrorY = 0;
        break;

    case 1:
        mirrorX = 1;
        mirrorY = 0;
        break;

    case 2:
        mirrorX = 0;
        mirrorY = 1;
        break;

    case 3:
        mirrorX = 1;
        mirrorY = 1;
        break;

    default:
        break;
    }
}

/*
 * gdi_get_screen_size:
 *	Return the max X & Y screen sizes. Needs to be called again, if you
 *	change screen orientation.
 *******************************************************************************
 */
void gdi_get_screen_size(int32 *x, int32 *y)
{
    *x = maxX;
    *y = maxY;
}

/*
 *******************************************************************************
 * Standard Graphical Functions
 *******************************************************************************
 */

/*
 * gdi_set_point:
 *	Plot a pixel.
 *******************************************************************************
 */
void gdi_set_point(int32 x, int32 y, int32 colour)
{
    if (mirrorX)
        x = (maxX - x - 1);

    if (mirrorY)
        y = (maxY - y - 1);

    lastX = x;
    lastY = y;

    x = ((x < 0) ? 0 : ((x > (maxX - 1)) ? (maxX - 1) : x));
    y = ((y < 0) ? 0 : ((y > (maxY - 1)) ? (maxY - 1) : y));

    SCREEN_BUFF[y][x] = ((colour) ? COL_BLACK : COL_WHITE);
}

/*
 * gdi_set_point:
 *	Plot a pixel.
 *******************************************************************************
 */
int32 gdi_get_point(int32 x, int32 y)
{
    if (mirrorX)
        x = (maxX - x - 1);

    if (mirrorY)
        y = (maxY - y - 1);

    x = ((x < 0) ? 0 : ((x > (maxX - 1)) ? (maxX - 1) : x));
    y = ((y < 0) ? 0 : ((y > (maxY - 1)) ? (maxY - 1) : y));

    return ((SCREEN_BUFF[y][x] == COL_BLACK) ? TRUE : FALSE);
}

/*
 * gdi_line: gdi_lineto:
 *	Classic Bressenham Line code
 *******************************************************************************
 */
void gdi_line(int32 x0, int32 y0, int32 x1, int32 y1, int32 colour)
{
    int32 dx, dy;
    int32 sx, sy;
    int32 err, e2;

    lastX = x1;
    lastY = y1;

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);

    sx = (x0 < x1) ? 1 : -1;
    sy = (y0 < y1) ? 1 : -1;

    err = dx - dy;
    for (;;)
    {
        gdi_set_point(x0, y0, colour);
        if ((x0 == x1) && (y0 == y1))
            break;

        e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void gdi_lineto(int32 x, int32 y, int32 colour)
{
    gdi_line(lastX, lastY, x, y, colour);
}

/*
 * gdi_rectangle:
 *	A rectangle is a spoilt days fishing
 *******************************************************************************
 */
void gdi_rectangle(int32 x1, int32 y1, int32 x2, int32 y2, int32 colour,
                   int32 filled)
{
    int32 x;
    if (filled)
    {
        if (x1 == x2)
        {
            gdi_line(x1, y1, x2, y2, colour);
        }
        else if (x1 < x2)
        {
            for (x = x1; x <= x2; ++x)
            {
                gdi_line(x, y1, x, y2, colour);
            }
        }
        else
        {
            for (x = x2; x <= x1; ++x)
            {
                gdi_line(x, y1, x, y2, colour);
            }
        }
    }
    else
    {
        gdi_line(x1, y1, x2, y1, colour);
        gdi_lineto(x2, y2, colour);
        gdi_lineto(x1, y2, colour);
        gdi_lineto(x1, y1, colour);
    }
}

/*
 * gdi_circle:
 *      This is the midpoint32 circle algorithm.
 *******************************************************************************
 */
void gdi_circle(int32 x, int32 y, int32 r, int32 colour, int32 filled)
{
    int32 ddF_x = 1;
    int32 ddF_y = -2 * r;

    int32 f = 1 - r;
    int32 x1 = 0;
    int32 y1 = r;

    if (filled)
    {
        gdi_line(x, y + r, x, y - r, colour);
        gdi_line(x + r, y, x - r, y, colour);
    }
    else
    {
        gdi_set_point(x, y + r, colour);
        gdi_set_point(x, y - r, colour);
        gdi_set_point(x + r, y, colour);
        gdi_set_point(x - r, y, colour);
    }

    while (x1 < y1)
    {
        if (f >= 0)
        {
            y1--;
            ddF_y += 2;
            f += ddF_y;
        }
        x1++;
        ddF_x += 2;
        f += ddF_x;
        if (filled)
        {
            gdi_line(x + x1, y + y1, x - x1, y + y1, colour);
            gdi_line(x + x1, y - y1, x - x1, y - y1, colour);
            gdi_line(x + y1, y + x1, x - y1, y + x1, colour);
            gdi_line(x + y1, y - x1, x - y1, y - x1, colour);
        }
        else
        {
            gdi_set_point(x + x1, y + y1, colour);
            gdi_set_point(x - x1, y + y1, colour);
            gdi_set_point(x + x1, y - y1, colour);
            gdi_set_point(x - x1, y - y1, colour);
            gdi_set_point(x + y1, y + x1, colour);
            gdi_set_point(x - y1, y + x1, colour);
            gdi_set_point(x + y1, y - x1, colour);
            gdi_set_point(x - y1, y - x1, colour);
        }
    }
}

/*
 * gdi_ellipse:
 *	Fast ellipse drawing algorithm by
 *      John Kennedy
 *	Mathematics Department
 *	Santa Monica College
 *	1900 Pico Blvd.
 *	Santa Monica, CA 90405
 *	jrkennedy6@gmail.com
 *	-Confirned in email this algorithm is in the public domain -GH-
 *******************************************************************************
 */
static void plot4ellipsePoints(int32 cx, int32 cy, int32 x, int32 y,
                               int32 colour, int32 filled)
{
    if (filled)
    {
        gdi_line(cx + x, cy + y, cx - x, cy + y, colour);
        gdi_line(cx - x, cy - y, cx + x, cy - y, colour);
    }
    else
    {
        gdi_set_point(cx + x, cy + y, colour);
        gdi_set_point(cx - x, cy + y, colour);
        gdi_set_point(cx - x, cy - y, colour);
        gdi_set_point(cx + x, cy - y, colour);
    }
}

void gdi_ellipse(int32 cx, int32 cy, int32 xRadius, int32 yRadius,
                 int32 colour, int32 filled)
{
    int32 x, y;
    int32 xChange, yChange, ellipseError;
    int32 twoAsquare, twoBsquare;
    int32 stoppingX, stoppingY;

    twoAsquare = 2 * xRadius * xRadius;
    twoBsquare = 2 * yRadius * yRadius;

    x = xRadius;
    y = 0;

    xChange = yRadius * yRadius * (1 - 2 * xRadius);
    yChange = xRadius * xRadius;

    ellipseError = 0;
    stoppingX = twoBsquare * xRadius;
    stoppingY = 0;

    while (stoppingX >= stoppingY) // 1st set of point32s
    {
        plot4ellipsePoints(cx, cy, x, y, colour, filled);
        ++y;
        stoppingY += twoAsquare;
        ellipseError += yChange;
        yChange += twoAsquare;

        if ((2 * ellipseError + xChange) > 0)
        {
            --x;
            stoppingX -= twoBsquare;
            ellipseError += xChange;
            xChange += twoBsquare;
        }
    }

    x = 0;
    y = yRadius;

    xChange = yRadius * yRadius;
    yChange = xRadius * xRadius * (1 - 2 * yRadius);

    ellipseError = 0;
    stoppingX = 0;
    stoppingY = twoAsquare * yRadius;

    while (stoppingX <= stoppingY) // 2nd set of point32s
    {
        plot4ellipsePoints(cx, cy, x, y, colour, filled);
        ++x;
        stoppingX += twoBsquare;
        ellipseError += xChange;
        xChange += twoBsquare;

        if ((2 * ellipseError + yChange) > 0)
        {
            --y;
            stoppingY -= twoAsquare;
            ellipseError += yChange;
            yChange += twoAsquare;
        }
    }
}

/*
 * gdi_putc:
 *	Print a single character to the screen
 *******************************************************************************
 */
void gdi_putc(int32 x, int32 y, int32 c, int32 bgCol, int32 fgCol)
{
    int32 y1, y2;

    uint8 line;
    uint8 *fontPtr;

    // Can't print if we're offscreen

    x = ((x < 0) ? 0 : ((x > (maxX - fontWidth)) ? (maxX - fontWidth) : x));
    y = ((y < 0) ? 0 : ((y > (maxY - fontHeight)) ? (maxY - fontHeight) : y));
    c = ((c > 255) ? '?' : c);

    fontPtr = font + c * fontHeight;

    for (y1 = 0; y1 < fontHeight; y1++)
    {
        y2 = y + y1;
        line = *fontPtr++;
        gdi_set_point(x + 0, y2, (line & 0x80) == 0 ? bgCol : fgCol);
        gdi_set_point(x + 1, y2, (line & 0x40) == 0 ? bgCol : fgCol);
        gdi_set_point(x + 2, y2, (line & 0x20) == 0 ? bgCol : fgCol);
        gdi_set_point(x + 3, y2, (line & 0x10) == 0 ? bgCol : fgCol);
        gdi_set_point(x + 4, y2, (line & 0x08) == 0 ? bgCol : fgCol);
        gdi_set_point(x + 5, y2, (line & 0x04) == 0 ? bgCol : fgCol);
        gdi_set_point(x + 6, y2, (line & 0x02) == 0 ? bgCol : fgCol);
        gdi_set_point(x + 7, y2, (line & 0x01) == 0 ? bgCol : fgCol);
    }
}

/*
 * gdi_puts:
 *	Send a string to the display. Obeys \n and \r formatting
 *******************************************************************************
 */
void gdi_puts(int32 x, int32 y, const int8 *str, int32 bgCol, int32 fgCol)
{
    int32 c, mx, my;

    mx = x;
    my = y;

    while (*str)
    {
        c = *str++;

        if (c == '\r')
        {
            mx = x;
            continue;
        }

        if (c == '\n')
        {
            my += fontHeight;
            if (my > (maxY - fontHeight))
                return;
            continue;
        }

        gdi_putc(mx, my, c, bgCol, fgCol);

        mx += fontWidth;
        if (mx > (maxX - fontWidth))
        {
            mx = 0;
            my += fontHeight;
            if (my > (maxY - fontHeight))
                return;
        }
    }
}

/*
 * gdi_putbmp:
 *	Send a picture to the display.
 *******************************************************************************
 */
void gdi_putbmp(int32 x0, int32 y0, int32 with, int32 height, uint8 *bmp)
{
    int32 x = 0, y = 0;
    for (y = y0; y < (height); y++)
    {
        for (x = x0; x < with; x++)
        {
            SCREEN_BUFF[y][x] = ((*bmp++) ? COL_BLACK : COL_WHITE);
        }
    }
}

int32 gdi_hzk_load(const int8 *path)
{
    uint8 *flag = NULL;
    fpHZK = fopen(path, "rb");
    if (fpHZK == NULL)
    {
        printf("ERROR: Can't open file[%s]!\n", path);
        goto err;
    }

    fseek(fpHZK, 0, SEEK_SET);
    fread(&HZK, 1, sizeof(hzk_t), fpHZK);

    // printf("HZK: %08X\n", HZK.flag);

    flag = (uint8 *)&(HZK.flag);
    if ((flag[0] != 'F') || (flag[1] != 'O') || (flag[2] != 'N') || (flag[3] != 'T')) // "FONT"
    {
        printf("ERROR: File[%s] is not a HZK font file!\n", path);
        goto err;
    }

    if ((HZK.height <= 0) || (HZK.width <= 0))
    {
        printf("ERROR: HZK File[%s] height or width is invalid!\n", path);
        goto err;
    }

    if ((HZK.start > HZK.end) || (HZK.start < 0x4E00) || (HZK.end > 0x9FFF))
    {
        printf("ERROR: HZK File[%s] start or end is invalid!\n", path);
        goto err;
    }

    printf("HZK height[%d], width[%d], start[%X], end[%X]\n", HZK.height, HZK.width, HZK.start, HZK.end);

    return OK;
err:
    if (fpHZK != NULL)
    {
        fclose(fpHZK);
        fpHZK = NULL;
    }
    return ERROR;
}

int32 gdi_hzk_unload(void)
{
    fclose(fpHZK);
    fpHZK = NULL;

    return OK;
}

int32 gdi_hzk_offset(const int8 *hz)
{
#if 0 // gb2312
    int32 offset = 0;

    uint8 ab = 0;
    uint8 cd = 0;
    if (hz == NULL)
    {
        return ERROR;
    }
    ab = hz[0];
    cd = hz[1];
    offset = (94 * (ab - 0xa1) + (cd - 0xa1)) * 32;

    return offset;
#else
    int32 offset = 0;
    uint32 size = 0;
    uint16 word = 0;

    if (hz == NULL)
    {
        return ERROR;
    }

    word = *((uint16 *)hz);
    size = ((HZK.width / 8) * HZK.height);
    offset += sizeof(hzk_t);
    offset += (word - HZK.start) * size;
    return offset;
#endif
}

int32 gdi_hzk_get(const int8 *hz, int8 *hzbuf)
{
    int32 offset = 0;
    int32 size = 0;
    if ((hz == NULL) || (hzbuf == NULL))
    {
        return ERROR;
    }
    offset = gdi_hzk_offset(hz);
    size = ((HZK.width / 8) * HZK.height);
    // printf("hz[%X], offset[%d], size[%d]\n", *(uint16 *)hz, offset, size);
    if (offset == ERROR)
    {
        return ERROR;
    }
    fseek(fpHZK, offset, SEEK_SET);
    fread(hzbuf, 1, size, fpHZK);

    return OK;
}

/*
 * gdi_hzk_putc:
 *	Print a single Chinese character to the screen
 *******************************************************************************
 */
void gdi_hzk_putc(int32 x, int32 y, const int8 *hz, int32 bgCol, int32 fgCol)
{
    uint8 line, xi, xj, yi;
    uint8 hzkb[HZK_BUF_MAX] = {0};

    // Can't print if we're offscreen

    x = ((x < 0) ? 0 : ((x > maxX) ? maxX : x));
    y = ((y < 0) ? 0 : ((y > maxY) ? maxY : y));

    if (gdi_hzk_get(hz, hzkb) != OK)
    {
        return;
    }

    for (yi = 0; yi < HZK.height; yi++)
    {
        for (xj = 0; xj < (HZK.width / 8); xj++)
        {
            for (xi = 0; xi < 8; xi++)
            {
                line = hzkb[yi * (HZK.width / 8) + xj] & (0x01 << (7 - xi));
                gdi_set_point(x + (xj * 8 + xi), y + yi, ((line == 0) ? bgCol : fgCol));
            }
        }
    }
}

static uint8 get_utf8_bytes(uint8 utf8)
{
    uint8 i = 0;
    for (i = 0; i < 6; i++)
    {
        if ((utf8 & (0x80 >> i)) == 0)
        {
            return i == 0 ? 1 : i;
        }
    }

    return 1;
}

static uint16 utf8_to_utf16be(const uint8 *u8, uint8 *u8len, uint16 *u16)
{
    uint16 utf16 = 0;
    uint8 i = 0, j = 0;
    uint8 length = 0, bytes = 0;

    length = strlen((char *)u8);
    bytes = get_utf8_bytes(u8[i]);

    if (length < bytes)
    {
        return 1;
    }

    if (bytes > 1)
    {
        utf16 = u8[i] & (0xFF >> (bytes + 1));
        for (j = 1; j < bytes; j++)
        {
            i++;
            utf16 <<= 6;
            utf16 += u8[i] & 0x3F;
        }
    }
    else
    {
        utf16 = u8[i];
    }

    *u8len = bytes;
    *u16 = utf16;

    return 0;
}

/*
 * gdi_puts:
 *	Send a string to the display. Obeys \n and \r formatting
 *******************************************************************************
 */
void gdi_hzk_puts(int32 x, int32 y, const int8 *str, int32 bgCol, int32 fgCol)
{
    int32 mx, my;
    uint8 ret = 0;
    uint8 u8len = 0;
    uint16 u16 = 0;

    mx = x;
    my = y;

    while ((*str != '\0') && (ret == 0))
    {
        ret = utf8_to_utf16be(str, &u8len, &u16);
        // printf("str[%X], len[%d], [%X]\n", *str, u8len, u16);
        if ((ret != 0) || (u8len == 0))
        {
            break;
        }

        if (u8len > 1)
        {
            gdi_hzk_putc(mx, my, (uint8 *)&u16, bgCol, fgCol);
            mx += HZK.width;
        }
        else
        {
            if (*str == '\r')
            {
                mx = x;
                goto next;
            }

            if (*str == '\n')
            {
                my += HZK.width;
                goto next;
            }
            gdi_putc(mx, my + (((HZK.height / 8) - 1) * 8), *str, bgCol, fgCol);
            mx += fontWidth;
        }
    next:
        str += u8len;
    }
}

#ifdef CONFIG_USE_PRTLOG
int32 gdi_prtlog_init(const uint8 *path)
{
    fpPRTLOG = fopen(path, "w+");
    if (fpPRTLOG == NULL)
    {
        printf("ERROR: Can't open file[%s]!\n", path);
        return ERROR;
    }

    return OK;
}

int32 gdi_prtlog_uninit(void)
{
    fclose(fpPRTLOG);
    fpPRTLOG = NULL;

    return OK;
}
#endif

#ifdef CONFIG_USE_ROLL
int32 gdi_roll_x(int8 rolln)
{
    if (rollON != FALSE)
    {
        if (rolln >= 0)
        {
        }
    }
    return OK;
}

int32 gdi_roll_y(int8 rolln)
{
    if (rollON != FALSE)
    {
        if (rolln >= 0)
        {
        }
    }
    return OK;
}

int32 gdi_roll_on(uint8 rollon)
{
    rollON = ((rollon != 0) ? TRUE : FALSE);
    return OK;
}
#endif

/*
 * gdi_init:
 *	Initialise the display and GPIO.
 *******************************************************************************
 */
int32 gdi_init(void)
{
    gdi_set_orientation(0);
    if (gdi_hzk_load(CONFIG_HZK16_PATH) != OK)
    {
        return ERROR;
    }
#ifdef CONFIG_USE_PRTLOG
    gdi_prtlog_init(CONFIG_PRTLOG_PATH);
#endif
    gdi_clear(0);
    gdi_update();
    return 0;
}

/*
 * gdi_uninit:
 *	Initialise the display and GPIO.
 *******************************************************************************
 */
int32 gdi_uninit(void)
{
    gdi_hzk_unload();
#ifdef CONFIG_USE_PRTLOG
    gdi_prtlog_uninit();
#endif
    return 0;
}
