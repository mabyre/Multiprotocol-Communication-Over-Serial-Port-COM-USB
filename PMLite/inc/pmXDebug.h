/*--------------------------------------------------------------------------*\
 * Copyright (c) 1998-2002 AbyreSoft. All rights reserved.
 *--------------------------------------------------------------------------*
 * pmXDebug.h - Written by Bruno Raby.
 * Debugging functions with Unix's style.
\*--------------------------------------------------------------------------*/

#ifndef _PM_XDEBUG_H_
#define _PM_XDEBUG_H_

/*--------------------------------------------------------------------------*/

#include "pmXEnv.h"

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   extern "C" {
#endif

/*--------------------------------------------------------------------------*\
 * Macros pour inserer ou supprimer du code de debug
\*--------------------------------------------------------------------------*/

#define DEBUG_LEVEL 3

#if DEBUG_LEVEL > 0
#   define  PM_CODE0(x) x
#else
#   define  PM_CODE0(x) do { } while (0)
#endif

#if DEBUG_LEVEL > 1
#   define  PM_CODE1(x) x
#else
#   define  PM_CODE1(x) do { } while (0)
#endif

#if DEBUG_LEVEL > 2
#   define  PM_CODE2(x) x
#else
#   define  PM_CODE2(x) do { } while (0)
#endif

/*--------------------------------------------------------------------------*\
   
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

\*--------------------------------------------------------------------------*/

#ifdef PM_USE_TRACES

#pragma message("PM_USE_TRACES - Defined")

#  include "cStdArg.h"

   void DEBUG0_T( char* aFormat, ... );
   void DEBUG0_TL( char* aFormat, ... );
   void DEBUG0_B( char* aFormat, ... );

   void DEBUG1_T( char* aFormat, ... );
   void DEBUG1_TL( char* aFormat, ... );
   void DEBUG1_B( char* aFormat, ... );

   void DEBUG2_T( char* aFormat, ... );
   void DEBUG2_TL( char* aFormat, ... );
   void DEBUG2_B( char* aFormat, ... );

   void DEBUG3_T( char* aFormat, ... );
   void DEBUG3_TL( char* aFormat, ... );
   void DEBUG3_B( char* aFormat, ... );

#  define PM_DEBUG0(X) DEBUG0_ ## X
#  define PM_DEBUG1(X) DEBUG1_ ## X
#  define PM_DEBUG2(X) DEBUG2_ ## X
#  define PM_DEBUG3(X) DEBUG3_ ## X

#else

#pragma message("PM_USE_TRACES - NOT Defined")

/*--------------------------------------------------------------------------*\
 * Avoid traces functions if PM_USE_TRACES is NOT Defined
\*--------------------------------------------------------------------------*/

#  define PM_DEBUG0(X)    ((void) 0)
#  define PM_DEBUG1(X)    ((void) 0)
#  define PM_DEBUG2(X)    ((void) 0)
#  define PM_DEBUG3(X)    ((void) 0)

#endif /* PM_USE_TRACES */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/*--------------------------------------------------------------------------*/

#endif /* _PM_XDEBUG_H_ */
