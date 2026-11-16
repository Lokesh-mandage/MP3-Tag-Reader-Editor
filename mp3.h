#ifndef MP3_H
#define MP3_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void view_mp3(const char *filename);

void edit_mp3(const char *filename,
              const char *option,
              const char *new_value);

void print_help(void);

#endif

