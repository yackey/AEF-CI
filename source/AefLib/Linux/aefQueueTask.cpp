/***************************************************************************/
/*!
 **	\file  aefLinuxQueueTask.cpp
 **	\brief Source for CAefQueueTask Class on the VxWorks platform
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
#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */

#include <sys/time.h>
#include <unistd.h>

#include <pthread.h>
#include <string.h>

#include "aefTypesBase.h"
#include "aefQueueTask.h"
#include "aefTimer.h"
//#include "aefTask.h"

using namespace std;

/*************************************************************************/

/*!
 *	\brief	Constructor for CAefQueueTask class.
 *
 *	Default constructor that initializes default settings for tasks in VxWorks.
 *
 ***************************************************************************/
CAefQueueTask::CAefQueueTask() : CAefTask()
{
    Construct();
}

/**************************************************************************/

/*!
 *	\brief	Constructor for CAefQueueTask class.
 *	
 *	Contructor for VxTask objects that configures task characteristics and other defaults
 *	for VxWorks tasks. 
 *	\param	name 		    Name of task.
 *	\param	priority 		Priority of task.
 *	\param	options 		Options for starting the task.
 *	\param	stack 			Stack space needed for task.
 *
 ***************************************************************************/
CAefQueueTask::CAefQueueTask(const char *name, int priority, int options, int stack)
: CAefTask(name, priority, options, stack)
{
    Construct();
}


/**************************************************************************/

/*!
 *	\brief	Default class initialization called from all constructors
 *	
    // mq_getattr and mq_setattr handle these
    // values are visible thru the /proc fs
    // 
 ***************************************************************************/

void CAefQueueTask::Construct()
{
    m_QueueID = MSG_Q_ID_INVALID;
    m_QueueOptions.mq_flags = 0; // O_NONBLOCK is the only choice
    m_QueueOptions.mq_msgsize = MAX_MQ_QUEUE_MESSAGE_SIZE;
    m_QueueOptions.mq_maxmsg = MAX_MQ_QUEUE_MESSAGES;
    m_pTaskTimer = new CAefTimer();

    m_hWdogReportTimer = AEF_INVALID_TIMER_HANDLE;

    // by default, let's go ahead and make all tasks derived from CAefQueueTask
    // responsible for kicking the dog...
    RegisterWatchdogKick();
}

/**************************************************************************/

/*!
 *	\brief	Deconstructor for CAefQueueTask class.
 *	
 *	Cleans up CAefQueueTask object - Deletes buffer for receiving messages.
 *
 ***************************************************************************/
CAefQueueTask::~CAefQueueTask()
{
    delete m_pTaskTimer;
    m_pTaskTimer = NULL;
}

/**************************************************************************/

/*!
 *	\brief	Add function to handle specific message id.	
 *	
 *	Method that maps a method to a specific message id.
 *
 *	\param	messageID 			Message ID to map.
 *	\param	handler 			Callback function to call.
 *
 *	\return AEF_SUCCESS if handler function was successfully added to the map, AEF_ERROR otherwise.
 ***************************************************************************/
AEF_STATUS CAefQueueTask::MapMessage(int messageID, AefCallback handler)
{
    m_TaskMessageMap[messageID] = handler;
    return (AEF_SUCCESS);
}


/**************************************************************************/

/*!
 *	\brief	Add function to handle specific message id.	
 *
 *	Sends TM_INIT message to application object. The application object may 
 *	then perform any initialization before receiving message events. 
 *
 * mq_getattr and mq_setattr handle the queue options
 * values are visible thru the /proc fs
 * queues live in /dev/mqueue
 * the name must begin with a slash
 * 
 * TODO: see if close's happen/in the right spot(s)
 * 
 * 
 ***************************************************************************/
