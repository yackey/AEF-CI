/***************************************************************************/
/*!
 **	\file 	aefTask.h
 **	\brief Basic Task Class
 **
 **	CAefTaskBase provides the basic infrastructure for a task or thread with
 **  a processing loop.  Classes derived from CAefTaskBase can choose whether
 **  or not to participate in kicking the watchdog.
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
#ifndef _aefTaskBase_h
#define _aefTaskBase_h

#include <aefTypesBase.h>
class CAefQueueTask;

/*!
 *	\class CAefTaskBase
 *	\brief Class to implement Tasks.
 *
 *	The abstract CAefTaskBase object defines the interface of a basic
 *	task with an undefined main execution loop.  The task is tracked
 *	in a map of all tasks, and it's possible for classes derived
 *	from CAefTaskBase to kick the software watchdog if they're capable
 *   (and willing).
 */
class CAefTaskBase
{
    // Class Methods
public:
    CAefTaskBase();
    virtual ~CAefTaskBase();

    virtual AEF_STATUS Start() = 0;
    virtual AEF_STATUS Stop(UNSIGNED32 msWait = AEF_WAIT_FOREVER) = 0;
    virtual AEF_STATUS Stop(CAefQueueTask* pTask, UNSIGNED32 msgID) = 0;

    virtual char* GetTaskName() = 0;
    virtual void SetTaskName(const char *name) = 0;
    virtual int GetTaskPriority() = 0;
    virtual void SetTaskPriority(int priority) = 0;
    virtual int GetTaskOptions() = 0;
    virtual void SetTaskOptions(int options) = 0;
    virtual int GetTaskStackSize() = 0;
    virtual void SetTaskStackSize(int stack) = 0;
    virtual inline bool IsRunning() = 0;
    virtual void SetDeleteFlag() = 0;
    virtual AEF_STATUS TakeWatchdogOwnership() = 0;
    virtual AEF_STATUS ReleaseWatchdogOwnership() = 0;
    virtual AEF_STATUS RegisterWatchdogKick() = 0;
    virtual AEF_STATUS UnregisterWatchdogKick() = 0;
    virtual void TaskDelay(UNSIGNED16 ms) = 0;
    ;

protected:
    virtual AEF_STATUS Init(void* pTaskData = NULL) = 0;
    virtual AEF_STATUS Exit(AEF_STATUS exitCode) = 0;
    virtual AEF_STATUS RunWrapper() = 0;
    virtual AEF_STATUS Run() = 0;
    virtual void WatchdogReport() = 0;
    virtual bool WatchdogCheck(UNSIGNED32 *task_id = NULL) = 0;

};

#endif //_aefTaskBase_h
