#ifndef UCODE_CHAR
#define UCODE_CHAR

#include <stddef.h>
#include <assert.h>

int getU8Char(const char *s, char *u8c);
int u8CharLen(const char *s);
size_t u8StrLen(const char *s);

#endif
