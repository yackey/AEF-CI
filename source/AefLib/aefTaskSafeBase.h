/***************************************************************************/
/*!
**	\file 	aefTaskSafe.h 
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
#ifndef _aefTaskSafeBase_h_
#define _aefTaskSafeBase_h_

class CAefTaskSafeBase
{
	// Class Methods
public:
	CAefTaskSafeBase();  
	virtual ~CAefTaskSafeBase();
	virtual bool TakeOwnership(UNSIGNED32 msWait = AEF_WAIT_FOREVER) = 0;
	virtual bool ReleaseOwnership() = 0;
	virtual bool IsOwned() = 0;
}; 

#endif //_aefTaskSafeBase_h_
