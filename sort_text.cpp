#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

struct String
{
	char* beg;
	char* end;
};

struct Letter
{
	const char *c;
	unsigned char i;
};

static Letter *alphabet_;
static unsigned int lenalphabet_;

FILE *fOpenProt(const char *name, const char *mode, const char *err);
String *getLines(char *arr, int (*isvalid)(const char *), size_t *nlinesp = nullptr);      
char *fReadToArr(FILE *f, size_t *size = nullptr);
size_t getFSize(FILE *f);
int cEntries(const char *str, int (*isvalid)(const char *), char c = '\n');
int sort_strcmp(const void *str1, const void *str2);
int sort_rStrCmp(const void *str1, const void *str2);

void fPrintArr(FILE *f, String *arr);

int isLetter(const char *c);
unsigned int makeAlphabet(const char *str);
int sort_Lettercmp(const void *alpha1, const void *alpha2);
int getLetter(const char *c);
int charCmp(const char *a, const char *b);

int getU8Char(const char *s, char *u8c);
int u8CharLen(const char *s);
size_t u8StrLen(const char *s);

/*!	@mainpage
 * 	Program to sort file lines in lexographical order					\n
 * 	usage:											\n
 * 	sorttext tosort.txt sorted.txt	- read from tosort.txt, sort and write to sorted.txt	\n
 *	sorttext tosort.txt		- read from tosort.txt, sort and write to stdout	\n
 *	sorttext			- read from stdin, sort and write to stdout		\n
 */
int main(int argc, char** argv)
{
	FILE *f_to_sort = stdin;
	if ((argc - 1) > 0) f_to_sort = fOpenProt(argv[1], "rb", "File opening failed");
	if (!f_to_sort)	return 1;

	FILE *f_sorted = stdout;
	if ((argc - 1) > 1) f_sorted = fOpenProt(argv[2], "w", "File write failed");
	if (!f_sorted) 	return 2;

/*	lenalphabet_ = makeAlphabet("BCAaфывА");
	for (Letter *cur_let = alphabet_; cur_let - alphabet_ < lenalphabet_; cur_let++)
		printf("alphabet_[%ld].c == '%s', .i == %d\n", cur_let - alphabet_, cur_let->c, cur_let->i);

	for (const char *teststr = "AaBbCc"; *teststr; teststr++)
		printf("isLetter(%c) = %d\n", *teststr, isLetter(teststr));

	printf("charCmp(ы, ф) == %d\n", charCmp("ы", "ф"));
*/
	char *chars = fReadToArr(f_to_sort);
	fclose(f_to_sort);
	
	assert(chars != nullptr);

	lenalphabet_ = makeAlphabet("АаБбВвГгДдЕеЁёЖжЗзИиКкЛлМмНнИиПпРрСсТтУуФфХхЦцЧчШшЩщъыьЭэЮюЯяAaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");

/*	char u8c[5] = {'\0'};
	for (const char *s = "абвгдеё"; *s; s += u8CharLen(s))
	{
		getU8Char(s, u8c);
		printf("%s\n", u8c);
	}
*/
	size_t nlines = 0;
 String *lines = getLines(chars, isLetter, &nlines);
	
	assert(lines != nullptr);
	
	qsort(lines, nlines, sizeof(*lines), sort_rStrCmp);

	fPrintArr(f_sorted, lines);
	fclose(f_sorted);

	free(chars);
	chars = nullptr;

	free(lines);
	lines = nullptr;

	free(alphabet_);
	alphabet_ = nullptr;
}

/*!	function to open file. If error occurse, prints message
 * 	@param name	- file name
 * 	@param mode	- file open mode
 * 	@param err	- error message
 */
FILE *fOpenProt(const char *name, const char *mode, const char *err)
{
	assert(name != nullptr);
	assert(mode != nullptr);
	assert(err != nullptr);

	FILE *f  = fopen(name, mode);
	if (!f)	perror(err);
	return f;
}

/*!	@brief prints strings from nullptr terminated string array into file
 * 	@param f - file to write to
 * 	@param arr - array to write
 */
void fPrintArr(FILE *f, String *arr)
{
	assert(f != nullptr);
	assert(arr != nullptr);

	for(String *str = arr; str->beg; str++)
	{
		fputs(str->beg,	f);
		fputc('\n',	f);
	}
}

/*!	@brief reads file to char array
 *	@param f - file to read
 *	@param size - pointer to size_t to write size in bytes, if specified
 *	@note default value of size = nullptr
 *	@return pointer to null terminated string of file content
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

/*!	@brief returns size of file in bytes
 * 	@param f - file to mesure
 * 	@return size of file f in bytes
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

/*!	@brief comparator function for comparing strings from array
 * 	@param str1 - pointer to string 1
 * 	@param str2 - pointer to string 2
 * 	@return result of strcmp of these strings
 */
