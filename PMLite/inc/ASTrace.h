/*--------------------------------------------------------------------------*\
 * Copyright (c) 1998-2000 AbyreSoft - All rights reserved.
 *--------------------------------------------------------------------------*
 * ASTrace.h - AS DLL Trace interface
\*--------------------------------------------------------------------------*/

#ifndef ASTrace_h
#define ASTrace_h

/*--------------------------------------------------------------------------*/

#include "OSWinInc.h"

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

#include <stdarg.h>

/*--------------------------------------------------------------------------*/

#ifdef ASTRACEDLL
#define ASTRACEAPI
#else
#define ASTRACEAPI __declspec(dllimport)
#pragma comment(lib, "ASTRACE")
#endif

/*--------------------------------------------------------------------------*\
 * T  for a line not terminated by a carriage return
 * TL for a line terminated by a carriage return
 * B  for a binary buffer dumped in hexadecimal
\*--------------------------------------------------------------------------*/

ASTRACEAPI void __stdcall as_trace_init(HINSTANCE anInstance);
ASTRACEAPI void __stdcall as_trace_close(void);

/*--------------------------------------------------------------------------*/

ASTRACEAPI void __stdcall as_trace_T(WORD aModule, char* aFormatString, ...);
ASTRACEAPI void __stdcall as_trace_VT(WORD aModule, char* aFormatString, va_list* anArgs);
ASTRACEAPI void __stdcall as_trace_TL(WORD aModule, char* aFormatString, ...);
ASTRACEAPI void __stdcall as_trace_VTL(WORD aModule, char* aFormatString, va_list* anArgs);
ASTRACEAPI void __stdcall as_trace_B(WORD aModule, BYTE* aBuffer, size_t aBufferLen);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*--------------------------------------------------------------------------*/
#endif /* ASTrace_h */
