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

FONT_BIN_FILE = "simkai.ttf-64.bin"

FONT_WIDTH = 64
FONT_HEIGHT = 64

FONT_WIDTH_B = math.ceil(FONT_WIDTH / 8)

FONT_BIN_OFFSET = 0x4E00
FONT_HEADER_SIZE = (4 * 6)  # sizeof(int32) * 6

# 8 * 64 byte, 64x64 bit
FONT_BYTE_SIZE = (FONT_WIDTH_B * FONT_HEIGHT)


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
    for y in range(FONT_HEIGHT):
        for x in range(FONT_WIDTH_B):
            byten = fbuff[(y * 8) + x]
            for x0 in range(8):
                col = byten & (1 << (7 - x0))
                if col != 0:
                    print("*", end="")
                else:
                    print(" ", end="")
        print("")


if __name__ == "__main__":

    print("Font Test: {0}".format(FONT_TEST_STR))

    FONT_TEST_BYTE = []
    for s in FONT_TEST_STR:
        FONT_TEST_BYTE.append(ord(s))
    # print(FONT_TEST_BYTE)

    for fcode in FONT_TEST_BYTE:
        print("============================== {0} ==============================".format(
            chr(fcode)))
        fbuff = read_font_buff(fcode)
        show_font_byte(fbuff)
