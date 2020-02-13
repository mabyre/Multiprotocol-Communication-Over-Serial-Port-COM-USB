/*--------------------------------------------------------------------------*\
 * Copyright (c) 1999-2000 AbyreSoft - All rights reserved.
 *--------------------------------------------------------------------------*
 * pmInterface.h - Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * Interface management functions for Protocol Machinerie (PM)
\*--------------------------------------------------------------------------*/

#ifndef _PM_INTERFACE_H_
#define _PM_INTERFACE_H_

/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "pmEnv.h"

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   extern "C" {
#endif

/*--------------------------------------------------------------------------*\
 * Assert function definition
\*--------------------------------------------------------------------------*/

#ifdef PM_USE_ASSERTS

#define c_assert( condition, message ) \
       if ( !condition )               \
       {                               \
           pm_printf( message ));   \
           assert( condition );        \
       }                               
#else

#define c_assert( a, b ) ((void)0)

#endif /* PM_USE_ASSERT */
       
/*--------------------------------------------------------------------------*\
 * Memory debug Interface Management          
\*--------------------------------------------------------------------------*/

#ifdef PM_USE_MEMORY_DEBUG

#include "cXMemDbg.h"

/* Internal alloc functions definitions for use into cMemDbg */
#define c_malloc_proc    malloc
#define c_free_proc      free
#define c_realloc_proc   realloc

#define pm_malloc(x)     c_malloc_dbg_imp(x, __FILE__, __LINE__)
#define pm_free          c_free_dbg_imp
#define pm_realloc(x, y) c_realloc_dbg_imp(x, y, __FILE__, __LINE__)

#define pm_xmemdbg_dump_state c_xmemdbg_dump_state_imp
#define pm_xmemdbg_set_max    c_xmemdbg_set_max_imp
#define pm_xmemdbg_check      c_xmemdbg_check_imp

#else

#define pm_malloc    malloc
#define pm_free      free
#define pm_realloc   realloc

#define pm_xmemdbg_dump_state()   ((void) 0)
#define pm_xmemdbg_set_max(x)     ((void) 0)
#define pm_xmemdbg_check()        ((void) 0)

#endif

/*--------------------------------------------------------------------------*\
 * Stack debug Interface Management          
\*--------------------------------------------------------------------------*/
#ifdef PM_USE_STACK_DEBUG

#include "cXStack.h"

#  define pm_xstack_reset_usage  c_xstack_reset_usage
#  define pm_xstack_update_usage c_xstack_update_usage
#  define pm_xstack_dump_usage   c_xstack_dump_usage

#else

#  define pm_xstack_reset_usage()   ((void) 0)
#  define pm_xstack_update_usage(x) ((void) 0)
#  define pm_xstack_dump_usage()    ((void) 0)

#endif /* PM_USE_STACK_DEBUG */

/*--------------------------------------------------------------------------*\
 * Console Interface Management          
\*--------------------------------------------------------------------------*/
#ifdef PM_USE_CONSOLE

#include "cConsole.h"

#  define pm_printf    c_printf
#  define pm_ngets     c_ngets

#else

#  define pm_printf    printf
#  define pm_ngets     gets

#endif /* PM_USE_CONSOLE */

/*--------------------------------------------------------------------------*\
 * Thread function management
\*--------------------------------------------------------------------------*/

#ifdef PM_USE_STACK_DEBUG

#  include "cXThread.h"

#  define pm_xthread_term c_xthread_term
#  define pm_xthread_init c_xthread_init

#else

#  define pm_xthread_term() ((void) 0)
#  define pm_xthread_init() ((void) 0)

#endif

/*--------------------------------------------------------------------------*\
 * Core functions use
\*--------------------------------------------------------------------------*/
#ifdef PM_USE_XCORE_FUNCTIONS

#include "cXCore.h"

#define     pm_memcpy       c_memcpy
#define     pm_memset       c_memset
#define     pm_memmove      c_memmove
#define     pm_memcmp       c_memcmp
#define     pm_memchr       c_memchr

#define     pm_strcmp       c_strcmp
#define     pm_strcpy       c_strcpy
#define     pm_strcat       c_strcat
#define     pm_strlen       c_strlen
#define     pm_strchr       c_strchr
#define     pm_xrand        c_xrand

#else /* PM_USE_XCORE_FUNCTIONS */

#define     pm_memcpy       memcpy
#define     pm_memset       memset
#define     pm_memmove      memmove
#define     pm_memcmp       memcmp
#define     pm_memchr       memchr

#define     pm_strcmp       strcmp
#define     pm_strcpy       strcpy
#define     pm_strcat       strcat
#define     pm_strlen       strlen
#define     pm_strchr       strchr
#define     pm_xrand        xrand

#endif /* PM_USE_XCORE_FUNCTIONS */

#ifdef PM_USE_CORE_FUNCTIONS

#include "cCore.h"

#define     pm_hexdigits    c_hexdigits
#define     pm_strrev       c_strrev
#define     pm_isdigit      c_isdigit
#define     pm_isspace      c_isspace
#define     pm_isalnum      c_isalnum
#define     pm_toupper      c_toupper
#define     pm_strncmp      c_strncmp
#define     pm_strnicmp     c_strnicmp
#define     pm_natouint32   c_natouint32
#define     pm_atoint32     c_atoint32
#define     pm_atouint32    c_atouint32
#define     pm_strncmp      c_strncmp
#define     pm_stricmp      c_stricmp
#define     pm_strncpymax   c_strncpymax
#define     pm_strcpymax    c_strcpymax
#define     pm_strncatmax   c_strncatmax
#define     pm_strcatmax    c_strcatmax
#define     pm_strpbrk      c_strpbrk
#define     pm_strpbrk      c_strpbrk
#define     pm_int16toa     c_int16toa
#define     pm_uint16toa    c_uint16toa
#define     pm_int32toa     c_int32toa
#define     pm_uint32toa    c_uint32toa
#define     pm_strnstr      c_strnstr
#define     pm_strnchr      c_strnchr
#define     pm_splitstr     c_splitstr
#define     pm_splitnstr    c_splitnstr
#define     pm_natouint32   c_natouint32
#define     pm_atoint32     c_atoint32
#define     pm_atouint32    c_atouint32

#else /* PM_USE_CORE_FUNCTIONS */

#define     pm_xhexdigits() "0123456789ABCDEF"
#define     pm_xstrrev      _strrev
#define     pm_isdigit      isdigit
#define     pm_isspace      isspace
#define     pm_toupper      toupper
#define     pm_strncmp      _strncmp
#define     pm_strnicmp     _strnicmp
#define     pm_xstricmp     _stricmp

#endif /* PM_USE_CORE_FUNCTIONS */

/*--------------------------------------------------------------------------*\
 * Mapping Functions
\*--------------------------------------------------------------------------*/
/* 
** Counterpart functions are directly mapped to the funtions of the 
** C Runtime library if they are available.
** Otherwise you must create a PM Internal implementations.
*/

/* void srand( unsigned int seed ) */
#define     pm_srand                srand

/* DWORD    GetTickCount(VOID); */
#define     pm_xget_tick_count      GetTickCount

/* VOID     Sleep( DWORD dwMilliseconds // sleep time in milliseconds ); */
#define     pm_xsleep               Sleep

/* int      _snprintf( char *buffer, size_t count, const char *format [, argument] ... ); */
#define     pm_xsnprintf            _snprintf

/* int      _vsnprintf( char *buffer, size_t count, const char *format, va_list argptr ); */
#define     pm_xvsnprintf            _vsnprintf

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/*--------------------------------------------------------------------------*/

#endif /* _PM_INTERFACE_H_ */