/*--------------------------------------------------------------------------*\
 * Copyright (c) 1998-2002 AbyreSoft. All rights reserved.
 *--------------------------------------------------------------------------*
 * pmDebug.h - Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * Ajouter la trace pour des flottants
 * Tracing functions.
\*--------------------------------------------------------------------------*/

#ifndef _PM_TRACE_H_
#define _PM_TRACE_H_

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   extern "C" {
#endif

/*--------------------------------------------------------------------------*/

#ifdef PM_USE_TRACES

#  include "windows.h"

    #define MODULE_DEBUG0 0x2000
    #define MODULE_DEBUG1 0x2001
    #define MODULE_DEBUG2 0x2002
    #define MODULE_DEBUG3 0x2003

    #define MODULE_ERROR0 0x0100
    #define MODULE_ERROR1 0x0101
    #define MODULE_ERROR2 0x0102
    #define MODULE_ERROR3 0x0103

    #define MODULE_TRACE0 0x1000
    #define MODULE_TRACE1 0x1001
    #define MODULE_TRACE2 0x1002
    #define MODULE_TRACE3 0x1003

	void pm_trace(WORD aModule, char *aFormatString, ...);
	void pm_trace0( char *aFormatString, ... );
	void pm_trace0(char *aFormatString, ...);
	void pm_trace1(char *aFormatString, ...);
	void pm_trace2(char *aFormatString, ...);
	void pm_trace3(char *aFormatString, ...);

#pragma message("PM_USE_TRACES - Defined")

#define PM_TRACE    pm_trace
#define PM_TRACE0   pm_trace0
#define PM_TRACE1   pm_trace1
#define PM_TRACE2   pm_trace2
#define PM_TRACE3   pm_trace3

#else

#pragma message("PM_USE_TRACES - NOT Defined")

/*--------------------------------------------------------------------------*\
 * Avoid traces functions if PM_USE_TRACES is NOT Defined
\*--------------------------------------------------------------------------*/

#  define PM_TRACE0(X)    ((void)0)
#  define PM_TRACE1(X)    ((void)0)
#  define PM_TRACE2(X)    ((void)0)
#  define PM_TRACE3(X)    ((void)0)

#endif /* PM_USE_TRACES */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/*--------------------------------------------------------------------------*/

#endif /* _PM_TRACE_H_ */
