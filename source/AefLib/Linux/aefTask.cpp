/***************************************************************************/
/*!
**	\file  aefLinuxTask.cpp
**	\brief Source for VxTask Class implementation
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

//#include "aefTimerBase.h"
#include "aefTask.h"
#include "aefMutex.h"
//#include "aefTaskMessageBase.h"
#include "aefQueueTask.h"

#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

TASK_QUEUE_MAP		CAefTask::m_TQMap;
CAefMutex		CAefTask::m_mutexTQMap;

TASK_STATUS_MAP		CAefTask::m_TStatusMap;
CAefMutex		CAefTask::m_mutexTStatusMap;
int			CAefTask::m_WatchdogTaskId = AEF_INVALID_TASK_ID;
UNSIGNED32              CAefTask::m_ntickspersec;

/*************************************************************************/
/*!
*	\brief	Constructor for CAefTask class.
*
*	Default Constructor that initializes default settings for tasks in VxWorks.
*
***************************************************************************/
CAefTask::CAefTask()
{
    Construct("", 100, 0, 2000);
}

/**************************************************************************/
/*!
*	\brief	Constructor for CAefTask class.
*
*	Contructor for VxTask objects that configures task characteristics and other defaults
*	for VxWorks tasks.
*	\param	name 		    Name of task.
*	\param	priority 		Priority of task.
*	\param	options 		Options for starting the task.
*	\param	stack 			Stack space needed for task.
*
***************************************************************************/
CAefTask::CAefTask(const char *name, int priority, int options, int stack)
{
	Construct(name, priority, options, stack);
}


/**************************************************************************/
/*!
*	\brief	Method that performs default initialization of class members.  Called from all constructors.
*
*	\param	name 		    Name of task.
*	\param	priority 		Priority of task.
*	\param	options 		Options for starting the task.
*	\param	stack 			Stack space needed for task.
*
***************************************************************************/
void CAefTask::Construct(const char *name, int priority, int options, int stack)
{
        // we want a binary semaphore
	sem_init(&m_semTaskStop, 0 ,1);
        m_TaskID = AEF_INVALID_TASK_ID;
	m_bTaskRunning = false;
	m_bKickTheDog = false;

	SetTaskName(name);
        // TODO: reconcile in pthread land
	m_TaskPriority = priority;
	m_TaskOptions = options ;   // TODO does this mea anything ??| VX_FP_TASK;
        // TODO: for example here is some form of reconciliation
	m_TaskStack = (stack > PTHREAD_STACK_MIN) ? stack : PTHREAD_STACK_MIN;
	m_threadHandle = INVALID_THREAD_HANDLE;
	m_pStopInformTask = NULL;
	m_bDeleteMyself = false;
        m_ntickspersec = sysconf(_SC_CLK_TCK);
}

/**************************************************************************/
/*!
*	\brief	Deconstructor for CAefTask class.
*
*	Cleans up CAefTask object
*
***************************************************************************/
CAefTask::~CAefTask()
{
    // TODO: take note/do something about sem_init failures
    sem_destroy(&m_semTaskStop);
}

/**************************************************************************/
/*!
*	\brief	Method that starts task.
*
*	Method will spawn the task.
*
*	\return AEF_SUCCESS if task could be started, AEF_ERROR otherwise.
*
***************************************************************************/
AEF_STATUS CAefTask::Start()
{
    int nRc = AEF_ERROR;
    if (m_TaskID == AEF_INVALID_TASK_ID)
    {
        // any time we start the task, make sure we reset a few things
        m_pStopInformTask = NULL; // no one has registered they want to know when we stop
        m_bDeleteMyself = false; // we won't delete ourselves unless we decide to after we're started.

        // TODO: come up with some 'unified' error handling scheme
        //  at least syslog 
        //  maybe an exception ??
        int nRai = pthread_attr_init(&m_attr);
        if (nRai != 0)
        {
            perror("pthread_attr_init: ");
        }

        if (m_TaskStack > PTHREAD_STACK_MIN) 
        {
            // TODO: how to handle these 
            //  like construct enforces a minimum for this
            //  Right spot ??
            nRai = pthread_attr_setstacksize(&m_attr, m_TaskStack);
            if (nRai != 0)
            {
                // TODO: perror doesn't work on these - YHGTBSM !!
                printf("CAefTask::Start pthread_attr_setstacksize failed (%d)\n", nRai);
            }
        }
        //TODO: map other options to a pthread_attr, e.g., m_TaskPriority, m_TaskOptions
        nRc = pthread_create(&m_threadHandle, &m_attr, &CAefTask::ThreadRun, this);
        if (nRc != AEF_SUCCESS)
        {
             // TODO: more creative error reporting syslog ?
            perror("CAefTask::Start - pthread_create failed: ");
            m_threadHandle = INVALID_THREAD_HANDLE;
            pthread_attr_destroy(&m_attr);               
        }
        else
        {
            m_TaskID = m_threadHandle;
        }
    }
    return nRc;  
}

