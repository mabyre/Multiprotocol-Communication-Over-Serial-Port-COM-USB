/* ===================================================================
   cXEnv.h
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   =================================================================== */

#ifndef cXEnv_h
#define cXEnv_h

/* ------------------------------------------------------------------- */
#include <memory.h>
#include <string.h>
#include <stdlib.h>

/* ------------------------------------------------------------------- */
#ifdef __cplusplus
   extern "C" {
#endif

/* ------------------------------------------------------------------- */
/*                   Type definitions                                   */
/* ------------------------------------------------------------------- */
#ifndef PMLITE_TYPES_DEFINED
#  define PMLITE_TYPES_DEFINED

   typedef unsigned char   pmbyte;
   typedef unsigned short  pmuint16;
   typedef unsigned long   pmuint32;
   typedef short           pmint16;
   typedef long            pmint32;
   typedef int             pmbool;

#  define pmfalse    0
#  define pmtrue     1

#  ifndef _SIZE_T_DEFINED
      typedef unsigned int size_t;
#     define _SIZE_T_DEFINED
#  endif

#  define PMLITE_EOL       "\r\n"
#  define PMLITE_EOL_LEN      2

#endif /* PMLITE_TYPES_DEFINED */

/* ------------------------------------------------------------------- */
/*                   Byte ordering                       */
/* ------------------------------------------------------------------- */
/* 
If nothing is defined big endian (network order) is assumed.
PMLITE_LITTLE_ENDIAN must be explicitly defined for this kind of architecture.
*/
#define PMLITE_LITTLE_ENDIAN     1

/* ------------------------------------------------------------------- */
/*                Processor capabilities                    */
/* ------------------------------------------------------------------- */
/*
Define this macro only if the architecture allows accesses to non aligned
data in memory.
*/
#define PMLITE_MEMORY_NON_ALIGNED   1

/* ------------------------------------------------------------------- */
/*             Traces and Asserts conditional use.             */
/* ------------------------------------------------------------------- */
/*
Default is to have traces in DEBUG builds only and no asserts.
*/
#ifndef NDEBUG
#  define PMLITE_TRACE
#endif 
/* 
#define PMLITE_ASSERT   1
*/

/* ------------------------------------------------------------------- */
/*                   Functions mapping.                     */
/* ------------------------------------------------------------------- */
/* 
ANSI memcpy counterpart. Can be mapped directly if the C Runtime
library is available and can be used like this:
#define     c_memcpy memcpy
*/

/* void*    c_memcpy(void *aDest, const void *aSrc, size_t aCount); */
#define     c_memcpy memcpy

/* ANSI memset counterpart */
/* void*    c_memset(void *aDest, int aChar, size_t aCount); */
#define     c_memset memset

/* ANSI memmove counterpart */
/* void*    c_memmove(void *aDest, const void *aSrc, size_t aCount); */
#define     c_memmove   memmove

/* ANSI memcmp counterpart */
/* int         c_memcmp(const void *aBuf1, const void *aBuf2, size_t aCount); */
#define     c_memcmp memcmp

/* ANSI memcmp counterpart */
/* void*    c_memchr(const void *aBuf, int aChar, size_t aCount); */
#define     c_memchr memchr

/* ANSI strcmp counterpart */
/* int         c_strcmp(const char *aString1, const char *aString2); */
#define     c_strcmp strcmp

/* ANSI strcpy counterpart */
/* char*    c_strcpy(char *aDest, const char *aSrc); */
#define     c_strcpy strcpy

/* ANSI strcat counterpart */
/* char*    c_strcat(char *aDest, const char *aSrc); */
#define     c_strcat strcat

/* ANSI strlen counterpart */
/* size_t      c_strlen(const char *aString); */
#define     c_strlen strlen

/* ANSI strchr counterpart */
/* char *      c_strchr(const char *aString, int aChar); */
#define     c_strchr strchr

/* ANSI srand counterpart */
/* void     c_xsrand(pmuint32 aSeed); */
#define c_srand   srand

/* ANSI-like rand() */
/* pmuint32 c_xrand(pmuint32 aMax); */
#define c_xrand   c_xrand_imp

/* pmuint32 c_get_tick_count(void); */
/* __declspec(dllimport) unsigned long __stdcall GetTickCount(void);
*/
#define c_xget_tick_count   GetTickCount

/* ------------------------------------------------------------------- */
/*                   Memory Management                   */
/* ------------------------------------------------------------------- */
#ifndef NDEBUG
#  define PMLITE_MEMORY_DEBUG 1
#endif /* NDEBUG */

#define c_malloc_proc               malloc
#define c_free_proc                 free
#define c_realloc_proc              realloc

/* ------------------------------------------------------------------- */
/*                   Stack Management                    */
/* ------------------------------------------------------------------- */
#ifndef NDEBUG
#  define PMLITE_STACK_DEBUG  1
#endif /* NDEBUG */

/* ------------------------------------------------------------------- */
/*                   Proc Management                        */
/* ------------------------------------------------------------------- */
#ifndef PMLITE_STACK_DEBUG

#  define PMLITE_BEGIN_PROC(x) ((void)0)

#else /* PMLITE_STACK_DEBUG */

#  include "cXStack.h"
#  define PMLITE_BEGIN_PROC(x) c_xstack_update_usage(#x)

#endif /* PMLITE_STACK_DEBUG */

/* ------------------------------------------------------------------- */
/*                   Thread Management                   */
/* ------------------------------------------------------------------- */
#include "cXThread.h"

#define c_xthread_init  c_xthread_init_imp
#define c_xthread_term  c_xthread_term_imp

/* ------------------------------------------------------------------- */
/*                   Macros or Functions                    */
/* ------------------------------------------------------------------- */
/*
There are several situations in which it is possible to use both a macro or
a function to perform the same little operation.
The choice of using a macro is better for speed.
The choice of using a function is better for code size.
By defining this macro, you choose to use macros instead of functions.
By not defining it, you choose to use functions instead of macros.
*/
#define PMLITE_USE_MACROS_FOR_FUNCTIONS      1

/* ------------------------------------------------------------------- */
/*             C Runtime usage for debugging purposes          */
/* ------------------------------------------------------------------- */
/*
Define this macro to use the C Runtime library to test the C-Runtime
standard implementation.
*/
#define PMLITE_USE_LIBC_FOR_TESTS         1

/* ------------------------------------------------------------------- */
#ifdef __cplusplus
   }
#endif

/* ------------------------------------------------------------------- */
#endif /* cXEnv_h */
