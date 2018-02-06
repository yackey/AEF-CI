/***************************************************************************/
/*!
 **	\file  aefTypes.h
 **	\brief {Brief Description}
 **
 **	Avtec, Inc.
 **	100 Innovation Place
 **	Lexington, SC 29072 USA
 **	(803) 358-3600
 **
 **	Copyright (C) Avtec, Inc., 1990-present.
 **	All rights reserved. Copying, compilation, modification, distribution
 **	or any other use whatsoever of this material is strictly prohibited
 **	without the written consent of Avtec, Inc.
 *****************************************************************************/
#ifndef _AEFTYPES_H
#define _AEFTYPES_H 1

#if defined(VXWORKS)

#include <VxWorks.h>
#include <stdio.h>
#include <msgQLib.h>
#include <string>
#include <map>
#include <taskLib.h>
#include <kernelLib.h>
#include <wdLib.h>
#include <semLib.h>
#include <tickLib.h>

#define AEF_ERROR  ERROR
#define AEF_SUCCESS  OK

#define AEF_WAIT_FOREVER ((UNSIGNED32)WAIT_FOREVER)
#define AEF_DONT_WAIT  ((UNSIGNED32)NO_WAIT)

#elif defined(WIN32)

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <map>
#include <string>

#define AEF_ERROR  ERROR
#define AEF_SUCCESS  ERROR_SUCCESS

#define AEF_WAIT_FOREVER ((UNSIGNED32)INFINITE)
#define AEF_DONT_WAIT  0

#elif defined(LINUX)

#include <aefTypes.h>
//#include <stdio.h>
//#include <string>
//#include <map>
//#include <limits.h>
//
//#define AEF_ERROR  -1
//#define AEF_SUCCESS  0
//
//#define AEF_WAIT_FOREVER ((UNSIGNED32)ULONG_MAX)
//#define AEF_DONT_WAIT  ((UNSIGNED32)0)
//
#endif
//
//typedef SIGNED32 AEF_STATUS;
//
//#define AEF_TIMER_TYPE_ONE_SHOT     1
//#define AEF_TIMER_TYPE_PERIODIC     2

#endif
