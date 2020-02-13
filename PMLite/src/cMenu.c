/*==========================================================================*\
 * Copyright (c) 1999-2002 AbyreSoft - All rights reserved.
 *==========================================================================*
 * cMenu.h - Written by Bruno Raby.
\*==========================================================================*/

#include "cMenu.h"
#include "cMenuDis.h"

#include "cXTrace.h"
#include "cXCore.h"
#include "cXStack.h"
#include "cConsole.h"

/*---------------------------------------------------------------------------*/

PMMenu* gMainMenu;

/*---------------------------------------------------------------------------*\
 * Menu line
\*---------------------------------------------------------------------------*/

PMMenuLine *PMMenuLine_New()
{
   PMMenuLine* self = (PMMenuLine*)c_malloc(sizeof(PMMenuLine));
   
   if (self != 0)
      c_memset(self, 0, sizeof(PMMenuLine));
   
   return self;
}

/*---------------------------------------------------------------------------*/

void PMMenuLine_Delete(PMMenuLine** self)
{
   if ((*self) != 0)
   {
      if ((*self)->itsMenu != 0)
         PMMenu_Delete(&(*self)->itsMenu);
         
      c_free(*self);
      (*self) = 0;
   }
}

/*---------------------------------------------------------------------------*\
 * Menu
\*---------------------------------------------------------------------------*/

PMMenu* PMMenu_New(PMMenuProc aProc)
{
   PMMenu*  self = (PMMenu*) c_malloc(sizeof(PMMenu));
   
   if (self != 0)
   {
      c_memset(self, 0, sizeof(PMMenu));
      self->itsProc = aProc;
   }
   
   return self;
}

/*---------------------------------------------------------------------------*/

void PMMenu_Delete(PMMenu* *self)
{
   PMMenuLine* theLine, *theLine2;
   
   if ((*self) != 0)
   {
      theLine = (*self)->itsFirst;
      
      while (theLine != 0)
      {
         theLine2 = theLine->itsNext;
         PMMenuLine_Delete(&theLine);
         theLine = theLine2;
      }
      
      c_free(*self);
      (*self) = 0;
   }
}

/*---------------------------------------------------------------------------*/

pmbool PMMenu_AddLineToMenu(PMMenu* self, pmuint16 aCode, const char *aText, PMMenu* aSubMenu, PMMenuItemProc aProc)
{
   PMMenuLine* theLine, *theLastLine;

   if ((aSubMenu != 0) && (aProc != 0))
      c_trace(TL(pmM_Test, "PMMenu_AddLineToMenu(): Both aMenu and aProc are not 0 ???"));

   if (aCode != kPMMenuSeparator)
   {
      theLine = self->itsFirst;
      
      while (theLine != 0)
      {
         if (aCode == theLine->itsCode)
         {
            c_trace(TL(pmM_Test, "PMMenu_AddLineToMenu(): a line already exists with code (%lu)", (unsigned long)aCode));
            return pmfalse;
         }
         
         theLine = theLine->itsNext;
      }
   }

   theLine = PMMenuLine_New();
   theLine->itsCode = aCode;
   theLine->itsText = aText;
   theLine->itsMenu = aSubMenu;
   theLine->itsProc = aProc;
   
   theLine->itsNext = 0;
   
   if (self->itsFirst == 0)
      self->itsFirst = theLine;
   else
   {
      theLastLine = self->itsFirst;
      while (theLastLine->itsNext != 0)
         theLastLine = theLastLine->itsNext;
      theLastLine->itsNext = theLine;
   }
   
   return pmtrue;
}

/*---------------------------------------------------------------------------*/

pmbool PMMenu_AddLinesToMenu(PMMenu* self, const PMMenuLine *aLines)
{
   const PMMenuLine  *theLine = aLines;

   while (theLine->itsCode != 0)
   {
      if (!PMMenu_AddLineToMenu(self, theLine->itsCode, theLine->itsText, theLine->itsMenu, theLine->itsProc))
         return pmfalse;
         
      theLine++;
   }
   
   return pmtrue;
}

/*---------------------------------------------------------------------------*/

void PMMenu_Execute(PMMenu* self, pmint32 aLevel)
{
   PMMenuLine* theLine;
   pmuint16 theCode;

   if ((self == 0) || (self->itsFirst == 0))
   {
      c_trace(TL(pmM_Test, "PMMenu_Execute(): Empty menu."));
      return;
   }

   if (self->itsProc != 0)
      (*self->itsProc)(kPMMenuInit, 0);

   if (self->itsFirst->itsNext == 0)
   {
      theLine = self->itsFirst;
      if (theLine->itsMenu != 0)
         PMMenu_Execute(theLine->itsMenu, aLevel + 1);
      else
         if (self->itsProc != 0)
            /* c_xstack_reset_usage(); */
            (*self->itsProc)(theLine->itsCode, theLine);
            /* c_xstack_dump_usage(); */
   }
   else
   {
      while (1)
      {
         PMMenu_DisplayLine(self, 0, aLevel);
         
         theLine = self->itsFirst;
         while (theLine != 0)
         {
            PMMenu_DisplayLine(self, theLine, aLevel);
            theLine = theLine->itsNext;
         }
         
         while (1)
         {
            theCode = PMMenu_GetChoice(self, aLevel);
            if (theCode == 0)
               break;
               
            theLine = self->itsFirst;
            while (theLine != 0)
            {
               if (theCode == theLine->itsCode)
                  break;
               theLine = theLine->itsNext;
            }
            
            if (theLine != 0)
               break;

            PMMenu_BadChoice(self, aLevel);
         }
         
         if (theCode == 0)
            break;
            
         if (theLine->itsMenu != 0)
            PMMenu_Execute(theLine->itsMenu, aLevel + 1);
         else
            if (self->itsProc != 0)
            {
               /* c_stack_reset_usage(); */
               (*self->itsProc)(theCode, theLine);
               /* c_stack_dump_usage(); */
            }
      }
   }
   
   if (self->itsProc != 0)
      (*self->itsProc)(kPMMenuClose, 0);
}

/*---------------------------------------------------------------------------*\
 * Main menu
\*---------------------------------------------------------------------------*/

void PMMenu_InitializeMainMenu()
{
   gMainMenu = PMMenu_New(0);
}

/*---------------------------------------------------------------------------*/

void PMMenu_DeleteMainMenu()
{
   PMMenu_Delete( &gMainMenu );
}

/*---------------------------------------------------------------------------*/

void PMMenu_AddToMainMenu( PMMenuLine aLines[], PMMenuProc aProc, const char *const aTitle )
{
   PMMenu*     theMenu;
   PMMenuLine* theLine;
   pmuint16 theCode = 1;

   if (gMainMenu == 0)
      return;
      
   theLine = gMainMenu->itsFirst;
   
   while ( theLine != 0 )
   {
      theCode++;
      theLine = theLine->itsNext;
   }

   if ( (theMenu = PMMenu_New( aProc )) != 0 )
   {
      PMMenu_AddLinesToMenu( theMenu, aLines );
      PMMenu_AddLineToMenu( gMainMenu, theCode, aTitle, theMenu, 0 );
   }
}

/*---------------------------------------------------------------------------*/

void PMMenu_ExecuteMainMenu()
{
   PMMenu_Execute( gMainMenu, 0 );
}
