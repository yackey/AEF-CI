/***************************************************************************/
/*!
 **	\file  aefimer.h
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
#ifndef _aefTimerBase_h
#define _aefTimerBase_h

#include <aefTypesBase.h>

// Define AEF Task timers

enum TASK_TIMER_IDS
{
    AEF_TIMER_WATCHDOG_REPORT_ID = 1,
};

// Application timers should start from this value
#define TASK_TIMER_BASE_ID 100

/*!
 *	\class CAefTimerBase 
 *	\brief AEF Timer object.  
 *	
 *	The CAefTimerBase class defines and implements the timer interface. 
 *	The idea is that timers need a task that is waiting on a message.
 *   This class will send a AEF_TM_TIMER message that should be associated
 *   with an handler method in the application class. To use this class, 
 *	derive a class from CAefQueueTask.   
 *
 *	\todo	Need to determine if we need an Id0 and Id1 to index a timer.
 *
 *	\note	Timers will be seperate for each task. You can only add a timer \n
 *			to your own task.
 */
class CAefTimerBase
{
public:
    CAefTimerBase();
    virtual ~CAefTimerBase();
    virtual void Init(AEF_TIMER_TASK_ID timerTaskId) = 0;
    virtual AEF_TIMER_HANDLE Start(UNSIGNED32 id, UNSIGNED32 ms, UNSIGNED8 type, void* pVoid = NULL) = 0;
    virtual AEF_TIMER_HANDLE Start(AEF_TIMER_TASK_ID timerTaskId, UNSIGNED32 id, UNSIGNED32 ms, UNSIGNED8 type, void* pVoid = NULL) = 0;
    virtual bool Stop(AEF_TIMER_HANDLE htimer) = 0;
    virtual void DeleteAllTimers() = 0;
    virtual bool GetTimerClientInfo(AEF_TIMER_HANDLE hTimer, UNSIGNED32& timerId, void*& pVoid) = 0;
    virtual void HandleTimer(AEF_TIMER_HANDLE hTimer) = 0;
};

#endif  // _aefTimerBase_h
