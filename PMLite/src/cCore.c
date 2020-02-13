/* ===================================================================
   cCore.c
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   =================================================================== */

#include "cCore.h"
#include "cXCore.h"
#include "cXTrace.h"

/* -------------------------------------------------------------------
   Returns a string made of valid hexadecimal characters. Alphabetic characters
   appear only one time in the string in uppercase.
   ------------------------------------------------------------------- */

char *c_xhexdigits_imp()
{
   PMLITE_BEGIN_PROC(c_xhexdigits_imp);

   return "0123456789ABCDEF";
}

/* -------------------------------------------------------------------
   Reverse character order in the string.
   Retruns a pointer onto the string.
   If the string is null, a null pointer is returned.
   ------------------------------------------------------------------- */

char *c_xstrrev_imp(char *aStr)
{
   size_t   theStart = 0;
   size_t   theIndex;
   char  theChar;
   
   PMLITE_BEGIN_PROC(c_xstrrev_imp);

   if (aStr != 0)
   {
      theIndex = c_strlen(aStr) - 1;
      
      while (theStart < theIndex) 
      {
         theChar = aStr[theStart];
         
         aStr[theStart] = aStr[theIndex];
         aStr[theIndex] = theChar;
         
         theStart++;
         theIndex--;
      }
   }
   
   return aStr;
}

/* -------------------------------------------------------------------
   Returns non zero (true) if the character is a standard white-space character.
   Standard white-space characters are defined to be:
      ' ' = 32
      '\r' = 13
      '\n' = 10
      '\t' = 9
      '\f' = 12
      '\v' = 11
   ------------------------------------------------------------------- */

int c_isspace_imp(int aChar)
{
   PMLITE_BEGIN_PROC(c_isspace_imp);

   return ((aChar >= 0x09) && (aChar <= 0x0D)) || (aChar == 0x20);
}

/* -------------------------------------------------------------------
   If the character can be converted to uppercase, the converted character is 
   returned.
   If not the character is returned.
   ------------------------------------------------------------------- */

int c_toupper_imp(int aChar)
{
   PMLITE_BEGIN_PROC(c_toupper_imp);

   if ((aChar < 'a') || (aChar > 'z'))
      return aChar;

   return (aChar + 'A' - 'a');
}

/* ------------------------------------------------------------------- */

   static int c_xcharicmp(char aChar1, char aChar2)
   {
      PMLITE_BEGIN_PROC(c_xcharicmp);

      if (aChar1 == aChar2)
         return 0;
         
      if ((aChar1 >= 'A') && (aChar1 <= 'Z'))
         aChar1 |= 0x20;
         
      if ((aChar2 >= 'A') && (aChar2 <= 'Z'))
         aChar2 |= 0x20;
         
      return (unsigned char) aChar1 - (unsigned char) aChar2;
   }

/* -------------------------------------------------------------------
   Compares the two strings case irrelevant. Comparison stops after 'aLength' 
   characters are compared or a null character is encountered.
      
      returns < 0 if aStr1 < aStr2
      returns  0  if aStr1 == aStr2
      returns > 0 if aStr1 > aStr2

   Crashes if 'aStr1' or 'aStr2' is null.
   ------------------------------------------------------------------- */

int c_xstrnicmp_imp(const char *aStr1, const char *aStr2, size_t aLength)
{
   int   theValue;

   PMLITE_BEGIN_PROC(c_xstrnicmp_imp);

   if (aLength != 0)
   {
      while (aLength-- > 0)
      {
         if ((theValue = c_xcharicmp(*aStr1, *aStr2)) != 0)
            return theValue;
            
         aStr1++;
         aStr2++;
      }
   }
   
   return 0;
}

/* -------------------------------------------------------------------
   Compares the two strings. Comparison stops after 'aLength' characters are 
   compared or a null character is encountered.
      
      returns < 0 if aStr1 < aStr2
      returns  0  if aStr1 == aStr2
      returns > 0 if aStr1 > aStr2

   Crashes if 'aStr1' or 'aStr2' is null.
   ------------------------------------------------------------------- */

int c_strncmp_imp(const char *aStr1, const char *aStr2, size_t aLength)
{
   PMLITE_BEGIN_PROC(c_strncmp_imp);

   if (aLength != 0)
   {
      while (aLength-- > 0)
      {
         if ((*aStr1) != (*aStr2))
            return ((unsigned char)(*aStr1)) - ((unsigned char)(*aStr2));

         aStr1++;
         aStr2++;
      }
   }
   
   return 0;
}

/* -------------------------------------------------------------------
   Compares the two strings.
      
      returns < 0 if aStr1 < aStr2
      returns  0  if aStr1 == aStr2
      returns > 0 if aStr1 > aStr2
      
   Crashes if 'aStr1' or 'aStr2' is null.
   ------------------------------------------------------------------- */

