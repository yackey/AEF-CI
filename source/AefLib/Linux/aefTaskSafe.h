/***************************************************************************/
/*!
**	\file 	aefLinuxTaskSafe.h 
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
#ifndef _AEFVXTASKSAFE_H_
#define _AEFVXTASKSAFE_H_

#include "aefTypes.h"
#include "aefTaskSafeBase.h"
#include "aefMutex.h"

#define AEF_LOCK_TIMEOUT	100		// a "useful" timeout value? (in ms)

class CAefTaskSafe : public CAefMutex, public CAefTaskSafeBase
{
	// Class Methods
public:
	CAefTaskSafe();  
	virtual ~CAefTaskSafe();
	virtual bool TakeOwnership(UNSIGNED32 msWait = AEF_WAIT_FOREVER);
	virtual bool ReleaseOwnership();
	virtual bool IsOwned();

protected:
	AEF_TASK_ID	m_taskOwnerID;
};

#endif /*_AEFVXTASKSAFE_H_*/
