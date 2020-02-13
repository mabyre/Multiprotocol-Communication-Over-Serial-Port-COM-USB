/* ===================================================================
   cCore.h
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   =================================================================== */

#ifndef cCore_h
#define cCore_h

/* ------------------------------------------------------------------- */

#include "cXTypes.h"
#include "cEnv.h"
#include "cXEnv.h"
#include "cXCore.h"

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
   extern "C" {
#endif

/* -------------------------------------------------------------------
   Additional common implementations.
   ------------------------------------------------------------------- */

char  *c_xhexdigits_imp();

char  *c_xstrrev_imp(char *aStr);

int   c_isspace_imp(int aChar);

int      c_isalnum_imp(int aChar);

int   c_toupper_imp(int aChar);

int      c_xstricmp_imp(const char *aStr1, const char *aStr2);

int      c_strncmp_imp(const char *aStr1, const char *aStr2, size_t aLength);

int      c_xstrnicmp_imp(const char *aStr1, const char *aStr2, size_t aLength);

char  *c_xstrncpymax_imp(char *aStr1, const char *aStr2, size_t aLength, size_t aMax);

char  *c_xstrcpymax_imp(char *aStr1, const char *aStr2, size_t aMax);

char  *c_xstrncatmax_imp(char *aStr1, const char *aStr2, size_t aLength, size_t aMax);

char  *c_xstrcatmax_imp(char *aStr1, const char *aStr2, size_t aMax);

char  *c_strpbrk_imp(const char *aSrc, const char *aChars);

char  *c_xstrpbrk_imp(const char *aSrc, const char *aChars, char aDelimiter);

int      c_isdigit_imp(int aChar);

char  *c_xstrnchr_imp(const char *aStr, size_t aLen, int aChar);

char  *c_xstrnstr_imp(const char *aStr, size_t aLen, const char *aPattern, size_t *aPos);

/* -------------------------------------------------------------------
   About the next 4 functions:
   aSize must be the buffer size, not the string max size that is 1 less.
   If the string generated is bigger than the buffer 0 is returned and
   a trace is emited.
   If you use a buffer whose size is greater or equal to 12, then you 
   are sure that the string will fit inside the buffer and you can ignore
   the return value that will never be 0.
   12 is 10 digits, minus sign and leading 0.
   11 is enough for unsigned values.
   ------------------------------------------------------------------- */
   
char  *c_xint16toa_imp(pmint16 aValue, char *aStr, size_t aSize);

char  *c_xuint16toa_imp(pmuint16 aValue, char *aStr, size_t aSize);

char  *c_xint32toa_imp(pmint32 aValue, char *aStr, size_t aSize);

char  *c_xuint32toa_imp(pmuint32 aValue, char *aStr, size_t aSize);

/* ------------------------------------------------------------------- */

pmbool   c_xsplitstr_imp(char *aStr, char aSplit, size_t anIndex, size_t *aStart, size_t *anEnd);

pmbool   c_xsplitnstr_imp(char* aStr, size_t aStrLength, char aSplit, size_t anIndex, size_t *aStart, size_t *anEnd);

/* ------------------------------------------------------------------- */

pmbool      c_xnatouint32_imp(const char *aStr, size_t aLength, pmuint32 *anUInt32);

pmint32     c_xatoint32_imp(const char *aStr);

pmuint32 c_xatouint32_imp(const char *aStr);

/* ------------------------------------------------------------------- */

   /* Returns the current value of elapsed milliseconds. */
pmuint32 c_xget_tick_count_imp(void);

   /* Waits for the specified time in milliseconds.   */
void     c_xsleep_imp(pmuint32 aMilliSecondPause);

/* -------------------------------------------------------------------
   Min and Max.
   ------------------------------------------------------------------- */

#define c_min(a,b)      ((a) < (b) ? (a) : (b))
#define c_max(a,b)      ((a) > (b) ? (a) : (b))

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
   }
#endif

/* ------------------------------------------------------------------- */

#endif
