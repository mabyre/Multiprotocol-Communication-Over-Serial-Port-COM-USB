/*--------------------------------------------------------------------------*\
 * Copyright (c) 1998-2000 AbyreSoft - All rights reserved.
 *--------------------------------------------------------------------------*
 * cXTrace.h - Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * Differentes facons de faire des traces
\*--------------------------------------------------------------------------*/

#ifndef cXTrace_h
#define cXTrace_h

/* -------------------------------------------------------------------
   This file contains all the declarations and macros required to use traces 
   and assertions.   
   ------------------------------------------------------------------- */

#include "cXEnv.h"
#include "cXTypes.h"

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
   extern "C" {
#endif

/* -------------------------------------------------------------------
   Traces:
   
   Traces are only emitted if PMLITE_TRACE is defined. In fact all the code relative
   to trace is generated only if PMLITE_TRACE is defined.
   
   A trace identifier is associated to each trace. A trace identifier is made of:
      - a module identifier (the same that is used for error)
      - a granularity level: a value from 0 to 255
   
   The trace identifier can be used to filter trace display. The implementation of 
   filtering is platform dependant.
   
   A trace is emited using one of the following macro:
   
      - For a line not terminated by a carriage return:
         c_trace(T(pmT_Module, "printf-like format string", ...);
      
      - For a line terminated by a carriage return:
         c_trace(TL(pmT_Module, "printf-like format string", ...);
      
      - For a binary buffer dumped in hexadecimal:
         c_trace(B(pmT_Module, aBufferPtr, aBufferLen);


   The format of a trace directive is :
      % flags width format
         
   Supported flags are:
      
      -     Display left justified (default is right justified)
      +     Always displays the sign symbol (default is to diplay only '-' symbol)
      space Displays a space (when the value is positive) instead of a '+' symbol
      #     Alternate form specifier
      
   Width should be a positive number

   Here is the list of supported format:
      
      %     Display the % character
      s     Display a 0 terminated string

      c     Display a character

      ld    Display a pmint32
      lu    Display a pmuint32      
      lX lx Display a pmint32 (or pmuint32) in hexadecimal

      hd    Display a pmint16
      hu    Display a pmuint16      
      hX hx Display a pmint16 (or pmuint16) in hexadecimal

      d     Display an int
      i     Display an int
      u     Display an unsigned int    
      X  x  Display an int (or unsigned int) in hexadecimal
   
   Not supported formats are:
   
      o     Display an unsigned int in octal
      
      f     Display a double
      e     Display a double
      E     Display a double
      F     Display a double
      g     Display a double
      G     Display a double
      
      p     Display a pointer to void
      
      L
      n

   ------------------------------------------------------------------- */

#ifdef PMLITE_TRACE

#pragma message("PMLITE_TRACE - Defined")

#  include "cStdArg.h"

   void c_trace_init( void *anInstance ) ;
   void c_trace_close( void ) ;
   
   void c__trace_T(char* aFormat, ...);
   void c__trace_VT(char* aFormat, va_list* anArgs);
   void c__trace_VTL(char* aFormat, va_list* anArgs);
   void c__trace_TL(char* aFormat, ...);
   void c__trace_B(pmbyte* aBuffer, size_t aBufferLen);

#  define c_trace0(X)   c__trace_ ## X

   void c_trace_VT(pmuint16 aModule, char* aFormat, va_list* anArgs);
   void c_trace_T(pmuint16 aModule, char* aFormat, ...);
   void c_trace_VTL(pmuint16 aModule, char* aFormat, va_list* anArgs);
   void c_trace_TL(pmuint16 aModule, char* aFormat, ...);
   void c_trace_B(pmuint16 aModule, pmbyte* aBuffer, size_t aBufferLen);

#  define c_trace(X) c_trace_ ## X

#else

#pragma message("PMLITE_TRACE - NOT Defined")

#  define c_trace_init(X) ((void) 0)
#  define c_trace_close() ((void) 0)

#  define c_trace0(X)     ((void) 0)
#  define c_trace(X)      ((void) 0)

#endif /* PMLITE_TRACE */

/* Macro used to define trace identifiers */
#define PMTraceDefine(x, y)   ((pmuint16)(((x) << 8) + (y)))

/* -------------------------------------------------------------------
   Function used to display traces. 
   This function is for internal use only and should never be called directly
   because some implementations may not use it.
   ------------------------------------------------------------------- */

void c_do_trace_str(pmuint16 aModule, const char *aStr, size_t aStrLen);

/* -------------------------------------------------------------------
   Assertion macros

   Assertion are only checked if PMLITE_ASSERT is defined. In fact all the code 
   relative to assertions is generated only if PMLITE_ASSERT is defined.
   
   Each platform has its own assertion handler. When an assertion is raised the 
   program stops. 
   The implementation of assertion signaling is platform dependant.
   PMLITE_TRACE affects the level of information assertion handler can have.
   If PMLITE_TRACE is defined, the function implementing assertion handling can 
   access the file name, the line number, the condition and the message.
   If PMLITE_TRACE is not defined, the function implementing assertion handling 
   has no information.
   
   The c_assert_param macro must be used to check for null parameters.
   ------------------------------------------------------------------- */

#ifdef PMLITE_ASSERT

#  ifdef PMLITE_TRACE

#     define c_assert(condition, message)                            \
      do                                                       \
      {                                                        \
         if (!(condition))                                        \
            c_do_assert(__FILE__, __LINE__, #condition, "ASSERT: " #message); \
      } while (0)

#  else /* PMLITE_TRACE */

#     define c_assert(condition, message)                \
      do                                           \
      {                                            \
         if (!(condition))                            \
            c_do_assert(0, 0, 0, 0);                     \
      } while (0)

#  endif /* PMLITE_TRACE */

#else /* PMLITE_ASSERT */

#  define c_assert(condition, message) ((void) 0)

#endif /* PMLITE_ASSERT */


   /* Macro to use to check for null parameters */
#define c_assert_param(param, function)   \
   c_assert(param != 0, function "(): Parameter [" #param "] cannot be 0.")
   
/* -------------------------------------------------------------------
   Function used to display assertions. 
   This function is for internal use only and should never be called directly
   because some implementations may not use it.
   ------------------------------------------------------------------- */

void c_do_assert(char* aFile, int aLine, char* aCondition, char* aMessage);

/* -------------------------------------------------------------------
   User trace Modules available for applications
   to be linked with reporter's ressources
   ------------------------------------------------------------------- */

#define APP0  0x8000
#define APP1  0x8100
#define APP2  0x8200
#define APP3  0x8300

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------   */

#endif