int c_xstricmp_imp(const char *aStr1, const char *aStr2) 
{
   size_t   theLength1, theLength2;
   
   PMLITE_BEGIN_PROC(c_xstricmp_imp);

   theLength1 = c_strlen(aStr1);
   theLength2 = c_strlen(aStr2);
   if (theLength2 > theLength1)
      theLength1 = theLength2;
      
   return c_xstrnicmp_imp(aStr1, aStr2, theLength1);
}

/* -------------------------------------------------------------------
   Copy up to 'aLength' characters from 'aSr1' to 'aStr2'. The copy operation 
   stops when one of the following condition is true:
      - 'aMax' minus one  characters have been copied
      - 'aLength' characters have been copied
      - a null character is copied
   
   #### All these comments need to be checked.
   Crashes if 'aStr2' is null.
   May crash if 'aMax' is greater than 'aLength'.
   
   Does nothing if 'aStr1' is null or 'aMax' is less than 1.
   If 'aStr2' is shorter than 'aLength' bytes.

   If there is no null in the first 'aLength' characters of 'aStr2', the result
   will not be null terminated.
      
   CAUTION: To be POSIX compliant the copy should not work if strings overlapped
   but in our implementation it works.
   ------------------------------------------------------------------- */

char *c_xstrncpymax_imp(char *aStr1, const char *aStr2, size_t aLength, size_t aMax)
{
   PMLITE_BEGIN_PROC(c_xstrncpymax_imp);

   if ((aStr1 == 0) || (aMax == 0))
      return 0;
      
   if (aLength >= (aMax - 1))
      aLength = aMax - 1;
      
   c_memmove((pmbyte*) aStr1, (pmbyte*) aStr2, aLength);
   aStr1[aLength] = 0;
   
   return aStr1;
}

/* ------------------------------------------------------------------- */

char *c_xstrcpymax_imp(char *aStr1, const char *aStr2, size_t aMax)
{
   PMLITE_BEGIN_PROC(c_xstrcpymax_imp);

   if (aStr1 == 0)
      return 0;
      
   return c_xstrncpymax_imp(aStr1, aStr2, c_strlen(aStr2), aMax);
}

/* ------------------------------------------------------------------- */

char *c_xstrncatmax_imp(char *aStr1, const char *aStr2, size_t aLength, size_t aMax)
{
   size_t   theLength;
   
   PMLITE_BEGIN_PROC(c_xstrncatmax_imp);

   if ((aStr2 == 0) || (aStr1 == 0) || (aMax == 0))
      return aStr1;
      
   theLength = c_strlen(aStr1);
   if (theLength + aLength > (aMax - 1))
      aLength = aMax - theLength - 1;
      
   c_memmove(&aStr1[theLength], aStr2, aLength);
   aStr1[theLength + aLength] = 0;
   
   return aStr1;
}

/* ------------------------------------------------------------------- */

char *c_xstrcatmax_imp(char *aStr1, const char *aStr2, size_t aMax)
{
   PMLITE_BEGIN_PROC(c_xstrcatmax_imp);

   return c_xstrncatmax_imp(aStr1, aStr2, c_strlen(aStr2), aMax);
}


/* ------------------------------------------------------------------- */

char *c_strpbrk_imp(const char *aSrc, const char *aChars)
{
   PMLITE_BEGIN_PROC(c_strpbrk_imp);

   return c_xstrpbrk_imp (aSrc, aChars, 0);
}

/* ------------------------------------------------------------------- */

char *c_xstrpbrk_imp(const char *aSrc, const char *aChars, char aDelimiter)
{
   const char *theChars;
   
   PMLITE_BEGIN_PROC(c_xstrpbrk_imp);

   if ((aSrc == 0) || (aChars == 0))
      return 0;
      
   while (*aSrc != aDelimiter)
   {
      theChars = aChars;
      
      while (*theChars != 0)
      {
         if (*aSrc == *theChars)
            return (char*) aSrc;
            
         theChars++;
      }
      
      aSrc++;
   }
   
   return 0;
}

/* ------------------------------------------------------------------- */

int c_isdigit_imp(int aChar)
{
   PMLITE_BEGIN_PROC(c_isdigit_imp);

   return (aChar >= '0') && (aChar <= '9');
}

/* ------------------------------------------------------------------- */

