#include	"csl.h"
#include	"clstructs.h"
#include 	"clsupport.h"
#include 	"inc\clreaders.h"
#include	"menu_parser.h"
#include 	<time.h>
#include 	<stdio.h>
//#include 	<conio.h> // textmode

#include <windows.h>

#include "ASTrace.h"
#include "cMenu.h"
#include "cXCore.h"
#include "cXStack.h"

/******************************************************************************/

#define APP_TITLE "Console Test application"

#include "string.h"
#include "stdlib.h"

char FullExeFileNameWithDirectory[1024];

PMMENU_DECLARE(TReadThread);
PMMENU_DECLARE(TWriteThread);
PMMENU_DECLARE(TMultiReader);

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
int main(int argc, char *argv[])
{
#ifndef NDEBUG
	static const char szTitle[] = APP_TITLE " - Debug";
#else
	static const char szTitle[] = APP_TITLE " - Release";
#endif
	COORD theCoord = { 80, 1000 };
	SMALL_RECT theRect = { 0, 0, 150, 24 };
	HWND theHWnd = 0;
	int i = 0;

	SetConsoleTitle(szTitle);
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), theCoord);
	theCoord = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));

#define WIN_USER_H_
#ifdef WIN_USER_H_
	while (1)
	{
		theHWnd = FindWindow("ConsoleWindowClass", szTitle);
		if (theHWnd == 0)
		{
			i++;
			if (i == 20)
			break;
			Sleep(1);
		}
		else
		 break;
	}
	if (theHWnd != 0)
	{
		SetWindowPos(theHWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		if (theRect.Bottom >= 2)
		{
			theRect.Bottom = theCoord.Y - 2;
			SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &theRect);
		}
	}
#endif

//	textmode(C4350); // j'ai touvé ça pour passer en mode 50 lignes mais ça ne marche pas

	strcpy( FullExeFileNameWithDirectory, argv[0]);

#if defined( PM_USE_TRACES ) || defined( PMLITE_TRACE )
	as_trace_init(GetModuleHandle(0));
#endif

	c_xstack_reset_usage();
	c_xthread_init();

	PMMenu_InitializeMainMenu();

	PMMENU_USE(TReadThread);
	PMMENU_USE(TWriteThread);
	PMMENU_USE(TMultiReader);

	PMMenu_ExecuteMainMenu();
	PMMenu_DeleteMainMenu();

	c_xmemdbg_dump_state();
	c_xthread_term();

#if defined( PM_USE_TRACES ) || defined( PMLITE_TRACE )
	as_trace_close();
#endif

	return EXIT_SUCCESS;
}
