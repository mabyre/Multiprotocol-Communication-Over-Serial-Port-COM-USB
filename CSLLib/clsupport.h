/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                               Copyright2014                               */
/*****************************************************************************/
/*  Name: clsupport_win32.h                                                  */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description : functions holding the functions                             */
/*   used by CSL as wait/semaphore/thread                                    */
/*****************************************************************************/

#ifndef CLSUPPORT_H_
#define CLSUPPORT_H_

#ifdef WIN32
	#include <stdio.h>
	#include <string.h>
	#include ".\porting\win32\clsupport_win32.h"
	#include ".\porting\win32\fileio_win32.h"
	#include ".\porting\win32\netal_win32.h"
	#include ".\porting\win32\comal_win32.h"
#else
#if defined( LINUXPC )
	#include <stdio.h>
	#include <string.h>
	#include ".\porting\win32\clsupport_linux.h"
	#include ".\porting\win32\fileio_linux.h"
	#include ".\porting\win32\netal_linux.h"
	#include ".\porting\win32\comal_linux.h"
#else
	#include <stdio.h>
	#include <string.h>
	#include ".\porting\win32\clsupport_android.h"
	#include ".\porting\win32\fileio_android.h"
	#include ".\porting\win32\netal_android.h"
	#include ".\porting\win32\comal_android.h"
#endif // ANDROID
#endif // WIN32
#endif /* CLSUPPORT_H_ */
