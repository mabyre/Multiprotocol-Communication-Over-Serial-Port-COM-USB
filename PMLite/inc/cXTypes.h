/*--------------------------------------------------------------------------*\
 * Copyright (c) 1998-2000 AbyreSoft - All rights reserved.
 *--------------------------------------------------------------------------*
 * cXType.h - Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * based types definition
\*--------------------------------------------------------------------------*/

#ifndef _CXTYPES_H_
#define _CXTYPES_H_

#include "cXEnv.h"

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
   extern "C" {
#endif

/*--------------------------------------------------------------------------*\
 *                   Type definitions                                   
\*--------------------------------------------------------------------------*/

typedef unsigned char   pmbyte;
typedef unsigned short  pmuint16;
typedef unsigned long   pmuint32;
typedef short           pmint16;
typedef long            pmint32;
typedef int             pmbool;

#define pmfalse    0
#define pmtrue     1

#ifndef _SIZE_T_DEFINED
      typedef unsigned int size_t;
#     define _SIZE_T_DEFINED
#endif

/*--------------------------------------------------------------------------*\
 * Maximal and minimal value definitions for pmint16, pmuint16, pmint32
 * and pmuint32 types.
\*--------------------------------------------------------------------------*/

#define kPMUINT32_MIN   ((pmuint32) 0)
#define kPMUINT32_MAX   ((pmuint32) 4294967295)

#define kPMUINT16_MIN   ((pmuint16) 0)
#define kPMUINT16_MAX   ((pmuint16) 65535)

#define kPMINT32_MIN    (((pmint32) -2147483647) - 1)
#define kPMINT32_MAX    ((pmint32) 2147483647)

#define kPMINT16_MIN    (((pmint16) -32767) - 1)
#define kPMINT16_MAX    ((pmint16) 32767)

/* ------------------------------------------------------------------- */

typedef pmuint16  pmerror;

#ifdef __cplusplus
   }
#endif

/* ------------------------------------------------------------------- */

#endif /* cXType */