int sort_strcmp(const void *str1, const void *str2)
{
	assert(str1 != nullptr);
	assert(str2 != nullptr);

	return strcmp(((String *)str1)->beg, ((String *)str2)->beg);
}

/*!	@brief comparator function for reversed comparing strings from array
 * 	@param str1 - pointer to string 1
 * 	@param str2 - pointer to string 2
 * 	@return result of comparing reversed strings without trailing non-alphabet symbols
 */
int sort_rStrCmp(const void *str1, const void *str2)
{
	assert(str1 != nullptr);
	assert(str2 != nullptr);

	const String *s1_p = ((const String *)str1);
	const String *s2_p = ((const String *)str2);

	char *c1_p = s1_p->end;
	char *b1_p = s1_p->beg;
	while (!isLetter(b1_p) && (b1_p < s1_p->end)) b1_p += u8CharLen(b1_p);

	char *c2_p = s2_p->end;
	char *b2_p = s2_p->beg;
	while (!isLetter(b2_p) && (b2_p < s2_p->end)) b2_p += u8CharLen(b2_p);

	while (c1_p-- > b1_p)
	{
		if (isLetter(c1_p))
		{
			while (c2_p-- > b2_p)
				if (isLetter(c2_p))
					if (charCmp(c1_p, c2_p) != 0) return charCmp(c1_p, c2_p);
					else break;
			if (c2_p == b2_p && c1_p != b1_p) return 1;
		}
		if (c1_p == b1_p && c2_p != b2_p) return -1;
	}
	return 0;
}

/*!	@brief converts every '\\n' in given string into '\0' and returns array of strings
 * 	@param arr	- string to operate with
 * 	@param nlinesp	- pointer to int to save number of strings, if specified
 * 	@note default value of nlinesp = nullptr
 * 	@return pointer to null terminated array of strings
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

/*!	@brief counts entries of char c in string str
 * 	@param str	- null terminated string
 * 	@param c	- char to count
 * 	@return count of c in str
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

/*!
 * 	@brief makes alpabet of letters from string
 * 	@param str	- string to make alphabet from
 * 	@return length of alphabet
 */
unsigned int makeAlphabet(const char *str)
{
	assert(str != nullptr);

	unsigned int len = u8StrLen(str);
	
	alphabet_ = (Letter *) calloc(sizeof(*alphabet_), len);
	{
		Letter *cur_let = alphabet_;
		for (const char *cur_c = str; *cur_c; cur_let++)
		{
			cur_let->c = cur_c, cur_let->i = cur_let - alphabet_;
			int l = 0;
			if (!(l = u8CharLen(cur_c))) return -1;
			cur_c += l;
		}
	}
	qsort(alphabet_, len, sizeof(*alphabet_), sort_Lettercmp);
	
	return len;
}

int isLetter(const char *c)
{
	return getLetter(c) + 1;
}

/*!	
 * 	@brief get index of alphabet_ corresponding to c
 * 	@param c	char to find
 * 	@return index of alphabet_
 */
int getLetter(const char *c)
{
	assert(c);

	const Letter alpha = {c, 0};
	const Letter *res = (Letter *) bsearch(&alpha, alphabet_, lenalphabet_, sizeof(alpha), sort_Lettercmp);
	if (res == nullptr) return -1;
	return res - alphabet_;
}

/*!
 * 	@brief function to sort alphabet
 */
int sort_Lettercmp(const void *alpha1, const void *alpha2)
{
	assert(alpha1 != nullptr);
	assert(alpha2 != nullptr);
	
	const char *a = ((Letter *)alpha1)->c;
	const char *b = ((Letter *)alpha2)->c;
	return strncmp(a, b, u8CharLen(a) < u8CharLen(b) ? u8CharLen(a) : u8CharLen(b));
}

/*!
 * 	@brief comparse chars by alphabet
 * 	@param a first char;
 * 	@param b second char;
 * 	@return > 0 if a > b, 0 if a = b, < 0 if a < b
 */
int charCmp(const char *a, const char *b)
{
	assert(a);
	assert(b);

	int alpha = getLetter(a);
	int beta  = getLetter(b);
	if (alpha < 0 &&  beta < 0) return 0;
	if (alpha < 0) return -1;
	if (beta < 0) return 1;
        return alphabet_[alpha].i - alphabet_[beta].i;
}

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
