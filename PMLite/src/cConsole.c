/*--------------------------------------------------------------------------*\
 * cConsole.c
 * Copyright (c) 1998-2000 AbyreSoft. All rights reserved.
\*--------------------------------------------------------------------------*/

#include "cConsole.h"

#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <ctype.h>

/*--------------------------------------------------------------------------*/
#define MAX_INPUT_STRING   1024
static   char  gInputString[MAX_INPUT_STRING + 1];

/*--------------------------------------------------------------------------*/
void c_printf(const char* aFormat, ...)
{
   va_list  theArgs;

   va_start(theArgs, aFormat);
   vprintf(aFormat, theArgs);
   va_end(theArgs);

   fflush(stdout);
}

/*--------------------------------------------------------------------------*/
char* c_ngets(char* aString, size_t aSize)
{
   if (aSize == 0 || aString == 0)
      return aString;

   gets(gInputString);
   if (c_strlen(gInputString) + 1 > aSize)
   {
      c_memcpy(aString, gInputString, aSize - 1);
      aString[aSize - 1] = 0;
   }
   else
      c_strcpy(aString, gInputString);

   return aString;
}

/*--------------------------------------------------------------------------*/
pmuint32 c_getu32(void)
{
   char  theString[32], *thePtr, *thePtr2;
   double   theValue;

   for (;;) {
      c_ngets( theString, sizeof(theString) );
      for ( thePtr = theString; *thePtr == ' '; thePtr++ ) {
      }
      for ( thePtr2 = thePtr; isdigit(*thePtr2); thePtr2++ ) {
      }
      if ( thePtr < thePtr2 && *thePtr2 == 0 ) {
         theValue = atof( thePtr );
         if ( 0 <= theValue && theValue <= kPMUINT32_MAX ) {
            return (pmuint32) theValue;
         }
      }
      printf("\n");
   }
   return 0; /* we never really get here */
}

/*--------------------------------------------------------------------------*/
pmuint16 c_getu16(void)
{
   pmuint32 theValue;

   do
   {
      theValue = c_getu32();
   } while (theValue > kPMUINT16_MAX);

   return (pmuint16)theValue;
}

/*--------------------------------------------------------------------------*/
char c_getc(void)
{
   return (char)getch();
}

/*--------------------------------------------------------------------------*/
char c_keypressed(void)
{
   return (char)(_kbhit() ? getch() : 0);
}
