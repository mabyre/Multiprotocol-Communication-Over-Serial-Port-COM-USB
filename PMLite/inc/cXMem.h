/* ===================================================================
   cXMem.h
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   ===================================================================  */

#ifndef cXMem_h
#define cXMem_h

/* ------------------------------------------------------------------- */

#include "cXTypes.h"

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------
   This is the implementation of the debug memory.
   It links all allocated blocks altogether in a list.
   c_mem_state traces the current allocated blocks as well as some statistics.
   c_set_max_mem allows to specify the maximum amount of memory to use.
   When trying to allocate more than this c_malloc returns 0.
   It helps simulating low memory conditions.
   The new allocated blocks are preceded and followed by 4 bytes with the value
   0xFD. This allows to find memory misuse.
   The data part of a new allocated block if filled with 0xCD.
   When the block is freed the data part of the block is filled with 0xDD.

   Don't call these functions directly but through use of the macro with same name
   minus trailing _dbg_imp. Those macros are removed in release mode.
   ------------------------------------------------------------------- */

#ifdef PMLITE_MEMORY_DEBUG

void  *c_malloc_dbg_imp(size_t aSize, const char* aFileName, int aLine);
void  *c_realloc_dbg_imp(void* aBlock, size_t aSize, const char* aFileName, int aLine);
void  c_free_dbg_imp(void* aBlock);

void  c_xmemdbg_dump_state_imp(void);
void  c_xmemdbg_set_max_imp(size_t aSize);
void  c_xmemdbg_check_imp(void);

#endif   /* PMLITE_MEMORY_DEBUG  */

/* ------------------------------------------------------------------- */

#ifdef PMLITE_MEMORY_DEBUG

   #define c_malloc(x)              c_malloc_dbg_imp(x, __FILE__, __LINE__)
   #define c_free                c_free_dbg_imp
   #define c_realloc(x, y)          c_realloc_dbg_imp(x, y, __FILE__, __LINE__)

   #define c_xmemdbg_dump_state     c_xmemdbg_dump_state_imp
   #define c_xmemdbg_set_max        c_xmemdbg_set_max_imp
   #define c_xmemdbg_check          c_xmemdbg_check_imp

#else /* PMLITE_MEMORY_DEBUG  */

   #define c_malloc              c_malloc_proc
   #define c_free                c_free_proc
   #define c_realloc             c_realloc_proc

   #define c_xmemdbg_dump_state()      ((void) 0)
   #define c_xmemdbg_set_max(x)     ((void) 0)
   #define c_xmemdbg_check()        ((void) 0)

#endif   /* PMLITE_MEMORY_DEBUG  */

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------- */

#endif
