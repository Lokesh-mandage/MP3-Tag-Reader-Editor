CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = mp3tag
SOURCES = main.c mp3view.c mp3edit.c
HEADERS = mp3.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) temp.mp3

.PHONY: all clean
