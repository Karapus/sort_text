#ifndef CUSTOM_ALPHABET
#define CUSTOM_ALPHABET

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ucode_char.h"

typedef struct
{
        const char *c;
        unsigned int i;
} Letter;

int isLetter(const char *c);
unsigned int makeAlphabet(const char *str);
void freeAlphabet();
int sort_Lettercmp(const void *alpha1, const void *alpha2);
int getLetter(const char *c);
int charCmp(const char *a, const char *b);

#endif
