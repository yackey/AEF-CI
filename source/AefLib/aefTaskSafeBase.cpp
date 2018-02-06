/***************************************************************************/
/*!
**	\file 	aefLinuxTaskSafe.cpp
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
#include "aefTypesBase.h"
#include "aefTaskSafeBase.h"

/**************************************************************************/
/*!
*	\brief Constructor for the TaskSafe class 
*	
***************************************************************************/
CAefTaskSafeBase::CAefTaskSafeBase()
{

}

/**************************************************************************/
/*!
*	\brief Deconstructor for the TaskSafe class.	
*	
***************************************************************************/
CAefTaskSafeBase::~CAefTaskSafeBase()
{
}

///**************************************************************************/
///*!
//*	\brief	Releases task ownership of this object. 
//*	
//* 	\return TRUE if successful or otherwise FALSE
//***************************************************************************/
//bool CAefTaskSafe::ReleaseOwnership()
//{
//	bool rc = false;
//
//	if (m_taskOwnerID == pthread_self())
//	{
//		m_taskOwnerID = AEF_INVALID_TASK_ID;
//		rc = true;
//		Unlock();  // release the lock we obtained when we took ownership
//	}
//	
//	return rc;
//}
//
//
///**************************************************************************/
///*!
//*	\brief	Tells if object is currently owned by a particular task
//*	
//* 	\return TRUE if successful or otherwise FALSE
//***************************************************************************/
//bool CAefTaskSafe::IsOwned()
//{
//	bool result;
//	
//	if (m_taskOwnerID == AEF_INVALID_TASK_ID) 
//        {
//		result = false;	// not owned
//	}
//	else 
//        {
//		result = true;
//	}
//
//	return result;
//}
