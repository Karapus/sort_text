#include "ucode_char.h"

int getU8Char(const char *s, char *u8c)
{
        assert(s);
        assert(u8c);

        int len = u8CharLen(s);
        for (int i = 0; i < len; i++)
                u8c[i] = s[i];
        return len;
}

int u8CharLen(const char *s)
{
        if ((*s & 0x80) == 0x00)return 1;
        int res = 0;
        if ((*s & 0xE0) == 0xC0) res = 2;
        if ((*s & 0xF0) == 0xE0) res = 3;
        if ((*s & 0xF8) == 0xF0) res = 4;
        for (int i = 1; i < res; i++)
                if ((s[i] & 0xC0) != 0x80) return 0;
        return res;
}

size_t u8StrLen(const char *s)
{
        size_t len = 0;
        for (const char *u8c = s; *u8c; u8c += u8CharLen(u8c), len++)
                ;
        return len;
}
