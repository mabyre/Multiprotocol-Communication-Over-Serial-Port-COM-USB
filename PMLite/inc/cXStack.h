/* ===================================================================
   cXStack.h
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   ===================================================================  */

#ifndef cXStack_h
#define cXStack_h

/* ------------------------------------------------------------------- */

#include "cXTypes.h"

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------- */

#ifdef PMLITE_STACK_DEBUG

typedef struct _TStackInfo
{
   size_t   itsBase;
   size_t   itsMax;
   pmbool   itsfReset;
   char* itsProcName;
} TStackInfo;

/* 
This must be implemented for each platform.
This should take multiple threads into account and return a per thread structure.
Must set itsfReset to pmfalse the first time it is called. This is used to avoid
stack measurement before c_stack_reset_usage_imp is called.
*/

TStackInfo* c_xstack_get_info_imp(void);

void     c_xstack_reset_usage_imp(void);
void     c_xstack_update_usage_imp(char* aProcName);
void     c_xstack_dump_usage_imp(void);

#endif   /* PMLITE_STACK_DEBUG */

/* ------------------------------------------------------------------- */

#ifdef PMLITE_STACK_DEBUG

#  pragma message("PMLITE_STACK_DEBUG - Defined")

#  define c_xstack_reset_usage     c_xstack_reset_usage_imp
#  define c_xstack_update_usage    c_xstack_update_usage_imp
#  define c_xstack_dump_usage      c_xstack_dump_usage_imp

#else

#  pragma message("PMLITE_STACK_DEBUG - NOT Defined")

#  define c_xstack_reset_usage()      ((void) 0)
#  define c_xstack_update_usage(x) ((void) 0)
#  define c_xstack_dump_usage()    ((void) 0)

#endif   /* PMLITE_STACK_DEBUG */

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------- */

#endif