/**************************************************************************/
/*!
*	\brief	Method to Stop task.
*
*	Method stops the task from executing. The task can be restarted
*	by calling Start().
*
*	\return AEF_SUCCESS, or AEF_ERROR if caller chose to wait and the task hasn't stopped at the end of the timeout period.
***************************************************************************/
AEF_STATUS CAefTask::Stop(UNSIGNED32 msWait)
{
    AEF_STATUS result = AEF_SUCCESS;
    // Stop Processing Loop
    m_bTaskRunning = false;
    // we can only check the semaphore if we weren't called
    // in the context of the running task (duh).
    if (pthread_self() != m_TaskID)
    {
        // The assumption is you just set the time values to 0 and call timed wait
        //  for an immediate return ??
        // So just one call needed
        struct timespec ts;
        ts.tv_nsec = 0;
        ts.tv_sec = 0;
        if (msWait != AEF_WAIT_FOREVER)
        {
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
            {
                perror("CAefTask::Stop clock_gettime: ");
                // no half baked stuff from error
                ts.tv_nsec = 0;
                ts.tv_sec = 0;
            }
            else
            {
                time_t nSec = (msWait/1000);
                long nNanoSec = ((msWait % 1000) * 1000000);
                ts.tv_nsec = nNanoSec;
                ts.tv_sec = nSec;
            }
        }
        
        if ((sem_timedwait(&m_semTaskStop, &ts)) == AEF_SUCCESS)
        {
            // we got the semaphore, which means the task
            // gave it up before it died.  Now give it back
            // in case the task needs it to start up again.
            sem_post(&m_semTaskStop);
        }
        else
        {
            result = AEF_ERROR;
        }
    }
    return result;
}

/**************************************************************************/
/*!
*  \brief  Method to Stop task.  Caller will be informed via message when task stops
*
*  The task can be restarted by calling Start(), once confirmation is received that it actually stopped.
*
*  \param  pTask   pointer to AefQueueTask that wants to know when this task stops
*  \param  msgID   messageID to send when this task stops
*
*  \return AEF_SUCCESS
***************************************************************************/
AEF_STATUS CAefTask::Stop(CAefQueueTask* pTask, UNSIGNED32 msgID)
{
	// save off info of who we should inform when the task has actually stopped
	m_pStopInformTask = pTask;
	m_stopInformMsgID = msgID;
	// Stop Processing Loop
	m_bTaskRunning = false;
	return AEF_SUCCESS;
}

/**************************************************************************/
/*!
*	\brief	Stores a name to associated with this class/task
*
*	\param	name	task name to be used.
*
***************************************************************************/
void CAefTask::SetTaskName(const char *name)
{
    if (name != NULL) 
    {
        // truncate the name if it's larger than the string variable
        // we're using to store it
        if (strlen(name) < TASK_NAME_LENGTH) 
        {
                strcpy(m_TaskName, name);
        }
        else 
        {
                strncpy(m_TaskName, name, TASK_NAME_LENGTH-1);
        }
    }
}

/**************************************************************************/
/*!
*	\brief	Attempt to become the task that "owns" the watchdog and is required to check it periodically
*
*	\return AEF_SUCCESS if task successfully registers as the owner, AEF_ERROR if another task already owns it
***************************************************************************/
AEF_STATUS CAefTask::TakeWatchdogOwnership()
{
    AEF_STATUS	rc = AEF_ERROR;

    // to take ownership, our task must be running (i.e., we must have
    // a valid task ID), and no one else can have already taken ownership
    // yet.
    if (m_mutexTStatusMap.Lock() == true)
    {
        if ((IsRunning() == true) && (m_WatchdogTaskId == AEF_INVALID_TASK_ID)) {
                m_WatchdogTaskId = m_TaskID;
                rc = AEF_SUCCESS;
        }
        m_mutexTStatusMap.Unlock();
    }

    return rc;
}

