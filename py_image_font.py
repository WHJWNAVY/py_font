from PIL import Image, ImageFont, ImageDraw

WORD = chr(0x4EEE)
FONT_SRC = "msyh.ttc"
FONT_SIZE = 32
myfont = ImageFont.truetype(font=FONT_SRC, size=FONT_SIZE,
                          layout_engine=ImageFont.LAYOUT_RAQM)

start, end = (0x4E00, 0x4EFF)  # 汉字编码范围
for codepoint in range(int(start), int(end)):
    word = chr(codepoint)
    print("==========", word, "==========")
    print("font.getsize() ==>[{0}]".format(myfont.getsize(word)))
    print("font.getoffset() ==>[{0}]".format(myfont.getoffset(word)))
    print("font.getmetrics() ==>[{0}]".format(myfont.getmetrics()))
    print("font.getmask().getbbox() ==>[{0}]".format(myfont.getmask(word).getbbox()))

    print(type(myfont.getmask(word)))

    image = Image.new("RGB", myfont.getsize(word))
    draw = ImageDraw.Draw(image)
    draw.text((0, 0), word, font=myfont)
    image = image.convert("1")
    image.save("{0}.bmp".format(word))
