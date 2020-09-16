#encoding: utf-8
import os
import pygame
from PIL import Image

FONT_SIZE = 32
FONT_SRC="msyh.ttc"

def font_init():
    pygame.init()

def font_to_image(word):
    font = pygame.font.Font(FONT_SRC, FONT_SIZE)
    # print("font.size:", font.size(word))
    font_metrics = font.metrics(word)
    (minx, maxx, miny, maxy, advance) = font_metrics[0]
    # print("font.metrics:", font.metrics(word))
    # print("font.get_linesize:", font.get_linesize())
    # print("font.get_height:", font.get_height())
    # print("font.get_ascent:", font.get_ascent())
    # print("font.get_descent:", font.get_descent())
    rtext = font.render(word, True, (0, 0, 0), (255, 255, 255))
    # print("rtext.get_size:", rtext.get_size())

    cropped = pygame.Surface((FONT_SIZE, FONT_SIZE))
    cropped.fill((255, 255, 255))
    blit_rect = (0, font.get_ascent() - maxy, maxx, font.get_ascent())
    cropped.blit(rtext, (0, 0), blit_rect)
    imgbuff = pygame.image.tostring(cropped, "RGBA", False)
    image = Image.frombytes("RGBA", (FONT_SIZE, FONT_SIZE), imgbuff)
    image = image.convert("1")
    return image

pygame.init()
#start, end = (0x4E00, 0x9FA5)  # 汉字编码范围
start, end = (0x4E00, 0x4EFF)  # 汉字编码范围
for codepoint in range(int(start), int(end)):
    word = chr(codepoint)
    print("==========", word, "==========")
    font = pygame.font.Font("msyh.ttc", FONT_SIZE)
    # print("font.size:", font.size(word))
    font_metrics = font.metrics(word)
    (minx, maxx, miny, maxy, advance) = font_metrics[0]
    # print("font.metrics:", font.metrics(word))
    # print("font.get_linesize:", font.get_linesize())
    # print("font.get_height:", font.get_height())
    # print("font.get_ascent:", font.get_ascent())
    # print("font.get_descent:", font.get_descent())
    rtext = font.render(word, False, (0, 0, 0), (255, 255, 255))
    # print("rtext.get_size:", rtext.get_size())

    cropped = pygame.Surface((FONT_SIZE, FONT_SIZE))
    cropped.fill((255, 255, 255))
    cropped.blit(rtext, (0, 0), (0, font.get_ascent() -
                                 maxy, maxx, font.get_ascent()))
    # print("cropped.get_size:", cropped.get_size())
    # pygame.image.save(cropped, os.path.join(chinese_dir, word + ".png"))
    imgbuff = pygame.image.tostring(cropped, "RGBA", False)
    image = Image.frombytes("RGBA", (FONT_SIZE, FONT_SIZE), imgbuff)
    image = image.convert("1")
    (fmaxx, fmaxy) = image.size
    for y in range(fmaxy):
        for x in range(fmaxx):
            col = image.getpixel((x, y))
            if col != 0:
                #col = 1
                print(" ", end="")
            else:
                #col = 0
                print("*", end="")
            #print("{0}".format(col), end="")
        print("")
