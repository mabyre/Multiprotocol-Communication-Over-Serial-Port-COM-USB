/*--------------------------------------------------------------------------*\
 * Copyright (c) 1999-2000 AbyreSoft - All rights reserved.
 *--------------------------------------------------------------------------*
 * pmXEnv.h - Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * Complilations conditionnelles de l'environnement pour la 
 * Protocol Machinerie (PM)
\*--------------------------------------------------------------------------*/

#ifndef _PMXENV_H_
#define _PMXENV_H_

/*--------------------------------------------------------------------------*/

#include <memory.h>
#include <string.h>
#include <stdlib.h>

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   extern "C" {
#endif

/*--------------------------------------------------------------------------*\
 * Conditional use of traces
\*--------------------------------------------------------------------------*/
/*
** Default is to have traces in DEBUG builds only and no asserts.
*/
#ifndef NDEBUG
#  define PM_USE_TRACES 1
#endif 

#ifdef NDEBUG
#  define PM_USE_ASSERTS 1
#endif 

/*--------------------------------------------------------------------------*\
 * Mapping Functions
\*--------------------------------------------------------------------------*/
/* 
** Counterpart functions are directly mapped to the funtions of the 
** C Runtime library if they are available.
** Otherwise you must redirect functions to PM Internal implementations.
*/

/* void*    pm_memcpy(void *aDest, const void *aSrc, size_t aCount); */
#define     pm_memcpy   memcpy

/* void*    pm_memset(void *aDest, int aChar, size_t aCount); */
#define     pm_memset   memset

/* void*    pm_memmove(void *aDest, const void *aSrc, size_t aCount); */
#define     pm_memmove  memmove

/* int      pm_memcmp(const void *aBuf1, const void *aBuf2, size_t aCount); */
#define     pm_memcmp   memcmp

/* void*    pm_memchr(const void *aBuf, int aChar, size_t aCount); */
#define     pm_memchr   memchr

/* int      pm_strcmp(const char *aString1, const char *aString2); */
#define     pm_strcmp   strcmp

/* char*    pm_strcpy(char *aDest, const char *aSrc); */
#define     pm_strcpy   strcpy

/* char*    pm_strcat(char *aDest, const char *aSrc); */
#define     pm_strcat   strcat

/* size_t   pm_strlen(const char *aString); */
#define     pm_strlen   strlen

/* char *   pm_strchr(const char *aString, int aChar); */
#define     pm_strchr   strchr

/* void     pm_xsrand(pmuint32 aSeed); */
#define     pm_srand    srand

/* pmuint32 pm_xrand(pmuint32 aMax); */
#define     pm_xrand    c_xrand_imp

/* pmuint32 c_get_tick_count(void);
** __declspec(dllimport) unsigned long __stdcall GetTickCount(void);
*/
#define     c_xget_tick_count   GetTickCount

/*--------------------------------------------------------------------------*\
 * Memory Management          
\*--------------------------------------------------------------------------*/

#ifndef NDEBUG
#  define PMLITE_MEMORY_DEBUG 1
#endif

#define c_malloc_proc               malloc
#define c_free_proc                 free
#define c_realloc_proc              realloc

/*--------------------------------------------------------------------------*\
 * Proc Management          
\*--------------------------------------------------------------------------*/
/* La macro PM_BEGIN_PROC affiche le nom de la fonction et les stats 
** de la stack
*/

#ifdef PMLITE_STACK_DEBUG

#  include "cXStack.h"

#  define PM_BEGIN_PROC(x) \
          c_xstack_update_usage(#x);\
          PM_DEBUG3(TL("%s", #x ));

#else 

#  define PM_BEGIN_PROC(x) ((void)0)

#endif 

/*--------------------------------------------------------------------------*\
 * Thread Management        
\*--------------------------------------------------------------------------*/

//#include "cXThread.h"

//#define c_xthread_init  c_xthread_init_imp
//#define c_xthread_term  c_xthread_term_imp

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/*--------------------------------------------------------------------------*/

#endif /* _PMXENV_H_ */
