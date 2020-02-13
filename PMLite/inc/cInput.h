/*---------------------------------------------------------------------------*\
 * Copyright (C) 1996-2002 - AbyreSoft. All rights reserved.
 * cInput.h
\*---------------------------------------------------------------------------*/

#ifndef cInput_h
#define cInput_h

/*---------------------------------------------------------------------------*/

#include "cConsole.h"

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
   extern "C" {
#endif

/*---------------------------------------------------------------------------*\
 * Returns a pmuint16.  Asks for a value until the user enters a valid value 
 * for a pmuint16.  
\*---------------------------------------------------------------------------*/
pmuint16    c_get_uint16(void);

/*---------------------------------------------------------------------------*\
 * Returns a pmint16. Asks for a value until the user enters a valid value 
 * for a pmint16.
\*---------------------------------------------------------------------------*/
pmint16     c_get_int16(void);

/*---------------------------------------------------------------------------*\
 * Returns a pmuint32. Asks for a value until the user enters a valid value 
 * for a pmuint32.
\*---------------------------------------------------------------------------*/
pmuint32    c_get_uint32(void);

/*---------------------------------------------------------------------------*\
 * Returns a pmint32. Asks for a value until the user enters a valid value 
 * for a pmint32.
\*---------------------------------------------------------------------------*/
pmint32     c_get_int32(void);

/*---------------------------------------------------------------------------*\
 * Displays the prompt (if not null) and asks the user for a character.
 * Returned character is always an uppercase character.
 * If the list of allowed character is not null, asks for a character 
 * until it is part of the list. (As characters are put to uppercase, it is
 * not necessary to include the uppercase and the lowercase version of a 
 * character in the list of allowed characters.
\*---------------------------------------------------------------------------*/
char    Input_Char(char* aPrompt, char* anAllowed);

/*---------------------------------------------------------------------------*\
 * Displays the prompt (if not null) and asks the user for a string.
 * 'aBuffer' should be at least 256 characters.
 * If 'aDefaultValue' is not null, 'aDefaultValue' can be easely entered by the user.
 * If 'afEmpty' is true an empty string can be returned.
 * If 'afCancel' is true, the user can cancel the input.    
 * Returns false only if the user has cancelled input.
\*---------------------------------------------------------------------------*/
pmbool  Input_String
(
    char* aPrompt, 
    char* aBuffer, 
    char* aDefaultValue, 
    pmbool afCancel, 
    pmbool afEmpty
);

/*---------------------------------------------------------------------------*\
 * La fonction ci-dessus possede certaines caracteristiques mais cela ne 
 * suffisait pas. Cette fonction affiche la valeur et la conserve par defaut
\*---------------------------------------------------------------------------*/
pmbool  Input_NewString( char* aPrompt, char* aBuffer, pmbool afEmpty );

/*---------------------------------------------------------------------------*\
 * Displays the prompt (if not null) and asks the user for a pmuint16.
 * Asks for a value until the user enters a valid value for a pmuint16.
 * If 'afDefault' is true, 'aDefaultValue' can be easely entered by the user.
 * if 'afCancel' is true, the user can cancel the input.    
 * Returns false only if the user has cancelled input.
\*---------------------------------------------------------------------------*/
pmbool  Input_UInt16(char* aPrompt, pmuint16* aValue, pmbool afDefault, pmuint16 aDefaultValue, pmbool afCancel);

/*---------------------------------------------------------------------------*\
 * Displays the prompt (if not null) and asks the user for a pmuint32.
 * Asks for a value until the user enters a valid value for a pmuint32.
 * If 'afDefault' is true, 'aDefaultValue' can be easely entered by the user.
 * If 'afCancel' is true, the user can cancel the input.    
 * Returns false only if the user has cancelled input.
\*---------------------------------------------------------------------------*/
pmbool  Input_UInt32(char* aPrompt, pmuint32* aValue, pmbool afDefault, pmuint32 aDefaultValue, pmbool afCancel);

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/*---------------------------------------------------------------------------*/

#endif