/**************************************************************************/
/*!
*	\brief	Relenquish ownership of the watchdog.
*
*   Note: This call should be made after the task is running (e.g., in the Init() routine, perhaps)!!!
*
*	\return AEF_SUCCESS if task successfully relenquishes ownership, AEF_ERROR if it fails
***************************************************************************/
AEF_STATUS CAefTask::ReleaseWatchdogOwnership()
{
    AEF_STATUS	rc = AEF_ERROR;

    // if task is shutting down, this method must be called
    // before the task ID is erased!
    if (m_mutexTStatusMap.Lock() == true)
    {
        if (m_WatchdogTaskId == m_TaskID) 
        {
                m_WatchdogTaskId = AEF_INVALID_TASK_ID;
                rc = AEF_SUCCESS;
        }
        m_mutexTStatusMap.Unlock();
    }

    return rc;
}

/**************************************************************************/
/*!
*	\brief	Register this task's intention to report it's "alive" status periodically
*
*	\return AEF_SUCCESS if task is successfully registered, AEF_ERROR otherwise
***************************************************************************/
AEF_STATUS CAefTask::RegisterWatchdogKick()
{
    AEF_STATUS	rc = AEF_ERROR;
    if (m_bKickTheDog == false) 
    {
        m_bKickTheDog = true;
        if (IsRunning() == true) 
        {
            // go ahead and kick it for the first time
            WatchdogReport();
        }
        rc = AEF_SUCCESS;
    }
    // if task is already registered, then this registration attempt fails
    return rc;
}

/**************************************************************************/
/*!
*	\brief	Remove this task from the list of tasks reporting periodic status
*
*	\return AEF_SUCCESS if task is removed from the list, AEF_ERROR otherwise
***************************************************************************/
AEF_STATUS CAefTask::UnregisterWatchdogKick()
{
    AEF_STATUS	rc = AEF_ERROR;

    if (m_bKickTheDog == true) 
    {
        if (m_mutexTStatusMap.Lock() == true)
        {
                m_TStatusMap.erase(m_TaskID);
                m_mutexTStatusMap.Unlock();
                m_bKickTheDog = false;
                rc = AEF_SUCCESS;
        }
    }
    // if task isn't currently registered, then this unregistration attempt fails

    return rc;
}


/**************************************************************************/
/*!
*	\brief	Method to initialize the task just after it has started
*
*	\return AEF_SUCCESS if task was Initialized successfully, AEF_ERROR otherwise
*
***************************************************************************/
AEF_STATUS CAefTask::Init(void* pTaskData)
{
    std::string	sTaskName = m_TaskName;

    // add this task to the list of running tasks
    if (m_mutexTQMap.Lock() == true) 
    {
        // this base class doesn't have a message queue, but maybe a derived class does?
        m_TQMap[sTaskName] = (MSG_Q_ID)pTaskData;
        m_mutexTQMap.Unlock();
    }

    return (AEF_SUCCESS);
}

/**************************************************************************/
/*!
*	\brief	Method to clean up the task just before it exists
*
*	\return AEF_SUCCESS, unless task class is actually deleted as part of this call
*
***************************************************************************/
AEF_STATUS CAefTask::Exit(AEF_STATUS exitCode) 
{
    std::string sTaskName = m_TaskName;

    // remove this task to the list of running tasks
    if (m_mutexTQMap.Lock() == true) 
    {
        m_TQMap.erase(sTaskName);
        m_mutexTQMap.Unlock();
    }

    // call this before we invalidate the task ID, just in case
    // the derived class hasn't called it yet...
    ReleaseWatchdogOwnership();

    m_TaskID = AEF_INVALID_TASK_ID;

    return (AEF_SUCCESS);
}


/**************************************************************************/
/*!
*	\brief  Static function executed by taskSpawn that in turn will call the instance Run method.
*
*	Method uses object pointer argument to call the Run method.
*
*	\param	pTaskClass  Pointer to instance of task object.
*
*	\return Not used, but will always return zero.
***************************************************************************/
void* CAefTask::ThreadRun(void* vp)
{
    CAefTask* pTaskClass = (CAefTask*)vp;
    pTaskClass->RunWrapper();
    return ((void*)0);
}

