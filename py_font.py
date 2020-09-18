#encoding: utf-8
import sys
import re
import time
import datetime
import os
import math
import random
import pygame
import numpy as np
from PIL import Image

FONT_LIST = [
    "msyh.ttc",  # 微软雅黑
    "simsun.ttc",  # 新宋体
    "simkai.ttf",  # 楷体
    "simli.ttf",  # 隶书
    "simyou.ttf",  # 幼圆
    "stxingkai.ttf",  # 行楷
]

DEF_FONT_SIZE = 64
DEF_FONT_SRC = "msyh.ttc"
DEF_FONT_START = 0x4E00
DEF_FONT_END = 0x9FA5

G_FONT_SIZE = 64
G_FONT_SRC = "msyh.ttc"
G_FONT_START = 0
G_FONT_END = 0
G_FONT_MODE = 0

G_PATH_BASE = 'font'


def font_init(fzise=DEF_FONT_SIZE, font=DEF_FONT_SRC, fstart=DEF_FONT_START, fend=DEF_FONT_END):
    pygame.init()
    global G_FONT_SIZE
    global G_FONT_SRC
    global G_FONT_START
    global G_FONT_END
    global G_PATH_BASE
    global G_FONT_MODE

    G_FONT_SIZE = fzise
    G_FONT_SRC = font
    G_FONT_START = fstart
    G_FONT_END = fend


def font_get_path(fname, fext, path, dir):
    fpath = _filepath = os.path.join(path, dir)
    fname = "{0}.{1}".format(fname, fext)
    fullpath = os.path.join(fpath, fname)
    if os.path.exists(fpath) != True:
        os.makedirs(fpath)  # 如果指定的文件夹不存在就递归创建
    return fullpath


def font_to_image(fcode, savefp=False):
    global G_FONT_SIZE
    global G_FONT_SRC
    global G_FONT_START
    global G_FONT_END
    global G_PATH_BASE
    global G_FONT_MODE

    fword = chr(fcode)
    print("==========", fword, "[0x%X]" % fcode, "==========")
    font_src = os.path.join(G_PATH_BASE, G_FONT_SRC)
    font = pygame.font.Font(font_src, G_FONT_SIZE)
    print("font.size:", font.size(fword))
    font_metrics = font.metrics(fword)
    font_descent = font.get_descent()
    font_ascent = font.get_ascent()
    font_height = font.get_height()
    font_linesize = font.get_linesize()

    font_baseline = font_height + font_descent
    (fminx, fmaxx, fminy, fmaxy, fadvance) = font_metrics[0]

    font_minx = fminx
    font_maxx = fmaxx
    font_miny = font_baseline - fmaxy
    font_maxy = font_baseline - fminy

    font_rect = (font_minx, font_miny, font_maxx, font_maxy)
    font_size = (font_maxx - font_minx, font_maxy - font_miny + 1)

    print("font.metrics:", font_metrics)
    print("font.height:", font_height)
    print("font.baseline:", font_baseline)
    print("font.ascent:", font_ascent)
    print("font.descent:", font_descent)
    print("font.linesize:", font_linesize)

    print("font.rect:", font_rect)
    print("font.size:", font_size)

    rtext = font.render(fword, False, (0, 0, 0), (255, 255, 255))
    print("rtext.get_size:", rtext.get_size())

    blit_dest = (math.ceil(
        (G_FONT_SIZE - font_size[0]) / 2), math.ceil((G_FONT_SIZE - font_size[1]) / 2))

    cropped = pygame.Surface((G_FONT_SIZE, G_FONT_SIZE))
    cropped.fill((255, 255, 255))
    cropped.blit(rtext, blit_dest, font_rect)
    print("cropped.get_size:", cropped.get_size())
    imgbuff = pygame.image.tostring(cropped, "RGBA", False)
    image = Image.frombytes("RGBA", cropped.get_size(), imgbuff)
    image = image.convert("1")
    if savefp == True:
        fpath = G_PATH_BASE
        fdir = "{0}-{1}".format(G_FONT_SRC, G_FONT_SIZE)
        iname = "{0}-{1}".format(fword, "%X" % fcode)
        fname = font_get_path(iname, 'bmp', fpath, fdir)
        image.save(fname)
    return image


def image_to_bytes(image, debug=False):
    global G_FONT_SIZE
    global G_FONT_SRC
    global G_FONT_START
    global G_FONT_END
    global G_PATH_BASE
    global G_FONT_MODE

    (font_width, font_height) = image.size

    byten = 0
    bytei = 0

    arrya_x = math.ceil(font_width / 8)
    array_y = font_height
    byte_arry = np.zeros((arrya_x * array_y), dtype=np.uint8)

    for y in range(font_height):
        for x in range(font_width):
            if ((x % 8) == 0) and (x != 0):
                byte_arry[bytei] = byten
                bytei += 1
                byten = 0

            byten = byten << 1
            col = image.getpixel((x, y))
            if col == 0:
                byten = byten | 1
                if debug == True:
                    print("*", end="")
            else:
                if debug == True:
                    print(" ", end="")
        if debug == True:
            print("")
        byte_arry[bytei] = byten
        bytei += 1
        byten = 0
    return bytearray(byte_arry)

def font_write_header(ffile):
    global G_FONT_SIZE
    global G_FONT_SRC
    global G_FONT_START
    global G_FONT_END
    global G_PATH_BASE
    global G_FONT_MODE

    # 写入镜像文件头信息
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

    font_header = np.array([G_FONT_SIZE, G_FONT_SIZE, G_FONT_START, G_FONT_END, G_FONT_MODE], dtype=np.int32)
    font_header = bytearray(font_header)
    ffile.write(bytearray(b"FONT"))
    ffile.write(font_header)

def font_convert_bin(ffile, save_image=False):
    global G_FONT_SIZE
    global G_FONT_SRC
    global G_FONT_START
    global G_FONT_END
    global G_PATH_BASE

    for fcode in range(int(G_FONT_START), int(G_FONT_END) + 1):
        image = font_to_image(fcode, save_image)
        byte_arry = image_to_bytes(image, save_image)
        # print("byte_arry:", byte_arry)
        ffile.write(byte_arry)


if __name__ == "__main__":

    font_init(font=FONT_LIST[2])
    font_path = G_PATH_BASE
    font_dir = "{0}-{1}".format(G_FONT_SRC, G_FONT_SIZE)
    font_image = "{0}-{1}".format(G_FONT_SRC, G_FONT_SIZE)
    font_name = font_get_path(font_image, 'bin', font_path, font_dir)
    font_file = open(font_name, "wb")
    font_write_header(font_file)
    font_convert_bin(font_file, False)
    font_file.close()
