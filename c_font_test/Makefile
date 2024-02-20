src_dir=$(pwd)
TARGET:=c_font_test
SOURCES:=main.c gdi.c
CC:=gcc

$(TARGET):
	$(CC) $(SOURCES) -o $(TARGET)

all:$(TARGET)

clean:
	rm $(TARGET)

.PHONY:all clean