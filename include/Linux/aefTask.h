/***************************************************************************/
/*!
 **	\file 	aefLinuxTask.h 
 **	\brief	Defines the VxWorks-specific data for the CAefTask base class. 
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
#ifndef _aefLinuxTask_h
#define _aefLinuxTask_h 1

#include "aefLinuxTypes.h"
#include "aefTaskInterface.h"
#include <pthread.h>
#include <semaphore.h>

class CAefMutex;
class CAefQueueTask;

class CAefTask : public aefTaskInterface
{
    // Class Methods
public:
    CAefTask();
    CAefTask(const char *name, int priority, int options, int stack);
    virtual ~CAefTask();

    virtual AEF_STATUS Start();
    virtual AEF_STATUS Stop(uint32_t msWait = AEF_WAIT_FOREVER);
    virtual AEF_STATUS Stop(CAefQueueTask* pTask, uint32_t msgID); 
        
    virtual char* GetTaskName()
    {
        return m_TaskName;
    }
    
    virtual void SetTaskName(const char *name);

    virtual int GetTaskPriority()
    {
        return m_TaskPriority;
    }

    virtual void SetTaskPriority(int priority)
    {
        m_TaskPriority = priority;
    }

    virtual int GetTaskOptions()
    {
        return m_TaskOptions;
    }

    virtual void SetTaskOptions(int options)
    {
        m_TaskOptions = options;
    }

    virtual int GetTaskStackSize()
    {
        return m_TaskStack;
    }

    virtual void SetTaskStackSize(int stack)
    {
        m_TaskStack = stack;
    }

    virtual inline bool IsRunning()
    {
        return (m_TaskID != AEF_INVALID_TASK_ID) ? true : false;
    };

    virtual void SetDeleteFlag()
    {
        m_bDeleteMyself = true;
    }

    virtual AEF_STATUS TakeWatchdogOwnership();
    virtual AEF_STATUS ReleaseWatchdogOwnership();
    virtual AEF_STATUS RegisterWatchdogKick();
    virtual AEF_STATUS UnregisterWatchdogKick();
    static uint32_t GetTimestamp();
    static uint32_t MsToTimeTicks(uint32_t ms);
    virtual void TaskDelay(uint16_t ms);

    void Construct(const char *name, int priority, int options, int stack); 
            
protected:
    virtual AEF_STATUS Init(void* pTaskData = NULL);
    virtual AEF_STATUS Exit(AEF_STATUS exitCode);
    static void* ThreadRun(void* vp);
    virtual AEF_STATUS RunWrapper();
    virtual AEF_STATUS Run() = 0;
    virtual void WatchdogReport();
    virtual bool WatchdogCheck(uint32_t *task_id = NULL);

    // Class Properties
protected:
    AEF_TASK_ID m_TaskID;
    char m_TaskName[TASK_NAME_LENGTH];
    int m_TaskPriority;
    int m_TaskOptions;
    int m_TaskStack;
    bool m_bTaskRunning;

    // Keep Global mapping of taskname to QueueId (if it exists)
    static TASK_QUEUE_MAP m_TQMap;
    static CAefMutex m_mutexTQMap;

    // Keep Global status of each task.
    static TASK_STATUS_MAP m_TStatusMap;
    static CAefMutex m_mutexTStatusMap;
    static int m_WatchdogTaskId;
    bool m_bKickTheDog;

    // TODO zzz should this be the base class or the derived class
    CAefQueueTask* m_pStopInformTask;
    uint32_t m_stopInformMsgID;

    bool m_bDeleteMyself;
    
    SEM_ID              m_semTaskStop;                                                  \
    pthread_attr_t      m_attr;                                                  \
    pthread_t           m_threadHandle;                                             \
    static uint32_t   m_ntickspersec;

};

#endif

