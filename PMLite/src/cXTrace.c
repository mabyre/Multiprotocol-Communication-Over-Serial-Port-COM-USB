/* ===================================================================
   cXTrace.c
   Copyright (C) 1990 - 2000 AbyreSoft - All rights reserved.
   ===================================================================  */

#include "cXTrace.h"

#ifdef PMLITE_TRACE

/* ------------------------------------------------------------------- */

#include "ASTrace.h"

/* ------------------------------------------------------------------- */

void c_trace_init( void *anInstance /*HINSTANCE anInstance*/ )
{
   as_trace_init( anInstance );
}

/* ------------------------------------------------------------------- */

void c_trace_close( void )
{
   as_trace_close();
}

/* ------------------------------------------------------------------- */

void c__trace_T(char* aFormatString, ...)
{
   va_list  theArgs;

   va_start(theArgs, aFormatString);
   as_trace_VT(0, aFormatString, &theArgs);
   va_end(theArgs);
}

/* ------------------------------------------------------------------- */

void c__trace_VT(char* aFormatString, va_list* anArgs)
{
   as_trace_VT(0, aFormatString, anArgs);
}

/* ------------------------------------------------------------------- */

void c__trace_TL(char* aFormatString, ...)
{
   va_list  theArgs;

   va_start(theArgs, aFormatString);
   as_trace_VTL(0, aFormatString, &theArgs);
   va_end(theArgs);
}

/* ------------------------------------------------------------------- */

void c__trace_VTL(char* aFormatString, va_list* anArgs)
{
   as_trace_VTL(0, aFormatString, anArgs);
}

/* ------------------------------------------------------------------- */

void c__trace_B(pmbyte* aBuffer, size_t aBufferLen)
{
   as_trace_B(0, aBuffer, aBufferLen);
}

/* ------------------------------------------------------------------- */

void c_trace_T(pmuint16 aModule, char* aFormatString, ...)
{
   va_list  theArgs;

   va_start(theArgs, aFormatString);
   as_trace_VT(aModule, aFormatString, &theArgs);
   va_end(theArgs);
}

/* ------------------------------------------------------------------- */

void c_trace_VT(pmuint16 aModule, char* aFormatString, va_list* anArgs)
{
   as_trace_VT(aModule, aFormatString, anArgs);
}

/* ------------------------------------------------------------------- */

void c_trace_TL(pmuint16 aModule, char* aFormatString, ...)
{
   va_list  theArgs;

   va_start(theArgs, aFormatString);
   as_trace_VTL(aModule, aFormatString, &theArgs);
   va_end(theArgs);
}

/* ------------------------------------------------------------------- */

void c_trace_VTL(pmuint16 aModule, char* aFormatString, va_list* anArgs)
{
   as_trace_VTL(aModule, aFormatString, anArgs);
}

/* ------------------------------------------------------------------- */

void c_trace_B(pmuint16 aModule, pmbyte* aBuffer, size_t aBufferLen)
{
   as_trace_B(aModule, aBuffer, aBufferLen);
}

/* ------------------------------------------------------------------- */

#endif /* PMLITE_TRACE */
