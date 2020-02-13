/*---------------------------------------------------------------------------*\
 * Copyright (C) 1996-2002 - AbyreSoft. All rights reserved.
 * cInput.c
\*---------------------------------------------------------------------------*/

#include "cInput.h"
#include "cXCore.h"

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else  /* __cplusplus */
#define NULL    ((void *)0)
#endif  /* __cplusplus */
#endif  /* NULL */
/*---------------------------------------------------------------------------*/

#define kInputFlag_PlusSign     0x01
#define kInputFlag_LessSign     0x02
#define kInputFlag_Hex          0x04
#define kInputFlag_ExtraData    0x08
#define kInputFlag_Overflow     0x10
#define kInputFlag_ValidValue   0x20

/*---------------------------------------------------------------------------*\
 * We have our own isspace and toupper functions defined here, because:
 * - they are slightly different from the Posix ones
 * - we want this module to depend only of PMXLite
\*---------------------------------------------------------------------------*/

#define Input_P_isspace(x)  ((x) <= 32)
#define Input_P_toupper(x)  ((((x) >= 'a') && ((x) <= 'z')) ? ((x) - ('a' - 'A')) : (x))

/*---------------------------------------------------------------------------*/

static pmuint32 Input_P_StrToValue(char *aStr, pmuint16 *aStateFlag, size_t aByteSize)
{
   pmuint32 theChar; 
   pmuint32 theValueMax;
   pmint16  theScanState = 0;
   pmuint32 theZeroCount = 0;
   pmuint32 theBase = 10;
   pmuint32 theValue = 0;
   pmbyte   theFlag = 0;
   pmuint32 theOverflowValue = 0xff;
   
   while ( *aStr != 0 )
   {
      theChar = (pmuint32) (*aStr);
      
      switch ( theScanState )
      {
         case 0:        
            /* Skip and count leading '0' characters. */
            if (theChar == '0')
            {
               theZeroCount++;
               theFlag |= kInputFlag_ValidValue;
               aStr++;
               break;
            }
            
            /* Skip white-space like characters.   */
            if (theChar <= ' ')
            {
               aStr++;
               break;
            }
            
            /* Check for a sign ('+' or '-') character
               and for an hexadecimal value. */
            if (theChar == '+')
            {
               theFlag |= kInputFlag_PlusSign;
               aStr++;
            }
            else if (theChar == '-')
            {
               theFlag |= kInputFlag_LessSign;
               aStr++;
            }
            else if (((theChar == 'x') || (theChar == 'X')) && (theZeroCount > 0))
            {
               theFlag |= kInputFlag_Hex;
               theBase = 16;
               aStr++;
            }
            
            /* Compute overflow value. */
            if (aByteSize == 4)
               theOverflowValue = 0xffffffff;
            else if (aByteSize == 2)
               theOverflowValue = 0xffff;

            /* Compute maximum value (depending of the base).  */
            theValueMax = theOverflowValue / theBase;
            theScanState = 1;          
            break;
         
         case 1:           
            /* Convert lowercase alphabetic characters to uppercase 
               (if parsing hexadecimal).  */
            if ((theBase == 16) && (theChar >= 'a') && (theChar <= 'f'))
            {
               theChar -= 'a';
               theChar += 'A';
            }
            
            /* Accept numeric and alphabetic characters (if parsing hexadecimal).   */
            if ((theChar >= '0') && (theChar <= '9'))
               theChar -= '0';
            else if ((theChar >= 'A') && (theChar <= 'F') && (theBase == 16))
            {
               theChar -= 'A';
               theChar += 10;
            }
            else
            {
               /* Check for extra data.   */
               if (*aStr != 0)
                  theFlag |= kInputFlag_ExtraData;             
               goto done;
            }
            
            /* Check for an overflow.  */
            if (theValue > theValueMax)
            {
               theFlag |= (kInputFlag_Overflow | kInputFlag_ExtraData);
               goto done;
            }
            
            theFlag |= kInputFlag_ValidValue;
            theValue *= theBase;
            
            /* Check for an overflow.  */
            if (theChar > (theOverflowValue - theValue))
            {
               theFlag |= (kInputFlag_Overflow | kInputFlag_ExtraData);
               goto done;
            }
            
            theValue += theChar;
            
            aStr++;           
            break;            
      }
   }

done:    
   
   /* Update sign.   */
   if (theFlag & kInputFlag_LessSign)
      theValue = (pmuint32)-(pmint32)theValue;
   
   if (aStateFlag != 0)
      (*aStateFlag) = theFlag;
   
   return theValue;
}

/*---------------------------------------------------------------------------*/

static pmuint32 c_getnumeric(size_t aByteSize, pmbool afAllowSigned)
{
   char     theString[32];
   pmuint32 theValue;
   pmuint16 theFlag;
   pmbool      thefOK = pmfalse;
   
   while (!thefOK)
   {
      c_ngets(theString, sizeof(theString));
      
      theValue = Input_P_StrToValue(theString, &theFlag, aByteSize);
      
      if ((theFlag & kInputFlag_ValidValue) != 0)
         if ((theFlag & (kInputFlag_ExtraData | kInputFlag_Overflow)) == 0)
         {
            if (afAllowSigned)
               thefOK = pmtrue;
            else
               thefOK = ((theFlag & kInputFlag_LessSign) == 0);
         }
               
      if (!thefOK)
         c_printf("\n");
   }
   
   return theValue;
}

/*---------------------------------------------------------------------------*/

pmuint32 c_get_uint32(void)
{
   return c_getnumeric(sizeof(pmuint32), pmfalse);
}

/*---------------------------------------------------------------------------*/

