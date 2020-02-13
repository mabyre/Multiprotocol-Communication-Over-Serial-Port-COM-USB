/* ===================================================================
   cXStackP.c
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   ===================================================================  */

#include "cXStack.h"

/* ------------------------------------------------------------------- */

#ifdef PMLITE_STACK_DEBUG

#include "OSWinInc.h"

/* ------------------------------------------------------------------- */

TStackInfo* c_xstack_get_info_imp(void)
{
   static DWORD   theTLS = 0xFFFFFFFF;
   TStackInfo*    theSI;

   if (theTLS == 0xFFFFFFFF)
   {
      theTLS = TlsAlloc();
      if (theTLS == 0xFFFFFFFF)
         return 0;
   }

   theSI = (TStackInfo*)TlsGetValue(theTLS);
   if (theSI == 0)
   {
      theSI = (TStackInfo*)malloc(sizeof(TStackInfo));
      theSI->itsfReset = pmfalse;
      if (!TlsSetValue(theTLS, theSI))
         return 0;
   }

   return theSI;
}

/* ------------------------------------------------------------------- */

#endif /* PMLITE_STACK_DEBUG */
