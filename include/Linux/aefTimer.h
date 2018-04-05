/***************************************************************************/
/*!
 **	\file  aefLinuxTimer.h
 **	\brief Implements timers under VxWorks. 
 **
 **	This implimentation of application timers under VxWorks uses the VxWorks 
 **	Watchdog Timers. 
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
#ifndef _aefLinuxTimer_h
#define _aefLinuxTimer_h

#include "aefTimerInterface.h"
#include "aefMutex.h"
#include <signal.h>

class CAefTimer : public aefTimerInterface
{
public:
    CAefTimer();
    virtual ~CAefTimer();
    virtual void Init(AEF_TIMER_TASK_ID timerTaskId);
    virtual AEF_TIMER_HANDLE Start(uint32_t id, uint32_t ms, uint8_t type, void* pVoid = NULL);
    virtual AEF_TIMER_HANDLE Start(AEF_TIMER_TASK_ID timerTaskId, uint32_t id, uint32_t ms, uint8_t type, void* pVoid = NULL);
    virtual bool Stop(AEF_TIMER_HANDLE htimer);
    virtual void DeleteAllTimers();
    virtual bool GetTimerClientInfo(AEF_TIMER_HANDLE hTimer, uint32_t& timerId, void*& pVoid);
    virtual void HandleTimer(AEF_TIMER_HANDLE hTimer);
    static void Fire(union sigval);

protected:
    TimerMap m_timerMap;
    CAefMutex m_mutexTimerMap;
    uint32_t m_TimerInstance;

private:
    AEF_TIMER_TASK_ID m_TimerTaskId;
};

#endif