/**************************************************************************/
/*!
*	\brief  "Wrapper" around the main execution loop.
*
*	Method allows for intialization when task begins, and cleanup when
*	task ends.
*
***************************************************************************/
AEF_STATUS CAefTask::RunWrapper()
{
    AEF_STATUS rc;
    sem_wait(&m_semTaskStop);
    m_bTaskRunning = true;
    // Initialize the task
    if (Init() == AEF_SUCCESS)
    {
        rc = Run();
    }
    else
    {
        rc = AEF_ERROR;
    }
    // clean up before the task exits
    Exit(rc);
    // give the semaphore back (synchronous indication that task has stopped)
    sem_post(&m_semTaskStop);
    // if a task has registered, send a "we've stopped" message (asynchronous indication)
    if ((m_pStopInformTask != NULL) && (m_pStopInformTask->IsRunning() == true))
    {
        CAefTaskMessage tm(m_stopInformMsgID);
        void* pThis = (void*) this;
        // data is the "this" pointer
        tm.SetData((unsigned char*) &pThis, sizeof (void*));
        m_pStopInformTask->SendMessage(&tm);
    }
    // finally, if we've been asked to delete ourselves, go ahead and do it
    if (m_bDeleteMyself == true)
    {
        delete this;
    }
    return rc;
}

/**************************************************************************/

/*!
 *	\brief Base-class WatchdogReport method.
 *
 ***************************************************************************/
void CAefTask::WatchdogReport()
{
    if (m_bKickTheDog == true)
    {
        if (m_mutexTStatusMap.Lock() == true)
        {
            m_TStatusMap[m_TaskID] |= AEF_TASK_REPORTING;
            m_mutexTStatusMap.Unlock();
        }
    }
}

/**************************************************************************/
/*!
*	\brief Base-class WatchdogCheck method.
*
***************************************************************************/
bool CAefTask::WatchdogCheck(UNSIGNED32 *task_id)
{
    bool bCheck = true;
    TASK_STATUS_MAP::iterator it;
    int reporting;
    if (m_mutexTStatusMap.Lock() == true)
    {
        if (m_WatchdogTaskId == m_TaskID)
        {
            // Update ourselves if we're supposed to be kicking too
            if (m_bKickTheDog == true)
            {
                m_TStatusMap[m_TaskID] |= AEF_TASK_REPORTING;
            }
            // printf("%s: WatchdogCheck\n", m_TaskName);

            for (it = m_TStatusMap.begin(); it != m_TStatusMap.end(); ++it)
            {
                // printf("Checking Task: %d\n", it->first);
                reporting = it->second;
                // Check if the reporting bit is off, if so then the bit is
                // off since the last time we checked the status.
                if (!(reporting & AEF_TASK_REPORTING))
                {
                    //We have one task not reporting, we will need to restart
                    //printf("Stop Kicking Hardware Watchdog (%s died)\n", taskName(it->first));
                    bCheck = false;
                    if (task_id != NULL)
                    {
                        *task_id = it->first;
                    }
                }
                else
                {
                    // The bit is still set, so let's clear it and check
                    // next time.
                    (it->second) &= ~(AEF_TASK_REPORTING);
                }
            }
        }
        m_mutexTStatusMap.Unlock();
    }
    return bCheck;
}

/**************************************************************************/
/*!
*   \brief Method to retrieve a basic tick count
*
*   \return The number of kernel "ticks" since the OS was started.
*
***************************************************************************/
UNSIGNED32 CAefTask::GetTimestamp()
{
    struct sysinfo si;
    sysinfo(&si);
    UNSIGNED32 nSec = si.uptime;
    return MsToTimeTicks(nSec * 1000);
}


/**************************************************************************/
/*!
*   \brief Convert a millisecond value into a tick count
*
*   \return The number "ticks" equal to the number of milliseconds passed in
*
***************************************************************************/
UNSIGNED32 CAefTask::MsToTimeTicks(UNSIGNED32 ms)
{
    UNSIGNED32 nTps = (ms/1000) * CAefTask::m_ntickspersec;
    return nTps;
}


/**************************************************************************/
/*!
*   \brief Method to delay the task while allowing other tasks to take the CPU
*
*   \param  ms  The number of milliseconds to delay/sleep for
*
*   \return void
*
***************************************************************************/
void CAefTask::TaskDelay(UNSIGNED16 ms)
{
    useconds_t nUsecDelay = (ms * 1000);
    // TODO: is this even almost correct
    // could calculate ticks per second; but for now assume 20 ms is close to one tick
    for ( int nMsPerTick = 20 ; nUsecDelay > 0 ; )
    {
        pthread_testcancel();
        usleep(nMsPerTick);
        nUsecDelay -= nMsPerTick;
    }
}