pmuint16 c_get_uint16(void)
{
   return (pmuint16)c_getnumeric(sizeof(pmuint16), pmfalse);
}

/*---------------------------------------------------------------------------*/

pmint32 c_get_int32(void)
{
   return (pmint32) c_getnumeric(sizeof(pmuint32), pmtrue);
}

/*---------------------------------------------------------------------------*/

pmint16 c_get_int16(void)
{
   return (pmint16) c_getnumeric(sizeof(pmuint16), pmtrue);
}

/*---------------------------------------------------------------------------*/

char Input_Char(char* aPrompt, char* anAllowed)
{
   char  theString[20];
   char* thePtr;

   while (1)
   {
      if (aPrompt != 0)
         c_printf("%s: ", aPrompt);

      c_ngets(theString, 10);
      
      thePtr = theString;
      while (Input_P_isspace(*thePtr))
         thePtr++;
         
      if (*thePtr == 0)
         continue;
         
      (*thePtr) = (char) Input_P_toupper(*thePtr);
      
      if ((anAllowed != 0) && (c_strchr(anAllowed, *thePtr) == 0))
         continue;

      break;
   }

   return *thePtr;
}

/*---------------------------------------------------------------------------*/

static pmbool Input_Numeric(char* aPrompt, pmuint32* aValue, pmbool afDefault, pmuint32 aDefault, pmbool afCancel, size_t aByteSize)
{
   char     theString[20];
   char     *thePtr;
   pmuint16 theFlag;

   while (1)
   {
      if (aPrompt != 0)
         c_printf("%s", aPrompt);
      
      if (afDefault)
         c_printf(" (*=%lu)", (unsigned long) aDefault);
      
      if (afCancel)
         c_printf(" (~ to cancel)");
         
      c_printf(": ");
      c_ngets(theString, sizeof(theString));

      thePtr = theString;
      while (Input_P_isspace(*thePtr))
         thePtr++;

      if (afDefault && (*thePtr == '*'))
      {
         (*aValue) = aDefault;
         return pmtrue;
      }
      
      if (afCancel && (*thePtr == '~'))
         return pmfalse;

      (*aValue) = Input_P_StrToValue(thePtr, &theFlag, aByteSize);
      
      if (((theFlag & (kInputFlag_LessSign | kInputFlag_ExtraData | kInputFlag_Overflow)) != 0) ||
         ((theFlag & kInputFlag_ValidValue) == 0))
         continue;
         
      return pmtrue;
   }

      /* Never reached */
   return pmfalse;
}

/*---------------------------------------------------------------------------*/

pmbool Input_UInt32(char* aPrompt, pmuint32* aValue, pmbool afDefault, pmuint32 aDefaultValue, pmbool afCancel)
{
   return Input_Numeric(aPrompt, aValue, afDefault, aDefaultValue, afCancel, sizeof(pmuint32));
}

/*---------------------------------------------------------------------------*/

pmbool Input_UInt16(char* aPrompt, pmuint16* aValue, pmbool afDefault, pmuint16 aDefaultValue, pmbool afCancel)
{
   pmuint32 theValue;
   
   if (!Input_Numeric(aPrompt, &theValue, afDefault, aDefaultValue, afCancel, sizeof(pmuint16)))
      return pmfalse;
   
   (*aValue) = (pmuint16) theValue; 
   return pmtrue;
}

/*---------------------------------------------------------------------------*/

pmbool Input_String
(
    char* aPrompt, 
    char* aBuffer, 
    char* aDefault, 
    pmbool afCancel, 
    pmbool afAllowEmpty
)
{
    char  theString[256];
    char *thePtr = "";

    while ( 1 )
    {
        if ( aPrompt != 0 )
            c_printf("%s", aPrompt );

        if ( aDefault != 0 )
            c_printf(" (*=%s)", aDefault );
 
        if ( afCancel )
            c_printf(" (~ to cancel)");
 
        c_printf(": ");
        c_ngets( theString, sizeof( theString ) );

        thePtr = theString;
        while ( Input_P_isspace(*thePtr) )
            thePtr++;

        if ( *theString == '\0' )
        {
            if ( !afAllowEmpty )
            {
                continue;
            }
            else if ( aDefault != 0 )
            {
                c_strcpy( aBuffer, aDefault );
                return pmtrue;
            }
        }

        if ( (aDefault != 0) && (*thePtr == '*') )
        {
            c_strcpy( aBuffer, aDefault );
            return pmtrue;
        }

        if ( afCancel && (*thePtr == '~') )
            return pmfalse;

        c_strcpy( aBuffer, theString );
        return pmtrue;
    }

   /* Never reached */
   return pmfalse;
}

/*---------------------------------------------------------------------------*\
 * Input_String() ci-dessus remet la valeur par defaut. Ici, on veut que si 
 * l'utilisateur tape return ou enter la string reste inchangee
\*---------------------------------------------------------------------------*/

pmbool Input_NewString
(
    char    *aPrompt, 
    char    *aBuffer, 
    pmbool   afAllowEmpty
)
{
    char  theString[256];
    char *thePtr = "";

    while ( 1 )
    {
        if ( aPrompt != 0 )
            c_printf("%s", aPrompt );

        if ( aBuffer != NULL )
            c_printf(" (%s)", aBuffer );
 
        c_printf(" : ");

        c_ngets( theString, sizeof( theString ) );

        thePtr = theString;
        while ( Input_P_isspace(*thePtr) )
            thePtr++;

        if ( *theString == '\0' )
        {
            if ( !afAllowEmpty )
            {
                continue;
            }
            else
            {
                return pmtrue;
            }
        }

        c_strcpy( aBuffer, theString );
        return pmtrue;
    }

   /* Never reached */
   return pmfalse;
}