AEF_STATUS CAefQueueTask::Init(void* pTaskData)
{
    AEF_STATUS rc = AEF_ERROR;
    m_sName = m_TaskName;
    m_sName.insert(0, "/");
    m_QueueID = mq_open(m_sName.c_str(), (O_RDWR | O_CREAT), 0666, &m_QueueOptions);

    printf("CAefQueueTask::Init m_QueueID %lu, name '%s'\n", m_QueueID, m_sName.c_str());
    if (m_QueueID != MSG_Q_ID_INVALID)
    {
        if ((rc = CAefTask::Init((void*) m_QueueID)) == AEF_SUCCESS)
        {
            // Initialize Timer "factory"
            m_pTaskTimer->Init(m_QueueID);

            // Let the base Task class handle messages that derived classes may virtually care about
            MapMessage(AEF_TM_TIMER, (AefCallback) & CAefQueueTask::TimerHandler);
            // End Task Message Map

            // if class is already registered to kick the dog, go ahead and set up a
            // report timer.
            if (this->m_bKickTheDog == true)
            {
                if (m_hWdogReportTimer != AEF_INVALID_TIMER_HANDLE)
                {
                    StopTimer(m_hWdogReportTimer);
                }
                m_hWdogReportTimer = StartTimer(AEF_TIMER_WATCHDOG_REPORT_ID, 1000, AEF_TIMER_TYPE_PERIODIC);
            }
        }
        else
        {
            perror("CAefQueueTask::Init - mq_open failed: ");
            m_QueueID = MSG_Q_ID_INVALID;
        }
    }

    return rc;
}

/**************************************************************************/

/*!
 *	\brief	Add function to handle specific message id.
 *	
 *	Sends TM_EXIT to the application object using the MapMessage association. 
 *
 ***************************************************************************/
AEF_STATUS CAefQueueTask::Exit(AEF_STATUS exitCode)
{
    m_pTaskTimer->DeleteAllTimers();
    return (CAefTask::Exit(exitCode));
}

/**************************************************************************/

/*!
 *	\brief  Method that implements the task functionality. 
 *
 *	Method will send the TM_INIT message to the application task using 
 *	the MessageMap associated function. Then the processing loops starts 
 *	and waits for task messages to be recieved on the task's message queue. 
 *	If the task is signalled to stop, this method will send the TM_EXIT message 
 *	to the application task using the MessageMap associated function. 
 *
 ***************************************************************************/
AEF_STATUS CAefQueueTask::Run()
{
    unsigned int nMsgPriority; // maybe not important ??
    struct timespec qTimeSpec;
    clock_gettime(CLOCK_REALTIME, &qTimeSpec);
    qTimeSpec.tv_sec += 2; // Set for 2 seconds

    while (m_bTaskRunning == true)
    {
        ssize_t nRcv = mq_timedreceive(m_QueueID,
                                       (char *) &m_TaskMsg,
                                       sizeof (CAefTaskMessage),
                                       &nMsgPriority,
                                       &qTimeSpec);

        if (nRcv != AEF_ERROR)
        {
            if (m_TaskMessageMap.find(m_TaskMsg.GetId()) != m_TaskMessageMap.end())
            {
                // Call function handler with pointer to the message
                Invoke(m_TaskMessageMap[m_TaskMsg.GetId()], &m_TaskMsg);
            }
            // We can clean up the message here. 
            m_TaskMsg.Cleanup();
        }
    }

    // We are now going to stop this task.

    // Let's stop advertising our message queue ID, so we don't get any more messages
    MSG_Q_ID myQueue = m_QueueID;
    std::string sTaskName = m_TaskName;

    if (m_mutexTQMap.Lock() == true)
    {
        m_TQMap[sTaskName] = 0; // NULL the entry in the queue map
        m_mutexTQMap.Unlock();
    }
    m_QueueID = MSG_Q_ID_INVALID; // NULL out our member variable also

    // In case we've been registered to kick the watchdog, let's go ahead and
    // unregister here, to make sure that none of our cleanup activities
    // henceforth take too long so as to trip the watchdog and cause a reset.

    // in the future, we may want prevent unregistering if we're exiting the task
    // because of an error...
    UnregisterWatchdogKick();

    // Now let's clean up the rest of the messages pending for this queue. 
    struct mq_attr qAttr;   
    clock_gettime(CLOCK_REALTIME, &qTimeSpec);
    qTimeSpec.tv_nsec += (500 * 1000000); // 500 ms
    
    while ((mq_getattr(myQueue, &qAttr) != AEF_ERROR) && (qAttr.mq_curmsgs > 0))
    {
        ssize_t nRcv = mq_timedreceive(m_QueueID,
                                       (char *) &m_TaskMsg,
                                       sizeof (CAefTaskMessage),
                                       &nMsgPriority,
                                       &qTimeSpec);

        if (nRcv != AEF_ERROR)
        {
            // We can clean up the message here. 
            m_TaskMsg.Cleanup();
        }
    }
    int nRc = mq_close(myQueue);
    if (nRc != 0)
    {
        perror("CAefQueueTask::Run mq_close: ");
    }
    m_QueueID = MSG_Q_ID_INVALID;

    return (AEF_SUCCESS);
}

/**************************************************************************/

