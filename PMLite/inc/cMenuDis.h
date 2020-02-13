/* ===========================================================================
   cMenuDis.h
   Copyright (C) 1996-98 AbyreSoft. All rights reserved.
   ===========================================================================   */

#ifndef cMenuDis_h
#define cMenuDis_h

/* ---------------------------------------------------------------------------   */

#include "cMenu.h"

/* ---------------------------------------------------------------------------   */

#ifdef __cplusplus
   extern "C" {
#endif

/* ---------------------------------------------------------------------------   */

/* Called with aMenu != 0 and aLine == 0 for "0 - Exit"  */
void     PMMenu_DisplayLine(PMMenu* aMenu, PMMenuLine* aLine, pmint32 aLevel);

pmuint16 PMMenu_GetChoice(PMMenu* aMenu, pmint32 aLevel);
void     PMMenu_BadChoice(PMMenu* aMenu, pmint32 aLevel);

/* ---------------------------------------------------------------------------   */

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------   */

#endif
