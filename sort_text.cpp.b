#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

struct fstricmp_context
{
	FILE *f;
	char *str1;
	char *str2;
	int strl;
} context;

int getclines(FILE *f, int *maxstrp);       
int fsearch(FILE *f, char c = '\n');
char *getstrfrompos(FILE *f, fpos_t *pos, char *str, int strl);
int fstrcmp(const void *pos1, const void *pos2);

char *strbuf1 = NULL;
char *strbuf2 = NULL;

int main(int argc, char** argv)
{
	FILE *f_to_sort  = (argc > 1) ? fopen(argv[argc - 2], "r") : stdin;
	FILE *f_sorted   = (argc > 1) ? fopen(argv[argc - 1], "w") : stdout;
      	int maxstr = 0;
	int clines = getclines(f_to_sort, &maxstr);
	fpos_t *pos_arr = (fpos_t *)malloc(sizeof(*pos_arr) * clines);
	for(int i = 0; i < clines; i++)
	{
		fsearch(f_to_sort,'\n');
		fgetpos(f_to_sort, pos_arr + i);
	}
	
	context.f = f_to_sort;
	context.str1 = strbuf1;
	context.str2 = strbuf2;
	context.strl = maxstr;
	qsort(pos_arr, clines, sizeof(*pos_arr), fstrcmp);

	strbuf1 = (char *)malloc(sizeof(*strbuf1) * maxstr);
	strbuf2 = (char *)malloc(sizeof(*strbuf2) * maxstr);
	for(int i = 0; i < clines; i++)
		fprintf(f_sorted, "%s", getstrfrompos(f_to_sort, pos_arr + i, strbuf1, maxstr));
	
	printf("lines %d, max %d\n", clines, maxstr);
}

int getclines(FILE *f, int *maxstrp)
{
	int curstr, clines;
	curstr = clines = *maxstrp = 0;
	while(!feof(f))
	{
		curstr++;
		if(fgetc(f) == '\n')
		{	
			clines++;
			*maxstrp = (curstr > *maxstrp) ? curstr : *maxstrp;
			curstr = 0;
		}

	}
	rewind(f);
	(*maxstrp)++;
	return clines;
}

int fsearch(FILE *f, char c/* = '\n'*/)
{
	for(int i = fgetc(f); i != c; i = fgetc(f))
		putchar(i);
	return 0;
}

char *getstrfrompos(FILE *f, fpos_t *pos, char *str, int strl)
{
	fsetpos(f, pos);
	fgets(str, strl, f);
	return str;
}

int fstrcmp(const void *pos1,const void *pos2)
{
	fpos_t *tpos1 = (fpos_t *)pos1;
	fpos_t *tpos2 = (fpos_t *)pos2;
	context.str1 = getstrfrompos(context.f, tpos1, context.str1, context.strl);
	context.str2 = getstrfrompos(context.f, tpos2, context.str2, context.strl);
	return strcmp(context.str1, context.str2);
}
