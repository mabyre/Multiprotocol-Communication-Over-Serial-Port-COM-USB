/* ===================================================================
   cEnv.h
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   =================================================================== */

#ifndef cEnv_h
#define cEnv_h

/* ------------------------------------------------------------------- */

#include "cXEnv.h"
#include <stdlib.h>
#include <stdio.h> /* For _snprintf */
#include <ctype.h>
#include "cStdArg.h"

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
   extern "C" {
#endif

/* -------------------------------------------------------------------
   Functions mapping.

   ANSI memcpy counterpart. Can be mapped directly if the C Runtime
   library is available and can be used like this:
      #define     c_memcpy memcpy
   ------------------------------------------------------------------- */

/* char  *c_hexdigits(); */
#define c_xhexdigits()           "0123456789ABCDEF"

/* char  *c_strrev(char *aStr); */
#define c_xstrrev             _strrev

/* int   c_isspace(int aChar); */
#define c_isspace             isspace

/* int   c_toupper(int aChar); */
#define c_toupper             toupper
   
/* int   c_strnicmp(const char *aStr1, const char *aStr2, size_t aLength); */
#define c_xstrnicmp           _strnicmp
   
/* int   c_strncmp(const char *aStr1, const char *aStr2, size_t aLength); */
#define c_strncmp             strncmp
   
/* int   c_stricmp(const char *aStr1, const char *aStr2); */
#define c_xstricmp            _stricmp

/* char  *c_strncpymax(char *aStr1, const char *aStr2, size_t aLength, size_t aMax); */
#define c_xstrncpymax         c_xstrncpymax_imp

/* char  *c_strcpymax(char *aStr1, const char *aStr2, size_t aMax); */
#define c_xstrcpymax          c_xstrcpymax_imp

/* char  *c_strncatmax(char *aStr1, const char *aStr2, size_t aLength, size_t aMax); */
#define c_xstrncatmax         c_xstrncatmax_imp

/* char  *c_strcatmax(char *aStr1, const char *aStr2, size_t aMax); */
#define c_xstrcatmax          c_xstrcatmax_imp

/* char  *c_strpbrk(const char *aSrc, const char *aChars); */
#define c_strpbrk             strpbrk

/* char  *c_xstrpbrk(const char *aSrc, const char *aChars, char aDelimiter); */
#define c_xstrpbrk            c_xstrpbrk_imp

/* char  *c_strpbrk(const char *aSrc, const char *aChars); */
#define c_strpbrk             strpbrk

/* int      c_isdigit(int aChar); */
#define c_isdigit             isdigit

/* int      c_isalnum(int aChar); */
#define  c_isalnum            isalnum

/* char  *c_int16toa_imp(pmint16 aValue, char *aStr, size_t aSize); */
#define c_xint16toa           c_xint16toa_imp

/* char  *c_uint16toa_imp(pmuint16 aValue, char *aStr, size_t aSize); */
#define c_xuint16toa          c_xuint16toa_imp

/* char  *c_int32toa_imp(pmint32 aValue, char *aStr, size_t aSize); */
#define c_xint32toa           c_xint32toa_imp

/* char  *c_uint32toa_imp(pmuint32 aValue, char *aStr, size_t aSize); */
#define c_xuint32toa          c_xuint32toa_imp

/* char  *c_strnstr(const char *aStr, size_t aLen, const char *aPattern, size_t *aPos); */
#define c_xstrnstr            c_xstrnstr_imp

/* char  *c_strnchr_imp(const char *aStr, size_t aLen, int aChar); */
#define c_xstrnchr            c_xstrnchr_imp

/* pmbool   c_splitstr(char *aStr, char aSplit, size_t anIndex, size_t *aStart, size_t *anEnd); */
#define c_xsplitstr           c_xsplitstr_imp

/* pmbool   c_splitnstr(char* aStr, size_t aStrLength, char aSplit, size_t anIndex, size_t *aStart, size_t *anEnd); */
#define c_xsplitnstr          c_xsplitnstr_imp

/* pmbool   c_natouint32(const char *aStr, size_t aLength, pmuint32 *anUInt32); */
#define c_xnatouint32         c_xnatouint32_imp

/* pmint32  c_atoint32(const char *aStr); */
#define c_xatoint32           c_xatoint32_imp

/* pmuint32 c_atouint32(const char *aStr); */
#define c_xatouint32          c_xatouint32_imp

/* pmuint32 c_get_tick_count(void); */
#define c_xget_tick_count     GetTickCount

/* void  c_sleep(pmuint32 aMilliSecondPause); */
#define c_xsleep              Sleep

/* int c_snprintf( char *buffer, size_t count, const char *format [, argument] ... ); */
#define c_xsnprintf           _snprintf

/* int _vsnprintf( char *buffer, size_t count, const char *format, va_list argptr ); */
#define c_xvsnprintf          _vsnprintf

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
   }
#endif

/* ------------------------------------------------------------------- */

#endif /* cEnv_h */
