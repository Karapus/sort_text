#ifndef TEXT_PROCESSING
#define TEXT_PROCESSING

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct
{
        char* beg;
        char* end;
} String;

FILE *fOpenProt(const char *name, const char *mode, const char *err);
String *getLines(char *arr, int (*isvalid)(const char *), size_t *nlinesp = nullptr);      
char *fReadToArr(FILE *f, size_t *size = nullptr);
size_t getFSize(FILE *f);
int cEntries(const char *str, int (*isvalid)(const char *), char c = '\n');
void fPrintArr(FILE *f, String *arr);

#endif
