/***************************************************************************/
/*!
**	\file	aefMutex.h 
**	\brief	AEF Mutex Object Definition
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
#ifndef _aefMutexBase_h_
#define _aefMutexBase_h_

#include <aefTypesBase.h>

class CAefMutexBase
{
public:
	CAefMutexBase();
	virtual ~CAefMutexBase();

	virtual bool		Lock(UNSIGNED32 msWait = AEF_WAIT_FOREVER) = 0;
	virtual bool		Unlock() = 0;
	virtual SIGNED32	GetLockCount() = 0;

};

#endif // _aefMutexBase_h_