/*!
 *	\brief Method to send Task Message to another task. 
 *
 *	Each VxTask derived object adds an entry into a static map of 
 *	TaskName, Queue Id. This gives the ability for task to send a 
 *	task message to another task by name without knowing the queue id. 
 *
 *	\param	taskname    Name of task. 
 *	\param	Pointer to Task Message object to send      
 *	\return 	AEF_SUCCESS or AEF_ERROR 
 ***************************************************************************/
AEF_STATUS CAefQueueTask::SendMessage(const char *taskname, CAefTaskMessage *pTM)
{
    AEF_STATUS rc = AEF_ERROR;
    std::string sTaskName = taskname;
    if (m_mutexTQMap.Lock() == true)
    {
        if (m_TQMap.find(sTaskName) != m_TQMap.end())
        {
            rc = SendMessage(m_TQMap[sTaskName], pTM);
        }
        m_mutexTQMap.Unlock();
    }

    return rc;
}


/**************************************************************************/

/*!
 *	\brief	Method that sends a message to a task by queue id.  
 *
 *	Calls VxWorks msgQSend to send a task message to a message queue speficied by 
 *	queue id. 
 *
 *	\param	msgTaskId   Message queue id. (for VxWorks at least)
 *	\param	ticks       Pointer to Task Message object to send      
 *	\return AEF_SUCCESS or AEF_ERROR 
 ***************************************************************************/
AEF_STATUS CAefQueueTask::SendMessage(AEF_MSG_TASK_ID msgTaskId, CAefTaskMessage* pTM)
{
    AEF_STATUS result = AEF_ERROR;
    if ((pTM != NULL) && (msgTaskId != MSG_Q_ID_INVALID))
    {
        int nSnd = mq_send(msgTaskId, (char *) pTM, sizeof (CAefTaskMessage), MSG_PRI_NORMAL);
        // TODO: vxWorks stuff says NO_WAIT - what does that mean ?
        //      (hint: it does not mean use O_NONBLOCK for the queue 
        if (nSnd == AEF_SUCCESS)
        {
            result = AEF_SUCCESS;
        }
    }
    return result;
}

/**************************************************************************/

/*!
 *   \brief  Method that sends a message to "this" task by queue id.  
 *
 *   Calls VxWorks msgQSend to send a task message to the message queue
 *   associated with this task. 
 *
 *   \param  pTM	Pointer to Task Message object to send      
 *   \return 		AEF_SUCCESS or AEF_ERROR 
 ***************************************************************************/
AEF_STATUS CAefQueueTask::SendMessage(CAefTaskMessage* pTM)
{
    AEF_STATUS result = AEF_ERROR;
    if ((m_bTaskRunning == true) && (m_QueueID != MSG_Q_ID_INVALID))
    {
        int nSnd = mq_send(m_QueueID, (char *) pTM, sizeof (CAefTaskMessage), MSG_PRI_NORMAL);
        // TODO: vxWorks stuff says NO_WAIT - what does that mean ?
        //      (hint: it does not mean use O_NONBLOCK for the queue
        if (nSnd == AEF_SUCCESS)
        {
            result = AEF_SUCCESS;
        }
    }
    return result;
}

/**************************************************************************/

/*!
 *   \brief  Method that sends a message to ALL tasks.  
 *
 *   Calls VxWorks msgQSend to send a task message to the message queue
 *   associated with all AEF tasks. 
 *
 *   \param  pTM	Pointer to Task Message object to send      
 *   \return 		AEF_SUCCESS or AEF_ERROR 
 ***************************************************************************/
AEF_STATUS CAefQueueTask::SendBroadcastMessage(CAefTaskMessage* pTM)
{
    AEF_STATUS rc = AEF_ERROR;
    TASK_QUEUE_MAP::iterator itTask;
    MSG_Q_ID taskQueue;

    if (m_mutexTQMap.Lock() == true)
    {
        rc = AEF_SUCCESS;

        for (itTask = m_TQMap.begin(); itTask != m_TQMap.end(); ++itTask)
        {
            taskQueue = (MSG_Q_ID) (itTask->second);
            if (taskQueue != 0)
            {
                SendMessage(taskQueue, pTM);
            }
        }
        m_mutexTQMap.Unlock();
    }

    return rc;
}

/**************************************************************************/

/*!
 *	\brief Starts the application timer.
 *
 *	The StartTimer method creates and starts a VxWorks watchdog timer. 
 *	This VxTimer object will then send a task message when the timer fires.
 *
 *	\param	id 		Application Timer Id.
 *	\param	ms		Number of milliseconds before the timer should expire.
 *	\param	type	Type of timer, either one-shot or periodic
 *	\param	pVoid	Garbage-in, garbage out void pointer
 *
 *	\return 			Returns VxWorks timer.  
 ***************************************************************************/
