#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

struct string
{
	char* beg;
	char* end;
};

struct letter
{
	char c;
	unsigned char i;
};

static letter *alphabet_;
static unsigned int lenalphabet_;

FILE *fopen_prot(const char *name, const char *mode, const char *err);
string *getlines(char *arr, int (*isvalid)(int), size_t *nlinesp = nullptr);      
char *freadtoarr(FILE *f, size_t *size = nullptr);
size_t getfsize(FILE *f);
int centries(char *str, int (*isvalid)(int), char c = '\n');
int sort_strcmp(const void *str1, const void *str2);
int sort_rstrcmp(const void *str1, const void *str2);
void fprintarr(FILE *f, string *arr);
int isletter(int c);
unsigned int makealphabet(char *str);
int sort_lettercmp(const void *alpha1, const void *alpha2);
int getletter(char c);
int charcmp(char a, char b);

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
	if ((argc - 1) > 0) f_to_sort = fopen_prot(argv[1], "rb", "File opening failed");
	if (!f_to_sort)	return 1;

	FILE *f_sorted = stdout;
	if ((argc - 1) > 1) f_sorted = fopen_prot(argv[2], "w", "File write failed");
	if (!f_sorted) 	return 2;

	/*lenalphabet_ = makealphabet_("BCAaфывА");
	for (letter *cur_let = alphabet_; cur_let->c; cur_let++)
		printf("alphabet_[%ld].c == '%c', alphabet_[%ld].i == %d\n", cur_let - alphabet_, cur_let->c, cur_let - alphabet_, cur_let->i);

	char *teststr = "AaBbCc";
	for (; *teststr; teststr++)
		printf("isletter(%c) = %d\n", *teststr, isletter(*teststr));

	printf("charcmp(ы, ф) == %d\n", charcmp('ы', 'ф'));*/

	char *chars = freadtoarr(f_to_sort);
	fclose(f_to_sort);
	
	assert(chars != nullptr);

	lenalphabet_ = makealphabet("АаБбВвГгДдЕеЁёЖжЗзИиКкЛлМмНнИиПпРрСсТтУуФфХхЦцЧчШшЩщъыьЭэЮюЯяAaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");
	
	size_t nlines = 0;
	string *lines = getlines(chars, isletter, &nlines);
	
	assert(lines != nullptr);
	
	qsort(lines, nlines, sizeof(*lines), sort_rstrcmp);

	fprintarr(f_sorted, lines);
	fclose(f_sorted);

	free(chars);
	chars == nullptr;

	free(lines);
	lines == nullptr;

	free(alphabet_);
	alphabet_ == nullptr;
}

/*!	function to open file. If error occurse, prints message
 * 	@param name	- file name
 * 	@param mode	- file open mode
 * 	@param err	- error message
 */
FILE *fopen_prot(const char *name, const char *mode, const char *err)
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
void fprintarr(FILE *f, string *arr)
{
	assert(f != nullptr);
	assert(arr != nullptr);

	for(string *str = arr; str->beg; str++)
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
char *freadtoarr(FILE *f, size_t *size)
{
	assert(f != nullptr);

	size_t nchars = getfsize(f);	
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
size_t getfsize(FILE *f)
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

	return strcmp(((string *)str1)->beg, ((string *)str2)->beg);
}

/*!	@brief comparator function for reversed comparing strings from array
 * 	@param str1 - pointer to string 1
 * 	@param str2 - pointer to string 2
 * 	@return result of comparing reversed strings without trailing non-alphabet symbols
 */
int sort_rstrcmp(const void *str1, const void *str2)
{
	assert(str1 != nullptr);
	assert(str2 != nullptr);

	const string *s1_p = ((const string *)str1);
	const string *s2_p = ((const string *)str2);

	char *c1_p = s1_p->end;
	char *b1_p = s1_p->beg;
	while (!isalpha(*b1_p) && (b1_p < s2_p->end)) b1_p++;

	char *c2_p = s2_p->end;
	char *b2_p = s2_p->beg;
	while (!isalpha(*b2_p) && (b2_p < s2_p->end)) b2_p++;

	while (c1_p-- > b1_p)
	{
		if (isalpha(*c1_p))
		{
			while (c2_p-- > b2_p)
				if (isalpha(*c2_p))
					if (*c1_p != *c2_p) return charcmp(*c1_p, *c2_p);
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
string *getlines(char *arr, int (*isvalid)(int), size_t *nlinesp)
{
	assert(arr != nullptr);

	int linectr = centries(arr, isvalid);
	string *lines = (string *)calloc(sizeof(string), linectr + 1);
	linectr = 0;
	lines->beg = arr;
	int val_s = 0;
	for (char *c_p = arr; *c_p != '\0'; c_p++)
	{
		val_s = val_s ? 1 :(*isvalid)(*c_p);
		if (*c_p == '\n')
		{
			lines[linectr].end = c_p;
			linectr += val_s ? 1 : 0;
			lines[linectr].beg = ++c_p;
			*(c_p - 1) = '\0';
			/*printf("getlines():\n %d \\n;\nPrevious string:\n\"%s\"\nCurrent string:\n\"%s\"\n",
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
int centries(char* str, int (*isvalid)(int), char c)
{
	assert(str != nullptr);

	int count = 0;
	int val_s = 0;
	for (char *cur_c = str; *cur_c; cur_c++)
	{
		val_s = val_s ? 1 : (*isvalid)(*cur_c);
		if (*cur_c == c && val_s) count++;
	}
	return count;
}

unsigned int makealphabet(char *str)
{
	assert(str != nullptr);

	unsigned int len = strlen(str);
	
	alphabet_ = (letter *)calloc(sizeof(*alphabet_), len + 1);
	{
		letter *cur_let = alphabet_;
		for (char *cur_c = str; *cur_c; cur_let++, cur_c++)
			cur_let->c = *cur_c, cur_let->i = cur_c - str;
	}
	(alphabet_ + len)->c = '\0';
	
	qsort(alphabet_, len, sizeof(*alphabet_), sort_lettercmp);
	
	return len;
}

int isletter(int c)
{
	return getletter(c) + 1;
}

int getletter(char c)
{
	letter alpha = {c, 0};
	letter *res = (letter *) bsearch(&alpha, alphabet_, lenalphabet_, sizeof(alpha), sort_lettercmp);
	if (res == nullptr) return -1;
	return res - alphabet_;
}

int sort_lettercmp(const void *alpha1, const void *alpha2)
{
	assert(alpha1 != nullptr);
	assert(alpha2 != nullptr);
	
	return ((letter *)alpha1)->c - ((letter *)alpha2)->c;
}

int charcmp(char a, char b)
{
	int alpha = getletter(a);
	int beta  = getletter(b);
	if (alpha < 0 &&  beta < 0) return 0;
	if (alpha < 0) return -1;
	if (beta < 0) return 1;
        return alphabet_[alpha].i - alphabet_[beta].i;
}
