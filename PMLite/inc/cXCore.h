/* ===================================================================
   cXCore.h
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   ===================================================================  */

#ifndef cXCore_h
#define cXCore_h

/* ------------------------------------------------------------------- */

#include "cXTypes.h"
#include "cXMem.h"

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
   extern "C" {
#endif

/* ------------------------------------------------------------------- */

   /* Module identifier */
#define pmM_Core  1

   /* Trace codes */
#define pmT_Core     PMTraceDefine(pmM_Core, 0)
#define pmT_Core_Stack  PMTraceDefine(pmM_Core, 1)

/* -------------------------------------------------------------------
   Standard Posix compatible implementations.
   ------------------------------------------------------------------- */

void  *c_memcpy_imp(void *aDest, const void *aSrc, size_t aCount);
void  *c_memset_imp(void *aDest, int aChar, size_t aCount);
void  *c_memmove_imp(void *aDest, const void *aSrc, size_t aCount);
int      c_memcmp_imp(const void *aBuf1, const void *aBuf2, size_t aCount);
void  *c_memchr_imp(const void *aBuf, int aChar, size_t aCount);

int      c_strcmp_imp(const char *aString1, const char *aString2);
char  *c_strcpy_imp(char *aDest, const char *aSrc);
char  *c_strcat_imp(char *aDest, const char *aSrc);
size_t   c_strlen_imp(const char *aString);
char  *c_strchr_imp(const char *aString, int aChar);

void     c_srand_imp(pmuint32 aSeed);
pmuint32 c_xrand_imp(pmuint32 aMax);

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
   }
#endif

/* ------------------------------------------------------------------- */

#endif