static char *c_numtoa_imp(pmint32 aValue, pmbool afSigned, char *aStr, size_t aSize) 
{
   pmuint32 theValue;
   pmbool      thefNegative = pmfalse;
   size_t      theIndex = 0;
#ifdef PMLITE_TRACE
   size_t      theSize = aSize;
#endif
   
   PMLITE_BEGIN_PROC(c_numtoa_imp);

   if (afSigned && (aValue < 0))
   {
      thefNegative = pmtrue;
      theValue = (pmuint32) (-aValue);
   }
   else
      theValue = (pmuint32) aValue;
      
   do
   {
      if (aSize == 0)
      {
#ifdef PMLITE_TRACE
         if (afSigned)
            c_trace(TL(pmT_Core, "c_numtoa_imp(): Buffer too small: Value = (%ld), Size = (%lu)", (pmint32)aValue, (pmuint32)theSize));
         else
            c_trace(TL(pmT_Core, "c_numtoa_imp(): Buffer too small: Value = (%lu), Size = (%lu)", (pmuint32)aValue, (pmuint32)theSize));
#endif
         return 0;
      }
      aStr[theIndex++] = (char) ((theValue % 10UL) + '0');     
      theValue /= 10UL;
      aSize--;
   } 
   while (theValue != 0);
      
   if (thefNegative)
   {
      if (aSize == 0)
      {
#ifdef PMLITE_TRACE
         if (afSigned)
            c_trace(TL(pmT_Core, "c_numtoa_imp(): Buffer too small: Value = (%ld), Size = (%lu)", (pmint32)aValue, (pmuint32)theSize));
         else
            c_trace(TL(pmT_Core, "c_numtoa_imp(): Buffer too small: Value = (%lu), Size = (%lu)", (pmuint32)aValue, (pmuint32)theSize));
#endif
         return 0;
      }
      aStr[theIndex++] = '-';
      aSize--;
   }

   if (aSize == 0)
   {
#ifdef PMLITE_TRACE
      if (afSigned)
         c_trace(TL(pmT_Core, "c_numtoa_imp(): Buffer too small: Value = (%ld), Size = (%lu)", (pmint32)aValue, (pmuint32)theSize));
      else
         c_trace(TL(pmT_Core, "c_numtoa_imp(): Buffer too small: Value = (%lu), Size = (%lu)", (pmuint32)aValue, (pmuint32)theSize));
#endif
      return 0;
   }
   aStr[theIndex] = 0;
   return c_xstrrev(aStr);
}

/* ------------------------------------------------------------------- */

char *c_xint16toa_imp(pmint16 aValue, char *aStr, size_t aSize) 
{
   PMLITE_BEGIN_PROC(c_xint16toa_imp);

   return c_numtoa_imp((pmint32) aValue, pmtrue, aStr, aSize);
}

/* ------------------------------------------------------------------- */

char *c_xuint16toa_imp(pmuint16 aValue, char *aStr, size_t aSize)
{
   PMLITE_BEGIN_PROC(c_xuint16toa_imp);

   return c_numtoa_imp((pmint32) aValue, pmfalse, aStr, aSize);
}

/* ------------------------------------------------------------------- */

char *c_xint32toa_imp(pmint32 aValue, char *aStr, size_t aSize)
{
   PMLITE_BEGIN_PROC(c_xint32toa_imp);

   return c_numtoa_imp((pmint32) aValue, pmtrue, aStr, aSize);
}

/* ------------------------------------------------------------------- */

char *c_xuint32toa_imp(pmuint32 aValue, char *aStr, size_t aSize)
{
   PMLITE_BEGIN_PROC(c_xuint32toa_imp);

   return c_numtoa_imp((pmint32) aValue, pmfalse, aStr, aSize);
}

/* ------------------------------------------------------------------- */

char *c_xstrnchr_imp(const char *aStr, size_t aLen, int aChar)
{
   char  *theCur = (char*) aStr;
   char  *theEnd;
   
   PMLITE_BEGIN_PROC(c_xstrnchr_imp);

   if ((aStr == 0) || (aLen == 0))
      return 0;
      
   theEnd = theCur + aLen;
   while (theCur < theEnd)
   {
      if (*theCur == (char) aChar)
         return theCur;
      theCur++;
   }
   
   return 0;
}

/* ------------------------------------------------------------------- */

pmbool c_xsplitnstr_imp(char* aStr, size_t aStrLength, char aSplit, size_t anIndex, size_t *aStart, size_t *anEnd)
{
   size_t   theIndex = 0, i = 0;
   char  *theStr = aStr;
   
   PMLITE_BEGIN_PROC(c_xsplitnstr_imp);

   if (aStr == 0)
      return pmfalse;
      
   *aStart = 0;
   while (i < aStrLength)
   {
      if (*theStr == aSplit)
      {
         if (theIndex == anIndex)
         {
            (*anEnd) = (size_t) (theStr - aStr);
            return pmtrue;
         }
         
         (*aStart) = (size_t) (theStr - aStr + 1);
         theIndex++;
      }
      
      theStr++;
      i++;
   }
   
   (*anEnd) = (size_t) (theStr - aStr);
   return (theIndex == anIndex);
}

