/***************************************************************************/
/*!
 **	\file  aefLinuxQueueTask.cpp
 **	\brief Source for CAefQueueTaskBase Class on the VxWorks platform
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
#include "aefQueueTaskBase.h"

/*************************************************************************/

/*!
 *	\brief	Constructor for CAefQueueTaskBase class.
 *
 *	Default constructor that initializes default settings for tasks in VxWorks.
 *
 ***************************************************************************/
CAefQueueTaskBase::CAefQueueTaskBase()
{
}
//
///**************************************************************************/
//
///*!
// *	\brief	Deconstructor for CAefQueueTaskBase class.
// *	
// *	Cleans up CAefQueueTaskBase object - Deletes buffer for receiving messages.
// *
// ***************************************************************************/
CAefQueueTaskBase::~CAefQueueTaskBase()
{
}

///**************************************************************************/
//
///*!
// *	\brief	Add function to handle specific message id.	
// *	
// *	Method that maps a method to a specific message id.
// *
// *	\param	messageID 			Message ID to map.
// *	\param	handler 			Callback function to call.
// *
// *	\return AEF_SUCCESS if handler function was successfully added to the map, AEF_ERROR otherwise.
// ***************************************************************************/
//AEF_STATUS CAefQueueTaskBase::MapMessage(int messageID, AefCallback handler)
//{
//    m_TaskMessageMap[messageID] = handler;
//    return (AEF_SUCCESS);
//}
//
///**************************************************************************/
//
///*!
// *	\brief	Add function to handle specific message id.
// *	
// *	Sends TM_EXIT to the application object using the MapMessage association. 
// *
// ***************************************************************************/
//AEF_STATUS CAefQueueTaskBase::Exit(AEF_STATUS exitCode)
//{
//    m_pTaskTimer->DeleteAllTimers();
//    return (CAefTask::Exit(exitCode));
//}
//
///**************************************************************************/
//
///*!
// *	\brief Method to send Task Message to another task. 
// *
// *	Each VxTask derived object adds an entry into a static map of 
// *	TaskName, Queue Id. This gives the ability for task to send a 
// *	task message to another task by name without knowing the queue id. 
// *
// *	\param	taskname    Name of task. 
// *	\param	Pointer to Task Message object to send      
// *	\return 	AEF_SUCCESS or AEF_ERROR 
// ***************************************************************************/
//AEF_STATUS CAefQueueTaskBase::SendMessage(const char *taskname, CAefTaskMessage *pTM)
//{
//    AEF_STATUS rc = AEF_ERROR;
//    std::string sTaskName = taskname;
//    if (m_mutexTQMap.Lock() == true)
//    {
//        if (m_TQMap.find(sTaskName) != m_TQMap.end())
//        {
//            rc = SendMessage(m_TQMap[sTaskName], pTM);
//        }
//        m_mutexTQMap.Unlock();
//    }
//
//    return rc;
//}
//
///**************************************************************************/
//
///*!
// *   \brief  Method that sends a message to ALL tasks.  
// *
// *   Calls VxWorks msgQSend to send a task message to the message queue
// *   associated with all AEF tasks. 
// *
// *   \param  pTM	Pointer to Task Message object to send      
// *   \return 		AEF_SUCCESS or AEF_ERROR 
// ***************************************************************************/
//AEF_STATUS CAefQueueTaskBase::SendBroadcastMessage(CAefTaskMessage* pTM)
//{
//    AEF_STATUS rc = AEF_ERROR;
//    TASK_QUEUE_MAP::iterator itTask;
//    MSG_Q_ID taskQueue;
//
//    if (m_mutexTQMap.Lock() == true)
//    {
//        rc = AEF_SUCCESS;
//
//        for (itTask = m_TQMap.begin(); itTask != m_TQMap.end(); ++itTask)
//        {
//            taskQueue = (MSG_Q_ID) (itTask->second);
//            if (taskQueue != 0)
//            {
//                SendMessage(taskQueue, pTM);
//            }
//        }
//        m_mutexTQMap.Unlock();
//    }
//
//    return rc;
//}
//
///**************************************************************************/
//
///*!
// *	\brief Starts the application timer.
// *
// *	The StartTimer method creates and starts a VxWorks watchdog timer. 
// *	This VxTimer object will then send a task message when the timer fires.
// *
// *	\param	id 		Application Timer Id.
// *	\param	ms		Number of milliseconds before the timer should expire.
// *	\param	type	Type of timer, either one-shot or periodic
// *	\param	pVoid	Garbage-in, garbage out void pointer
// *
// *	\return 			Returns VxWorks timer.  
// ***************************************************************************/
//AEF_TIMER_HANDLE CAefQueueTaskBase::StartTimer(UNSIGNED32 id, UNSIGNED32 ms, UNSIGNED8 type, void* pVoid)
//{
//    AEF_TIMER_HANDLE hTimer = 0;  
//    if (IsRunning() == true)
//    {
//        hTimer = m_pTaskTimer->Start(id, ms, type, pVoid);
//    }
//    return hTimer;
//}
//
///**************************************************************************/
//
///*!
// *	\brief	Cancel Timer 
// *
// *	Method that can be used to cancel a timer. This method can be called before
// *	a timer expires or at any time for a Periodic timer. 
// *
// *	\param	htimer 		Timer handle id.
// *
// ***************************************************************************/
//void CAefQueueTaskBase::StopTimer(AEF_TIMER_HANDLE hTimer)
//{
//    if (IsRunning() == true)
//    {
//        m_pTaskTimer->Stop(hTimer);
//    }
//}
//
///**************************************************************************/
//
///*!
// *	\brief Base-class OnTimer method. 
// *
// *	Base class method that must be called by the application object.
// *
// *	\param	msg 	Pointer to Task Message object.
// ***************************************************************************/
//void CAefQueueTaskBase::OnTimer(UNSIGNED32 timerId, void* pVoid)
//{
//    // fill this in if this class needs to handle any timers.
//    switch (timerId)
//    {
//    case AEF_TIMER_WATCHDOG_REPORT_ID:
//        WatchdogReport();
//        break;
//    }
//}
//
///**************************************************************************/
//
///*!
// *	\brief AEF_TM_TIMER message handler
// *
// *	Method that handles all timers for this class (and derived classes).
// *	It will handle deleting the watchdog timer and is responsible for
// *	restarting the timer if the type is Periodic. 
// *
// *	\param	msg 	Pointer to Task Message object.
// *	\return 		Returns VxWorks timer.  
// ***************************************************************************/
//void CAefQueueTaskBase::TimerHandler(void *pMsg)
//{
//    CAefTaskMessage *pTm = (CAefTaskMessage*) pMsg;
//    AEF_TIMER_HANDLE hTimer;
//    UNSIGNED32 timerId;
//    void* pVoid;
//    if ((pTm->GetData((unsigned char*) &hTimer, sizeof (AEF_TIMER_HANDLE)) == AEF_SUCCESS) &&
//            (m_pTaskTimer->GetTimerClientInfo(hTimer, timerId, pVoid) == true))
//    {
//        // call the timer class's handler method to perform cleanup if necessary
//        m_pTaskTimer->HandleTimer(hTimer);
//
//        // call the virtual OnTimer method
//        OnTimer(timerId, pVoid);
//    }
//}
//
///**************************************************************************/
//
///*!
// *	\brief	Register this task's intention to report it's "alive" status periodically
// *
// *	\return AEF_SUCCESS if task is successfully registered, AEF_ERROR otherwise
// ***************************************************************************/
//AEF_STATUS CAefQueueTaskBase::RegisterWatchdogKick()
//{
//    AEF_STATUS rc = AEF_ERROR;
//    CAefTask::RegisterWatchdogKick();
//    if ((this->m_bKickTheDog == true) && (IsRunning() == true))
//    {
//        if (m_hWdogReportTimer != AEF_INVALID_TIMER_HANDLE)
//        {
//            StopTimer(m_hWdogReportTimer);
//        }
//        m_hWdogReportTimer = StartTimer(AEF_TIMER_WATCHDOG_REPORT_ID, 1000, AEF_TIMER_TYPE_PERIODIC);
//        rc = AEF_SUCCESS;
//    }
//    return rc;
//}
//
//
///**************************************************************************/
//
///*!
// *	\brief	Remove this task from the list of tasks reporting periodic status
// *
// *	\return AEF_SUCCESS if task is removed from the list, AEF_ERROR otherwise
// ***************************************************************************/
//AEF_STATUS CAefQueueTaskBase::UnregisterWatchdogKick()
//{
//    AEF_STATUS rc;
//    if ((rc = CAefTask::UnregisterWatchdogKick()) == AEF_SUCCESS)
//    {
//        // stop the report timer, if it's running.  If the check timer is
//        // running, keep it running...
//        if (m_hWdogReportTimer != AEF_INVALID_TIMER_HANDLE)
//        {
//            StopTimer(m_hWdogReportTimer);
//            m_hWdogReportTimer = AEF_INVALID_TIMER_HANDLE;
//        }
//    }
//    return rc;
//}
//
