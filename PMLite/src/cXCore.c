/* ===================================================================
   cXCore.c
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   ===================================================================  */

#include "cXCore.h"
#include "cXTrace.h"

/* ------------------------------------------------------------------- */

void *c_memcpy_imp(void *aDest, const void *aSrc, size_t aCount)
{
   const char  *theSrcPtr;
   char     *theDestPtr;

   PMLITE_BEGIN_PROC(c_memcpy_imp);

   for (theSrcPtr = aSrc, theDestPtr = aDest, aCount++; --aCount;)
      *theDestPtr++ = *theSrcPtr++;

   return aDest;
}

/* ------------------------------------------------------------------- */

void* c_memset_imp(void *aDest, int aChar, size_t aCount)
{
   char  *theDestPtr = aDest;
   
   PMLITE_BEGIN_PROC(c_memset_imp);

   while (aCount-- != 0)
      *(theDestPtr)++ = (char) aChar;
      
   return aDest;
}

/* ------------------------------------------------------------------- */

void* c_memmove_imp(void *aDest, const void *aSrc, size_t aCount)
{
   const char  *theSrcPtr;
   char     *theDestPtr;

   PMLITE_BEGIN_PROC(c_memmove_imp);

   if ((void*) aSrc > aDest)
   {
      for (theSrcPtr = aSrc, theDestPtr = aDest, aCount++; --aCount;)
         *theDestPtr++ = *theSrcPtr++;
   }
   else
   {
      for (theSrcPtr = (const char*) aSrc + aCount, theDestPtr = (char*) aDest + aCount, aCount++; --aCount;)
         *--theDestPtr = *--theSrcPtr;
   }
   
   return aDest;
}

/* -------------------------------------------------------------------
   Compare the first 'aCount' bytes of 'aBuffer1' with the first 'aCount' bytes 
   of 'aBuffer2'. Retruns an int that is greater than, equal to, or less than 
   zero according to the relative order of 'aBuffer1' and 'aBuffer2':
      
      returns < 0 if aBuffer1 < aBuffer2
      returns  0  if aBuffer1 == aBuffer2
      returns > 0 if aBuffer1 > aBuffer2
      
   WARNING: This function has never been tested
   ------------------------------------------------------------------- */

int c_memcmp_imp(const void *aBuffer1, const void *aBuffer2, size_t aCount)
{
   PMLITE_BEGIN_PROC(c_memcmp_imp);

   if (aCount == 0)
      return 0;

    while ((--aCount != 0) && ((*(char*) aBuffer1) == (*(char*) aBuffer2))) 
    {
        aBuffer1 = (char*) aBuffer1 + 1;
        aBuffer2 = (char*) aBuffer2 + 1;
    }

   return (*((unsigned char*) aBuffer1)) - (*((unsigned char*) aBuffer2));
}

/* ------------------------------------------------------------------- */

void *c_memchr_imp(const void *aBuffer, int aChar, size_t aCount)
{
   const unsigned char  *thePtr;
   unsigned char        theChar = (unsigned char) aChar;

   PMLITE_BEGIN_PROC(c_memchr_imp);

   for (thePtr = (unsigned char *) aBuffer, aCount++; --aCount;)
      if (*thePtr++ == theChar)
         return (void *) (thePtr - 1);

   return 0;
}

/* ------------------------------------------------------------------- */

int c_strcmp_imp(const char *aString1, const char *aString2)
{
   const unsigned char  *thePtr1 = (unsigned char *) aString1;
   const unsigned char *thePtr2 = (unsigned char *) aString2;
   unsigned char     theChar1, theChar2;
   
   PMLITE_BEGIN_PROC(c_strcmp_imp);

   while ((theChar1 = *thePtr1++) == (theChar2 = *thePtr2++))
      if (theChar1 == 0)
         return 0;

   return (theChar1 - theChar2);
}

/* ------------------------------------------------------------------- */

char *c_strcpy_imp(char *aDest, const char *aSrc)
{
   const char  *theSrc = aSrc;
   char     *theDest = aDest;
   
   PMLITE_BEGIN_PROC(c_strcpy_imp);

   while ((*theDest++ = *theSrc++) != 0)
      ;
      
   return aDest;
}

/* ------------------------------------------------------------------- */

char* c_strcat_imp(char *aDest, const char *aSrc)
{
   const char  *theSrcPtr = aSrc;
   char     *theDestPtr = aDest;
   
   PMLITE_BEGIN_PROC(c_strcat_imp);

   while ((*theDestPtr++) != 0)
      ;
   
   theDestPtr--;
   
   while ((*theDestPtr++ = *theSrcPtr++) != 0)
      ;
   
   return aDest;
}

/* ------------------------------------------------------------------- */

size_t c_strlen_imp(const char *aString)
{
   size_t   theLength = (size_t) -1;
   
   PMLITE_BEGIN_PROC(c_strlen_imp);

   do
   {
      theLength++;
   }
   while ((*aString++) != 0);
   
   return theLength;
}

/* -------------------------------------------------------------------
   Returns a pointer to the first occurence of the char in the string. If the char
   is 0, a pointer to the terminating null character is returned.
   If the character is not found 0 is returned.
   
   WARNING: This function has never been tested
   ------------------------------------------------------------------- */

char* c_strchr_imp(const char *aString, int aChar)
{
   PMLITE_BEGIN_PROC(c_strchr_imp);

    while ((*aString != 0) && (*aString != (char) aChar))
      aString++;

    if (*aString == (char) aChar)
      return (char*) aString;
            
    return 0;
}
