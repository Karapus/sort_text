#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "src/text_processing.h"
#include "src/custom_alphabet.h"
#include "quick-sort/quick_sort.h"

int sort_strcmp(const void *str1, const void *str2);
int sort_rStrCmp(const void *str1, const void *str2);

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

	char *chars = fReadToArr(f_to_sort);
	fclose(f_to_sort);
	
	assert(chars != nullptr);

	makeAlphabet("АаБбВвГгДдЕеЁёЖжЗзИиКкЛлМмНнИиПпРрСсТтУуФфХхЦцЧчШшЩщъыьЭэЮюЯяAaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");

	size_t nlines = 0;
 	String *lines = getLines(chars, isLetter, &nlines);
	
	assert(lines != nullptr);
	
	//qsort(lines, nlines, sizeof(*lines), sort_rStrCmp);
	quickSort(lines, nlines, sizeof(*lines), sort_rStrCmp);

	fPrintArr(f_sorted, lines);
	fclose(f_sorted);

	free(chars);
	chars = nullptr;

	free(lines);
	lines = nullptr;

	freeAlphabet();
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
