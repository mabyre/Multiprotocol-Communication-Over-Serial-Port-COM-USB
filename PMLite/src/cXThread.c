/* ===================================================================
   cXThread.c
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   ===================================================================  */

#include "cXEnv.h"
#include "cXStack.h"

/* ------------------------------------------------------------------- */

pmbool c_xthread_init_imp(void)
{
#ifdef PMLITE_STACK_DEBUG
   TStackInfo* theSI;

   /* Initialize stack info */
   theSI = c_xstack_get_info_imp();
   if (theSI == 0)
      return pmfalse;

   c_xstack_reset_usage();
#endif /* PMLITE_STACK_DEBUG */

   return pmtrue;
}

/* ------------------------------------------------------------------- */

void c_xthread_term_imp(void)
{
#ifdef PMLITE_STACK_DEBUG
   TStackInfo* theSI;

   /* Initialize stack info */
   theSI = c_xstack_get_info_imp();
   if (theSI != 0)
      free(theSI);
#endif /* PMLITE_STACK_DEBUG */
}
