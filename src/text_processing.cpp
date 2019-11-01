#include "text_processing.h"

/*!     function to open file. If error occurse, prints message
 *      @param name     - file name
 *      @param mode     - file open mode
 *      @param err      - error message
 */
FILE *fOpenProt(const char *name, const char *mode, const char *err)
{
        assert(name != nullptr);
        assert(mode != nullptr);
        assert(err != nullptr);

        FILE *f  = fopen(name, mode);
        if (!f) perror(err);
        return f;
}

/*!     @brief reads file to char array
 *      @param f - file to read
 *      @param size - pointer to size_t to write size in bytes, if specified
 *      @note default value of size = nullptr
 *      @return pointer to null terminated string of file content
 */
char *fReadToArr(FILE *f, size_t *size)
{
        assert(f != nullptr);

        size_t nchars = getFSize(f);
        char *chars = (char *)calloc(sizeof(char), nchars + 1);

        assert(chars != nullptr);

        nchars = fread(chars, sizeof(char), nchars, f);
        chars[nchars] = '\0';

        if (size) *size = nchars;

        return chars;
}

/*!     @brief returns size of file in bytes
 *      @param f - file to mesure
 *      @return size of file f in bytes
 */
size_t getFSize(FILE *f)
{
        assert(f != nullptr);

        long cur_pos = ftell(f);
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, cur_pos, SEEK_SET);
        return size;
}

/*!     @brief converts every '\\n' in given string into '\0' and returns array of strings
 *      @param arr      - string to operate with
 *      @param nlinesp  - pointer to int to save number of strings, if specified
 *      @note default value of nlinesp = nullptr
 *      @return pointer to null terminated array of strings
 */
String *getLines(char *arr, int (*isvalid)(const char *), size_t *nlinesp)
{
        assert(arr != nullptr);

        int linectr = cEntries(arr, isvalid);
        String *lines = (String *)calloc(sizeof(String), linectr + 1);
        linectr = 0;
        lines->beg = arr;
        int val_s = 0;
        for (char *c_p = arr; *c_p != '\0'; c_p++)
        {
                val_s = val_s ? 1 :(*isvalid)(c_p);
                if (*c_p == '\n')
                {
                        lines[linectr].end = c_p;
                        linectr += val_s ? 1 : 0;
                        lines[linectr].beg = ++c_p;
                        *(c_p - 1) = '\0';
                        /*printf("getLines():\n %d \\n;\nPrevious string:\n\"%s\"\nCurrent string:\n\"%s\"\n",
                                linectr, lines[linectr - 1].beg, lines[linectr].beg);*/

                }
        }
        lines[linectr].beg = lines[linectr].end = nullptr;

        if (nlinesp) *nlinesp = linectr;

        return lines;
}

/*!     @brief counts entries of char c in string str
 *      @param str      - null terminated string
 *      @param c        - char to count
 *      @return count of c in str
 */
int cEntries(const char* str, int (*isvalid)(const char *), char c)
{
        assert(str != nullptr);

        int count = 0;
        int val_s = 0;
        for (const char *cur_c = str; *cur_c; cur_c++)
        {
                val_s = val_s ? 1 : (*isvalid)(cur_c);
                if (*cur_c == c && val_s) count++;
        }
        return count;
}

/*!     @brief prints strings from nullptr terminated string array into file
 *      @param f - file to write to
 *      @param arr - array to write
 */
void fPrintArr(FILE *f, String *arr)
{
        assert(f != nullptr);
        assert(arr != nullptr);

        for(String *str = arr; str->beg; str++)
        {
                fputs(str->beg, f);
                fputc('\n',     f);
        }
}
