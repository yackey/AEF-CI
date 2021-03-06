/***************************************************************************/
/*!
 **	\file 	aefQueueTaskInterface.h 
 **	\brief Task Class that supports messaging and timers
 **
 **	Abstrac base class
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
#pragma once

#include <aefTypes.h>
class CAefQueueTask;
typedef void (CAefQueueTask::*AefCallback)(void *msg);
typedef std::map<int, AefCallback> TASK_MESSAGE_MAP;

class CAefTaskMessage;

/*!
 *	\class CAefMsgTask
 *	\brief Class definition that implements a task capable of receiving messages and using timers.
 *	
 *	The CAefMsgTask is derived from CAefTask and contains its own
 *   main execution loop.  It implements a message queue so that it
 *   is capable of receiving task messages and using timers.
 */

class aefQueueTaskInterface
{
    // Class Methods
public:
    virtual AEF_STATUS MapMessage(int messageID, AefCallback handler) = 0;
    virtual AEF_STATUS SendMessage(CAefTaskMessage* pTM) = 0;
    virtual void Invoke(AefCallback handler, void *arg1) = 0;
protected:
    virtual AEF_STATUS Init(void* pTaskData = NULL) = 0;
    virtual AEF_STATUS Exit(AEF_STATUS exitCode) = 0;
    virtual AEF_STATUS Run() = 0;
    virtual AEF_TIMER_HANDLE StartTimer(uint32_t id, uint32_t ms, uint8_t type, void* pVoid = NULL) = 0;
    virtual void StopTimer(AEF_TIMER_HANDLE htimer) = 0;
    virtual void OnTimer(uint32_t timerId, void* pVoid) = 0;
    virtual void TimerHandler(void *pMsg) = 0;
};

