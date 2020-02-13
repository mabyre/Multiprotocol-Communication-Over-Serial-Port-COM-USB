/*--------------------------------------------------------------------------*\
 * Copyright (c) 1999-2002 AbyreSoft - All rights reserved.
 *--------------------------------------------------------------------------*
 * cMenu.h - Written by Bruno Raby.
\*--------------------------------------------------------------------------*/

#ifndef cMenu_h
#define cMenu_h

/*--------------------------------------------------------------------------*/

#include "cXTypes.h"

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   extern "C" {
#endif

/*--------------------------------------------------------------------------*\
 *   Module and traces.
\*--------------------------------------------------------------------------*/

#define pmM_Test  2

#define pmT_Test  PMTraceDefine(pmM_Test, 0)

/*--------------------------------------------------------------------------*/

struct _PMMenu;
typedef struct _PMMenu  PMMenu, *pPMMenu;

struct _PMMenuLine;
typedef struct _PMMenuLine PMMenuLine, *pPMMenuLine;

/*--------------------------------------------------------------------------*/

typedef void (*PMMenuProc)(pmuint16 aCode, PMMenuLine* aLine);
typedef void (*PMMenuItemProc)(void);

/*--------------------------------------------------------------------------*/

struct _PMMenuLine
{
   pmuint16       itsCode;
   const char    *itsText;
   PMMenu*        itsMenu;
   PMMenuLine*    itsNext;
   PMMenuItemProc itsProc;
};

/*--------------------------------------------------------------------------*/

struct _PMMenu
{
   PMMenuLine*    itsFirst;
   PMMenuProc     itsProc;
};

/*--------------------------------------------------------------------------*\
 * Global variable referencing the main menu.
\*--------------------------------------------------------------------------*/

extern PMMenu *gMainMenu;

/*--------------------------------------------------------------------------*/

#define kPMMenuInit     ((pmuint16) -1)
#define kPMMenuClose    ((pmuint16) -2)

/*--------------------------------------------------------------------------*/

#define kPMMenuSeparator   ((pmuint16) -1)

/*--------------------------------------------------------------------------*/

PMMenuLine* PMMenuLine_New();
void        PMMenuLine_Delete(PMMenuLine** self);

/*--------------------------------------------------------------------------*/

PMMenu  *PMMenu_New(PMMenuProc aProc);
void     PMMenu_Delete(PMMenu** self);
pmbool   PMMenu_AddLineToMenu(PMMenu* self, pmuint16 aCode, const char* aText, PMMenu* aSubMenu, PMMenuItemProc aProc);
pmbool   PMMenu_AddLinesToMenu(PMMenu* self, const PMMenuLine *aLines);
void     PMMenu_Execute(PMMenu* self, pmint32 aLevel);

/*--------------------------------------------------------------------------*/

void     PMMenu_InitializeMainMenu();
void     PMMenu_AddToMainMenu(PMMenuLine aLines[], PMMenuProc aProc, const char* const aTitle);
void     PMMenu_DeleteMainMenu();
void     PMMenu_ExecuteMainMenu();

/*--------------------------------------------------------------------------*\
 * Exemple of variables to define for Core menu
 *    extern const char* const CorePMMenuTitle;
 *    extern dPMMenuLine CorePMMenuLines[];
 *    extern void CorePMMenuProc(pmuint16 aCode, pPMMenuLine aLine);
 *    
 * and then use PMMENU_DECLARE(Core); on top of main and 
 *    PMMENU_USE(Core) before calling PMMenuExecute
\*--------------------------------------------------------------------------*/
/*
** Je ne trouve pas le moyen, en C++, de faire un reference externe a une 
** donnee de type const char* const (ca ne marche pas !)
** Par contre pour une donnee de type const char* (ca marche...)
*/
#ifdef __cplusplus
#define PMMENU_DECLARE(name)                                            \
   extern const char* name ## PMMenuTitle;                              \
   extern PMMenuLine name ## PMMenuLines[];                             \
   extern void name ## PMMenuProc(pmuint16 aCode, PMMenuLine* aLine)
#else
#define PMMENU_DECLARE(name)                                            \
   extern const char* const name ## PMMenuTitle;                        \
   extern PMMenuLine name ## PMMenuLines[];                             \
   extern void name ## PMMenuProc(pmuint16 aCode, PMMenuLine* aLine)
#endif

#define PMMENU_USE(name)                                               \
   PMMenu_AddToMainMenu(name ## PMMenuLines, name ## PMMenuProc, name ## PMMenuTitle)

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
#define PMMENU_BEGIN(name, title)                               \
   const char* name ## PMMenuTitle = title;                     \
   void name ## PMMenuProc(pmuint16 aCode, PMMenuLine* aLine);  \
   PMMenuLine name ## PMMenuLines[] = {
#else
#define PMMENU_BEGIN(name, title)                               \
   const char* const name ## PMMenuTitle = title;               \
   void name ## PMMenuProc(pmuint16 aCode, PMMenuLine* aLine);  \
   PMMenuLine name ## PMMenuLines[] = {
#endif

#define PMMENU_ITEM(x, y)  { x, y, 0, 0, 0 },
#define PMMENU_ITEM_EX(x, y, z)  { x, y, 0, 0, z },
#define PMMENU_ITEM_SEPARATOR(y) { kPMMenuSeparator, y, 0, 0, 0 },
#define PMMENU_END() { 0, 0, 0, 0, 0 } \
                                      }; /* End of declaration structure */

/*--------------------------------------------------------------------------*/

#define PMMENU_HANDLER(name) void name ## PMMenuProc(pmuint16 aCode, PMMenuLine* aLine)

#define PMMENU_CHOICE_BEGIN() \
    switch (aCode) {

#define PMMENU_CASE_INIT(x)             case kPMMenuInit: x(); break
#define PMMENU_CASE_CLOSE(x)            case kPMMenuClose: x(); break
#define PMMENU_CASE_CHOICE(x, y)        case x: y(); break
#define PMMENU_CASE_CHOICE_1(x, y, p0)  case x: y(p0); break

/* Le "case -3:" ne correspond a rien il est seulement la pour annuler 
** un warning de compilation en C++ 
*/
#define PMMENU_CHOICE_END()                     \
    case -3:                                    \
    default:                                    \
        if (aLine != 0 && aLine->itsProc != 0)  \
        (*aLine->itsProc)(); break;             \
    } /* fin du switch (aCode) */

#define PMMENU_DEFAULT_HANDLER(name)    \
    PMMENU_HANDLER(name)                \
    {                                   \
        PMMENU_CHOICE_BEGIN()           \
        PMMENU_CHOICE_END()             \
    }

/*--------------------------------------------------------------------------*/

#define PMMENU_SUBEXECUTE(name)                             \
{                                                           \
   PMMENU_DECLARE(name);                                    \
                                                            \
   pPMMenu theMenu = PMMenu_New(name ## PMMenuProc);        \
                                                            \
   if (theMenu != 0)                                        \
   {                                                        \
      PMMenu_AddLinesToMenu(theMenu, name ## PMMenuLines);  \
      PMMenu_Execute(theMenu, 0);                           \
      PMMenu_Delete(&theMenu);                              \
   }                                                        \
}

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/*--------------------------------------------------------------------------*/

#endif
