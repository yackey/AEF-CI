/***************************************************************************/
/*!
 **	\file 	aefLinuxQueueTask.h 
 **	\brief	Defines the VxWorks-specific fields and methods in the CAefQueueTask class.
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
#ifndef _aefLinuxQueueTask_h_
#define _aefLinuxQueueTask_h_

#include "aefTypes.h"
#include "aefQueueTaskBase.h"
#include "aefTask.h"
#include "aefTaskMessage.h"
#include <string.h>

class CAefTimer;

#define VXTASK_MESSAGE_OPTIONS MSG_Q_FIFO  // zzzz put this in vx'ville

class CAefQueueTask : public CAefTask, public CAefQueueTaskBase
{
    // Class Methods
public:
    CAefQueueTask();
    CAefQueueTask(const char *name, int priority, int options, int stack);
    virtual ~CAefQueueTask();
    virtual AEF_STATUS MapMessage(int messageID, AefCallback handler);
    static AEF_STATUS SendMessage(const char *taskname, CAefTaskMessage *pTM);
    static AEF_STATUS SendMessage(AEF_MSG_TASK_ID queue_id, CAefTaskMessage* pTM);
    virtual AEF_STATUS SendMessage(CAefTaskMessage* pTM);
    static AEF_STATUS SendBroadcastMessage(CAefTaskMessage* pTM);

    virtual void Invoke(AefCallback handler, void *arg1)
    {
        (this->*handler)(arg1);
    };
    virtual AEF_STATUS RegisterWatchdogKick();
    virtual AEF_STATUS UnregisterWatchdogKick();
protected:
    virtual void Construct();
    virtual AEF_STATUS Init(void* pTaskData = NULL);
    virtual AEF_STATUS Exit(AEF_STATUS exitCode);
    virtual AEF_STATUS Run();
    virtual AEF_TIMER_HANDLE StartTimer(UNSIGNED32 id, UNSIGNED32 ms, UNSIGNED8 type, void* pVoid = NULL);
    virtual void StopTimer(AEF_TIMER_HANDLE htimer);
    virtual void OnTimer(UNSIGNED32 timerId, void* pVoid);
    virtual void TimerHandler(void *pMsg);
    // Class Properties
protected:
    CAefTimer* m_pTaskTimer;
    // Message map 
    TASK_MESSAGE_MAP m_TaskMessageMap;
    MSG_Q_ID m_QueueID;
    mq_attr m_QueueOptions;
    std::string m_sName;
    AEF_TIMER_HANDLE m_hWdogReportTimer;
    CAefTaskMessage m_TaskMsg;

};

#endif // _aefLinuxQueueTask_h_

