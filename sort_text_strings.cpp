#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct string
{
	char* s;
	size_t l;
};

FILE *fopen_prot(const char *name, const char *mode, const char *err);
string *getlines(char *arr, int *nlinesp = NULL);      
char *freadtoarr(FILE *f, long *size = NULL);
long getfsize(FILE *f);
int centries(char *str, int (*isvalid)(int), char c = '\n');
int sort_strcmp(const void *str1, const void *str2);
int sort_rstrcmp(const void *str1, const void *str2);
void fprintarr(FILE *f, string *arr);

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

	char *chars = freadtoarr(f_to_sort);
	fclose(f_to_sort);
	
	int nlines = 0;
	string *lines = getlines(chars, &nlines);
	
	qsort(lines, nlines, sizeof(*lines), sort_rstrcmp);

	fprintarr(f_sorted, lines);
	fclose(f_sorted);

	free(chars);
	free(lines);
}

/*!	function to open file. If error occurse, prints message
 * 	@param name	- file name
 * 	@param mode	- file open mode
 * 	@param err	- error message
 */
FILE *fopen_prot(const char *name, const char *mode, const char *err)
{
	FILE *f  = fopen(name, mode);
	if (!f)
	{
		perror(err);
	}
	return f;
}

/*!	@brief prints strings from null terminated char** array into file
 * 	@param f - file to write to
 * 	@param arr - array to write
 */
void fprintarr(FILE *f, string *arr)
{
	for(string *str = arr; str->s; str++)
	{
		fputs(str->s,	f);
		fputc('\n',	f);
	}
}

/*!	@brief reads file to char array
 *	@param f - file to read
 *	@param size - pointer to long to write size in bytes, if specified
 *	@note default value of size = NULL
 *	@return pointer to null terminated string of file content
 */
char *freadtoarr(FILE *f, long *size)
{
	long nchars = getfsize(f);	
	char *chars = (char *)calloc(sizeof(char), nchars + 1);
	nchars = fread(chars, sizeof(char), nchars, f);
	chars[nchars] = '\0';

	if (size) *size = nchars;

	return chars;
}

/*!	@brief returns size of file in bytes
 * 	@param f - file to mesure
 * 	@return size of file f in bytes
 */
long getfsize(FILE *f)
{
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
	return strcmp(((string *)str1)->s, ((string *)str2)->s);
}

/*!	@brief comparator function for reversed comparing strings from array
 * 	@param str1 - pointer to string 1
 * 	@param str2 - pointer to string 2
 * 	@return result of comparing reversed strings without trailing non-alphabet symbols
 */
int sort_rstrcmp(const void *str1, const void *str2)
{
	string s1 = *((string *)str1);
	string s2 = *((string *)str2);

	long i1 = (long) s1.l - 1;
	while (ispunct(s1.s[i1]) || isspace(s1.s[i1]) || iscntrl(s1.s[i1])) i1--;

	long i2 = (long) s2.l - 1;
	while (ispunct(s2.s[i2]) || isspace(s2.s[i2]) || iscntrl(s2.s[i2]))  i2--;

	for (; i1 >= 0 && i2 >= 0; i1--, i2--)
	{	
		if (s1.s[i1] != s2.s[i2]) return s1.s[i1] - s2.s[i2];
	}
	if (i1 == i2) return 0;
	if (i1) return s1.s[i1];
	return s2.s[i2];
}

/*!	@brief converts every '\\n' in given string into '\0' and returns array of strings
 * 	@param arr	- string to operate with
 * 	@param nlinesp	- pointer to int to save number of strings, if specified
 * 	@note default value of nlinesp = NULL
 * 	@return pointer to null terminated array of strings
 */
string *getlines(char *arr, int *nlinesp)
{
	int linectr = centries(arr, isalpha);
	string *lines = (string *)calloc(sizeof(string), linectr + 1);
	linectr = 0;
	lines[0].s = arr;
	int val_s = 0;
	for (char *s = arr; *s != '\0'; s++)
	{
		val_s ? val_s : isalpha(*s);
		if (*s == '\n')
		{
			lines[++linectr].s = ++s;
			lines[linectr - 1].l = lines[linectr].s - lines[linectr - 1].s - 1;
			*(s - 1) = '\0';
			/*printf("getlines() L72:\n %d \\n;\nPrevious string:\n\"%s\"\nCurrent string:\n\"%s\"\n",
				linectr, lines[linectr - 1].s, lines[linectr].s);*/

		}
	}
	lines[linectr].s = NULL;

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
	int count = 0;
	int val_s = 0;
	for (char *cur_c = str; *cur_c; cur_c++)
	{
		val_s ? val_s : (*isvalid)(*cur_c);
		if (*cur_c == c && val_s) count++;
	}
	return count;
}
