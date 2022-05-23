import io
import sys
import re
import time
import datetime
import os
import math
import random
import numpy as np

FONT_TEST_STR = "我的祖国繁荣昌盛"

FONT_BIN_FILE = "simsun.ttc-32.bin"

FONT_WIDTH = 32
FONT_HEIGHT = 32
FONT_START_CODE = 0x4E00
FONT_END_CODE = 0x9FA5

FONT_WIDTH_B = math.ceil(FONT_WIDTH / 8)

FONT_BIN_OFFSET = 0x4E00
FONT_HEADER_SIZE = (4 * 6)  # sizeof(int32) * 6

# 8 * 64 byte, 64x64 bit
FONT_BYTE_SIZE = (FONT_WIDTH_B * FONT_HEIGHT)


def read_font_header():
    global FONT_WIDTH
    global FONT_HEIGHT
    global FONT_BIN_OFFSET
    global FONT_START_CODE
    global FONT_END_CODE

    global FONT_WIDTH_B
    global FONT_BYTE_SIZE
    
    '''
    typedef struct font_bin_s
    {
        int32_t     flag;           //file flag, "FONT"
        int32_t     width;          //font width, 64
        int32_t     height;         //font height, 64
        int32_t     start;          //font start, 0x4E00
        int32_t     end;            //font end, 0x9FA5
        int32_t     mode;           //font mode, 1
    } font_bin_t;
    '''
    with open(FONT_BIN_FILE, 'rb') as f:
        f.seek(0)  # seek to header
        rbuff = f.read(FONT_HEADER_SIZE)

    flag = str(rbuff[0:4])
    width = int.from_bytes(rbuff[4:8], byteorder='little')
    height = int.from_bytes(rbuff[8:12], byteorder='little')
    start = int.from_bytes(rbuff[12:16], byteorder='little')
    end = int.from_bytes(rbuff[16:20], byteorder='little')
    mode = int.from_bytes(rbuff[20:24], byteorder='little')
    # print("flag: [%s]" % flag)
    # print("width: [%d]" % width)
    # print("height: [%d]" % height)
    # print("start: [0x%X]" % start)
    # print("end: [0x%X]" % end)
    # print("mode: [%d]" % mode)

    if flag != "b'FONT'" or width <=8 or height <=8:
        print("font file header error")
        return False

    FONT_WIDTH = width
    FONT_HEIGHT = height
    FONT_BIN_OFFSET = start
    FONT_START_CODE = start
    FONT_END_CODE = end

    FONT_WIDTH_B = math.ceil(FONT_WIDTH / 8)
    FONT_BYTE_SIZE = (FONT_WIDTH_B * FONT_HEIGHT)

    print("font width: [%d]" % FONT_WIDTH)
    print("font height: [%d]" % FONT_HEIGHT)
    print("font bin offset: [0x%X]" % FONT_BIN_OFFSET)
    print("font start code: [0x%X]" % FONT_START_CODE)
    print("font end code: [0x%X]" % FONT_END_CODE)
    print("font width byte: [%d]" % FONT_WIDTH_B)
    print("font byte size: [%d]" % FONT_BYTE_SIZE)

    return True


def read_font_buff(fcode):
    with open(FONT_BIN_FILE, 'rb') as f:
        fseek = fcode - FONT_BIN_OFFSET
        fseek = fseek * FONT_BYTE_SIZE
        fseek = fseek + FONT_HEADER_SIZE  # skip header
        f.seek(fseek)  # seek to font data
        rbuff = f.read(FONT_BYTE_SIZE)
    return rbuff


def show_font_byte(fbuff):
    byten = 0
    # for y in range(FONT_HEIGHT):
    #     print("%02X: " % y, end='')
    #     for x in range(FONT_WIDTH_B):
    #         bytei = y * FONT_WIDTH_B + x
    #         print("%02X " % fbuff[bytei], end='')
    #     print("")
    for y in range(FONT_HEIGHT):
        print("%02X: " % y, end='')
        for x in range(FONT_WIDTH_B):
            bytei = y * FONT_WIDTH_B + x
            byten = fbuff[bytei]
            for x0 in range(8):
                col = byten & (1 << (7 - x0))
                if col != 0:
                    print("*", end="")
                else:
                    print(" ", end="")
        print("")


if __name__ == "__main__":
    if read_font_header() != True:
        print("font header error")
        exit(0)

    # print("Font Test: {0}".format(FONT_TEST_STR))
    # for s in FONT_TEST_STR:
    #     FONT_TEST_BYTE.append(ord(s))

    FONT_TEST_BYTE = [x for x in range(FONT_START_CODE, FONT_END_CODE + 1)]

    # print(FONT_TEST_BYTE)

    findex = 0
    for fcode in FONT_TEST_BYTE:
        print("============================== idx[%d] code[%X] chr[%s] ==============================" % (findex, fcode, chr(fcode)))
        fbuff = read_font_buff(fcode)
        show_font_byte(fbuff)
        findex = findex + 1
