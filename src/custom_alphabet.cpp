#include "custom_alphabet.h"

static Letter *alphabet_;
static unsigned int lenalphabet_;

/*!
 *      @brief makes alpabet of letters from string
 *      @param str      - string to make alphabet from
 *      @return length of alphabet
 *
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
        
        return lenalphabet_ = len;
}

void freeAlphabet()
{
	free(alphabet_);
	lenalphabet_ = 0;
	alphabet_ = nullptr;
	return;
}

int isLetter(const char *c)
{
        return getLetter(c) + 1;
}

/*!     
 *      @brief get index of alphabet_ corresponding to c
 *      @param c        char to find
 *      @return index of alphabet_
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
 *      @brief function to sort alphabet
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
 *      @brief comparse chars by alphabet
 *      @param a first char;
 *      @param b second char;
 *      @return > 0 if a > b, 0 if a = b, < 0 if a < b
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