/* ------------------------------------------------------------------- */

pmbool c_xsplitstr_imp(char *aStr, char aSplit, size_t anIndex, size_t *aStart, size_t *anEnd)
{
   PMLITE_BEGIN_PROC(c_xsplitstr_imp);

   return c_xsplitnstr_imp(aStr, c_strlen(aStr), aSplit, anIndex, aStart, anEnd);
}

/* ------------------------------------------------------------------- */

static pmint32 c_xatovalue(const char *aStr, pmbool afSigned)
{
   int      theChar;    /* current character. */
   pmint32  theTotal;      /* current total. */
   int      theSign = 1;   /* if '-', then negative, otherwise positive. */

   PMLITE_BEGIN_PROC(c_xatovalue);

      /* Skip whitespaces */
   while (c_isspace((int) (unsigned char)(*aStr)))
       ++aStr;

   theChar = (int) (unsigned char) *aStr++;

   if ((theChar == '-') && (!afSigned))
      return 0;
      
   theSign = theChar;                        /* Save sign indication */
   if ((theChar == '-') || (theChar == '+'))
       theChar = (int) (unsigned char) *aStr++; /* Skip sign */
   
   theTotal = 0;
   while (c_isdigit(theChar)) 
   {
       theTotal = 10 * theTotal + (theChar - '0'); /* Accumulate digit. */
       theChar = (int)(unsigned char)*aStr++;      /* Get next char. */
   }

      /* Returns result, negated if necessary. */
   if (theSign == '-')
       return -theTotal;
   
   return theTotal;   
}

/* ------------------------------------------------------------------- */

pmint32 c_xatoint32_imp(const char *aStr)
{
   PMLITE_BEGIN_PROC(c_xatoint32_imp);

   return c_xatovalue(aStr, pmtrue);
}

/* ------------------------------------------------------------------- */

pmuint32 c_xatouint32_imp(const char *aStr)
{
   PMLITE_BEGIN_PROC(c_xatouint32_imp);

   return (pmuint32) c_xatovalue(aStr, pmfalse);
}

/* ------------------------------------------------------------------- */

pmbool c_xnatouint32_imp(const char* aStr, size_t aLength, pmuint32 *anUInt32)
{
   char  theStr[10];
   char* thePtr = theStr;
   
   PMLITE_BEGIN_PROC(c_xnatouint32_imp);

   (*anUInt32) = 0;
   if (aStr == 0)
      return pmfalse;
      
   c_xstrncpymax_imp(theStr, aStr, aLength, 10);
   do
   {
      if (!c_isdigit(*thePtr++))
         return pmfalse;
   }
   while (*thePtr != 0);
   
         //#####
   (*anUInt32) = (pmuint32) c_xatoint32(theStr);
   return pmtrue;
}

/* ------------------------------------------------------------------- */

char *c_xstrnstr_imp(const char *aStr, size_t aLen, const char *aPattern, size_t *aPos)
{
   pmbool      thefFound = pmfalse;
   size_t      theCurLen = 0;
   size_t      theLocalCurLen = 0;
   const char* theCurPtr;
   const char* theCurPatternPtr;

   PMLITE_BEGIN_PROC(c_xstrnstr_imp);

   if ((aStr == 0) || (aPattern == 0))
      return 0;

   while ((theCurLen < aLen) && (!thefFound))
   {
      theCurPatternPtr = aPattern;
      theCurPtr = aStr;

      theLocalCurLen = theCurLen;
      
      while ((theLocalCurLen < aLen) && (*theCurPtr == *theCurPatternPtr) && (*theCurPatternPtr != 0))
      {
         theLocalCurLen++;
         theCurPtr++;
         theCurPatternPtr++;
      }

      if (*theCurPatternPtr == 0)
      {
         thefFound = pmtrue;
         if (aPos != 0)
            (*aPos) = theCurLen;
      }
      else
      {
         aStr++;
         theCurLen++;
      }
   }

   return thefFound ? (char*) aStr : 0;
}

/* ------------------------------------------------------------------- */

int c_isalnum_imp(int aChar)
{
   PMLITE_BEGIN_PROC(c_isalnum_imp);

   if (((aChar >= '0') && (aChar <= '9')) ||
      ((aChar >= 'a') && (aChar <= 'z')) ||
      ((aChar >= 'A') && (aChar <= 'Z'))) 
      return pmtrue;
      
   return pmfalse;
}