AEF_TIMER_HANDLE CAefQueueTask::StartTimer(UNSIGNED32 id, UNSIGNED32 ms, UNSIGNED8 type, void* pVoid)
{
    AEF_TIMER_HANDLE hTimer = 0;  
    if (IsRunning() == true)
    {
        hTimer = m_pTaskTimer->Start(id, ms, type, pVoid);
    }
    return hTimer;
}

/**************************************************************************/

/*!
 *	\brief	Cancel Timer 
 *
 *	Method that can be used to cancel a timer. This method can be called before
 *	a timer expires or at any time for a Periodic timer. 
 *
 *	\param	htimer 		Timer handle id.
 *
 ***************************************************************************/
void CAefQueueTask::StopTimer(AEF_TIMER_HANDLE hTimer)
{
    if (IsRunning() == true)
    {
        m_pTaskTimer->Stop(hTimer);
    }
}

/**************************************************************************/

/*!
 *	\brief Base-class OnTimer method. 
 *
 *	Base class method that must be called by the application object.
 *
 *	\param	msg 	Pointer to Task Message object.
 ***************************************************************************/
void CAefQueueTask::OnTimer(UNSIGNED32 timerId, void* pVoid)
{
    // fill this in if this class needs to handle any timers.
    switch (timerId)
    {
    case AEF_TIMER_WATCHDOG_REPORT_ID:
        WatchdogReport();
        break;
    }
}

/**************************************************************************/

/*!
 *	\brief AEF_TM_TIMER message handler
 *
 *	Method that handles all timers for this class (and derived classes).
 *	It will handle deleting the watchdog timer and is responsible for
 *	restarting the timer if the type is Periodic. 
 *
 *	\param	msg 	Pointer to Task Message object.
 *	\return 		Returns VxWorks timer.  
 ***************************************************************************/
void CAefQueueTask::TimerHandler(void *pMsg)
{
    CAefTaskMessage *pTm = (CAefTaskMessage*) pMsg;
    AEF_TIMER_HANDLE hTimer;
    UNSIGNED32 timerId;
    void* pVoid;
    if ((pTm->GetData((unsigned char*) &hTimer, sizeof (AEF_TIMER_HANDLE)) == AEF_SUCCESS) &&
            (m_pTaskTimer->GetTimerClientInfo(hTimer, timerId, pVoid) == true))
    {
        // call the timer class's handler method to perform cleanup if necessary
        m_pTaskTimer->HandleTimer(hTimer);

        // call the virtual OnTimer method
        OnTimer(timerId, pVoid);
    }
}

/**************************************************************************/

/*!
 *	\brief	Register this task's intention to report it's "alive" status periodically
 *
 *	\return AEF_SUCCESS if task is successfully registered, AEF_ERROR otherwise
 ***************************************************************************/
AEF_STATUS CAefQueueTask::RegisterWatchdogKick()
{
    AEF_STATUS rc = AEF_ERROR;
    CAefTask::RegisterWatchdogKick();
    if ((this->m_bKickTheDog == true) && (IsRunning() == true))
    {
        if (m_hWdogReportTimer != AEF_INVALID_TIMER_HANDLE)
        {
            StopTimer(m_hWdogReportTimer);
        }
        m_hWdogReportTimer = StartTimer(AEF_TIMER_WATCHDOG_REPORT_ID, 1000, AEF_TIMER_TYPE_PERIODIC);
        rc = AEF_SUCCESS;
    }
    return rc;
}


/**************************************************************************/

/*!
 *	\brief	Remove this task from the list of tasks reporting periodic status
 *
 *	\return AEF_SUCCESS if task is removed from the list, AEF_ERROR otherwise
 ***************************************************************************/
AEF_STATUS CAefQueueTask::UnregisterWatchdogKick()
{
    AEF_STATUS rc;
    if ((rc = CAefTask::UnregisterWatchdogKick()) == AEF_SUCCESS)
    {
        // stop the report timer, if it's running.  If the check timer is
        // running, keep it running...
        if (m_hWdogReportTimer != AEF_INVALID_TIMER_HANDLE)
        {
            StopTimer(m_hWdogReportTimer);
            m_hWdogReportTimer = AEF_INVALID_TIMER_HANDLE;
        }
    }
    return rc;
}

