/* ===========================================================================
   cConsole.h
   Copyright (c) 1996-98 AbyreSoft. All rights reserved.
   ===========================================================================*/

#ifndef cConsole_h
#define cConsole_h

/*--------------------------------------------------------------------------*/

#include "cXTypes.h"

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   extern "C" {
#endif

/*--------------------------------------------------------------------------*\
   Displays data than flush output.
   
   //#### NOT TRUE
   Here is the list of supported display formats:
      
      %%    Display the % character
      %s    Display a 0 terminated string

      %c    Display a character
      %b    Display a character in hexadecimal

      %e    Display a pmerror

      %ld      Display a pmint32
      %lu      Display a pmuint32
      %lx      Display a pmint32 (or pmuint32) in hexadecimal

      %hd      Display a pmint16
      %hu      Display a pmuint16
      %hx      Display a pmint16 (or pmuint16) in hexadecimal

      %t    Display a size_t
      
      %d    Display an int
      %i    Display an int
      %u    Display an unsigned int
      %x    Display an int (or unsigned int) in hexadecimal
      
      %n    Display an end of line
      
      %p    Display a buffer (giving its address and its size as a size_t)
            Example: c_trace(TL("%p", theBuffer, (size_t) theBufferSize)
      
\*--------------------------------------------------------------------------*/
   
void     c_printf(const char* aFormat, ...);

   /* Flush input before getting data. Must not return an empty string. */
char*    c_ngets(char* aString, size_t aSize);

char     c_getc(void);

   /* Returns the ASCII code of the pressed key or 0 if no key is pressed (does not wait). */
char     c_keypressed(void);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/*--------------------------------------------------------------------------*/

#endif
