/***************************************************************************/
/*!
 **	\file  aefTimerInterface.h
 **	\brief aefTimer Interface defination
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

// Define AEF Task timers

enum TASK_TIMER_IDS
{
    AEF_TIMER_WATCHDOG_REPORT_ID = 1,
};

// Application timers should start from this value
#define TASK_TIMER_BASE_ID 100

class aefTimerInterface
{
public:
    virtual void Init(AEF_TIMER_TASK_ID timerTaskId) = 0;
    virtual AEF_TIMER_HANDLE Start(uint32_t id, uint32_t ms, uint8_t type, void* pVoid = NULL) = 0;
    virtual AEF_TIMER_HANDLE Start(AEF_TIMER_TASK_ID timerTaskId, uint32_t id, uint32_t ms, uint8_t type, void* pVoid = NULL) = 0;
    virtual bool Stop(AEF_TIMER_HANDLE htimer) = 0;
    virtual void DeleteAllTimers() = 0;
    virtual bool GetTimerClientInfo(AEF_TIMER_HANDLE hTimer, uint32_t& timerId, void*& pVoid) = 0;
    virtual void HandleTimer(AEF_TIMER_HANDLE hTimer) = 0;
};

