#encoding: utf-8
import io
import sys
import re
import time
import datetime
import os
import math
import random
import numpy as np

G_FONT_START = 0x0000
G_FONT_END = 0xFFFF

G_FONT_CODE_LOG = "py_font_code.log"

#sys.stdout = io.TextIOWrapper(sys.stdout.buffer,encoding='utf-16') #改变标准输出的默认编码

#for fcode in range(int(G_FONT_START), int(G_FONT_END) + 1):
    #fword = chr(fcode)
#    fcode_hex = "%04X" % fcode
#    byte_arry = bytes.fromhex(fcode_hex)
    #print("==========", len(byte_arry), byte_arry, "==========")
#    byte_arry = bytearray(byte_arry)
#    byteorder = byte_arry[0]
#    byte_arry[0] = byte_arry[1]
#    byte_arry[1] = byteorder
    #print("==========", len(byte_arry), byte_arry, "==========")
#    fword = byte_arry.decode("utf-16", "ignore")
#    print("==========", fword, "[0x{0}]".format(fcode_hex), "==========")


fileno = open(G_FONT_CODE_LOG, "w+", encoding='utf-8')

for fcode in range(int(G_FONT_START), int(G_FONT_END) + 1):
    fword = chr(fcode)
    fcode_hex = "%04X" % fcode
    
    if not fword.isprintable():
        fword = "XX"
    flogs = "========== [{0}] [0x{1}] ==========".format(fword, fcode_hex)
    print(flogs)
    fileno.write(flogs)
    fileno.write("\n")

fileno.close()