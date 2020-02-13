// ================================================================
// OSWinInc.h
// (C) 1997 AbyreSoft - All rights reserved.
// ================================================================

#ifndef OSWinInc_h
#define OSWinInc_h

// ----------------------------------------------------------------
#ifndef STRICT
   #define STRICT 1
#endif
#ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN      1
#endif

#pragma warning (disable : 4201 4214 4514 4127)

/*
 *  NOGDICAPMASKS     - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
 *  NOVIRTUALKEYCODES - VK_*
 *  NOWINMESSAGES     - WM_*, EM_*, LB_*, CB_*
 *  NOWINSTYLES       - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
 *  NOSYSMETRICS      - SM_*
 *  NOMENUS           - MF_*
 *  NOICONS           - IDI_*
 *  NOKEYSTATES       - MK_*
 *  NOSYSCOMMANDS     - SC_*
 *  NORASTEROPS       - Binary and Tertiary raster ops
 *  NOSHOWWINDOW      - SW_*
 *  OEMRESOURCE       - OEM Resource values
 *  NOATOM            - Atom Manager routines
 *  NOCLIPBOARD       - Clipboard routines
 *  NOCOLOR           - Screen colors
 *  NOCTLMGR          - Control and Dialog routines
 *  NODRAWTEXT        - DrawText() and DT_*
 *  NOGDI             - All GDI defines and routines
 *  NOKERNEL          - All KERNEL defines and routines
 *  NOUSER            - All USER defines and routines
 *  NONLS             - All NLS defines and routines
 *  NOMB              - MB_* and MessageBox()
 *  NOMEMMGR          - GMEM_*, LMEM_*, GHND, LHND, associated routines
 *  NOMETAFILE        - typedef METAFILEPICT
 *  NOMINMAX          - Macros min(a,b) and max(a,b)
 *  NOMSG             - typedef MSG and associated routines
 *  NOOPENFILE        - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
 *  NOSCROLL          - SB_* and scrolling routines
 *  NOSERVICE         - All Service Controller routines, SERVICE_ equates, etc.
 *  NOSOUND           - Sound driver routines
 *  NOTEXTMETRIC      - typedef TEXTMETRIC and associated routines
 *  NOWH              - SetWindowsHook and WH_*
 *  NOWINOFFSETS      - GWL_*, GCL_*, associated routines
 *  NOCOMM            - COMM driver routines
 *  NOKANJI           - Kanji support stuff.
 *  NOHELP            - Help engine interface.
 *  NOPROFILER        - Profiler interface.
 *  NODEFERWINDOWPOS  - DeferWindowPos routines
 *  NOMCX             - Modem Configuration Extensions
*/
#define NOGDICAPMASKS      1
#define NOVIRTUALKEYCODES  1
/* #define NOWINMESSAGES      1 */
#define NOWINSTYLES        1
#define NOSYSMETRICS    1
#define NOMENUS            1
#define NOICONS            1
#define NOKEYSTATES        1
#define NOSYSCOMMANDS      1
#define NORASTEROPS        1
#define NOSHOWWINDOW    1
#define OEMRESOURCE        1
#define NOATOM          1
#define NOCLIPBOARD        1
#define NOCOLOR            1
#define NOCTLMGR        1
#define NODRAWTEXT         1
#define NOGDI           1
/* #define NOKERNEL        1 */
/* #define NOUSER          1 */
#define NONLS           1
#define NOMB            1
#define NOMEMMGR        1
#define NOMETAFILE         1
#define NOMINMAX        1
/* #define NOMSG           1 */
#define NOOPENFILE         1
#define NOSCROLL        1
#define NOSERVICE       1
#define NOSOUND            1
#define NOTEXTMETRIC    1
#define NOWH            1
#define NOWINOFFSETS    1
#define NOCOMM          1
#define NOKANJI            1
#define NOHELP          1
#define NOPROFILER         1
#define NODEFERWINDOWPOS   1
#define NOMCX           1

#include <windows.h>

#pragma warning (disable : 4201 4214 4514 4127)

// ----------------------------------------------------------------
#endif // OSWinInc_h
