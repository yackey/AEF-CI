/***************************************************************************/
/*!
 **	\file  aefLinuxTimer.cpp
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
#include "aefBase.h"
#include "aefTypesBase.h"
#include "aefTaskMessage.h"
//#include "aefTask.h"
#include "aefTimer.h"
#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <mqueue.h>

/**************************************************************************/

/*!
 *	\brief Default Constructor for CAefTimer object.
 *
 ***************************************************************************/
CAefTimer::CAefTimer()
{
    m_TimerInstance = 1; // 0 is an invalid instance identifier
}


/**************************************************************************/

/*!
 *	\brief Empty Destructor for CAefTimer object.
 *
 ***************************************************************************/
CAefTimer::~CAefTimer()
{
    DeleteAllTimers();
}


/**************************************************************************/

/*!
 *	\brief Initialized the VxTimer object. 
 *
 *	Initializes the VxTimer object with the task owner information and 
 *	task message queue. This must be called to allow the timer infrastructure
 *	to send the AEF_TM_TIMER message and to limit the timer to the owning task id. 
 *
 *	\param	MSG_Q_ID 	queue_id 	Task Queue Id.
 *
 *	\return WDOG_ID					Returns VxWorks timer.  
 ***************************************************************************/
void CAefTimer::Init(AEF_TIMER_TASK_ID timerTaskId)
{
    m_TimerTaskId = timerTaskId;
}

/**************************************************************************/

/*!
 *	\brief Starts the application timer.
 *
 *	The StartTimer method creates and starts a VxWorks watchdog timer. 
 *	This VxTimer object will then send a task message when the timer fires.
 *
 *	\param		id 		Application Timer Id.
 *	\param		ms		Number of milliseconds before the timer should expire.
 *	\param		type	Timer type, either one-shot or periodic
 *	\param		pVoid	Garbage-in, garbage-out void pointer
 *
 *	\return 	Timer ID.
 ***************************************************************************/
AEF_TIMER_HANDLE CAefTimer::Start(UNSIGNED32 id, UNSIGNED32 ms, UNSIGNED8 type, void* pVoid)
{
    return Start(m_TimerTaskId, id, ms, type, pVoid);
}

/**************************************************************************/

/*!
 *	\brief Starts the application timer.
 *
 *	The StartTimer method creates and starts a VxWorks watchdog timer. 
 *	This VxTimer object will then send a task message when the timer fires.
 *
 *	\param	queue_id 	Application Queue Id.
 *	\param	ms		Number of milliseconds before the timer should expire.
 *	\param	ticks		Number of ticks before the timer should expire.
 *	\param	type		Timer type, either one-shot or periodic
 *	\param	pVoid		Garbage-in, garbage-out void pointer
 *
 *	\return Returns Timer ID, or NULL if unsuccessful.  
 ***************************************************************************/
AEF_TIMER_HANDLE CAefTimer::Start(AEF_TIMER_TASK_ID timerTaskId, UNSIGNED32 id, UNSIGNED32 ms, UNSIGNED8 type, void* pVoid)
{
    AEF_TIMER_HANDLE hTimer = AEF_INVALID_TIMER_HANDLE;
    // Allocate new Timer Info 
    AEF_TIMER_INFO *pTimerInfo = new AEF_TIMER_INFO;
    if (pTimerInfo != NULL)
    {
        hTimer = m_TimerInstance++;
        if (hTimer == AEF_INVALID_TIMER_HANDLE)
        {
            hTimer = m_TimerInstance++;
        }
        pTimerInfo->hTimer = hTimer;
        pTimerInfo->id = id;
        pTimerInfo->type = type;
        pTimerInfo->timerTaskId = timerTaskId;
        pTimerInfo->pVoid = pVoid;
        //struct itimerspec its;
        time_t nSec = (ms / 1000);
        long nNanoSec = ((ms % 1000) * 1000000);
        pTimerInfo->its.it_value.tv_sec = nSec;
        pTimerInfo->its.it_value.tv_nsec = nNanoSec;
        // use the timer itself to provide periodic behavior
        //  rather than re-firing in the handler
        //  Seems like a good choice, but is slightly different behavior.
        if (type == AEF_TIMER_TYPE_PERIODIC)
        {
            pTimerInfo->its.it_interval.tv_nsec = nNanoSec;
            pTimerInfo->its.it_interval.tv_sec = nSec;
        }
        else
        {
            pTimerInfo->its.it_interval.tv_sec = 0;
            pTimerInfo->its.it_interval.tv_nsec = 0;
        }
        // TODO: think ...
        // Soooo, I started with the SIGEV_SIGNAL method. Setting up a signal handler
        //  that would call Fire (I probably could have made 'Fire' be the handler
        //  directly - no matter). I wanted to avoid the signals/signal handler
        //  paradigm because I (possibly mistakenly) believe they can affect
        //  the entire process, are queued at a max of one (see timer_getoverrun). 
        //
        // Anyway,
        //  SIGEV_THREAD is another option. The man page says
        //  "Upon timer expiration, invoke sigev_notify_function as if it
        //      were the start function of a new thread". It is in fact a new thread.
        // This supposedly avoids the 'signal missing' discussed in timer_getoverrun
        //  although this mechanism is not without concerns.
        // Digest timer_getoverrun in this regard.
        //
        // However, both/either mechanism seems to operate 'flawlessly' for my
        //  test application(s).
        //
        // timerfd_create allegedly avoids this, treating timers as a 
        //  plain old file descriptor. We'd have to add a monitoring mechanism,
        //  i.e., poll, read. However, expirations can still be missed.
        // I could be vastly over thinking this.
        // For now I have settled on SIGEV_THREAD as it seems a more 'natural'
        //  interface.
        //
        struct sigevent sev;      
        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_value.sival_ptr = pTimerInfo;
        sev.sigev_notify_function = &CAefTimer::Fire;
        // these are the thread attributes for the SIGEV_THREAD
        // accept the defaults, i.e., NULL
        sev.sigev_notify_attributes = NULL; 
        // create the timer
        int nRc = timer_create(CLOCK_REALTIME, &sev, &pTimerInfo->timerId);
        if (nRc == 0)
        {
            // Start timer
            nRc = timer_settime(pTimerInfo->timerId, 0, &pTimerInfo->its, NULL);
            if (nRc != 0)
            {
                perror("CAefTimer::Start timer_settime failed: ");
                // Could not start the timer, so let's clean up.
                // Delete the timer.
                timer_delete(pTimerInfo->timerId);
                hTimer = AEF_INVALID_TIMER_HANDLE;
            }
            else
            {
                if (m_mutexTimerMap.Lock() == true)
                {
                    m_timerMap[hTimer] = pTimerInfo;
                    m_mutexTimerMap.Unlock();
                }
            }
        }
    }
    if (hTimer == AEF_INVALID_TIMER_HANDLE)
    {
        if (pTimerInfo == NULL)
        {
            delete pTimerInfo;
        }
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
 *	\param	hTimer 		Timer handle.
 *
 *	\return true if timer was found and stopped, false otherwise
 *
 ***************************************************************************/
bool CAefTimer::Stop(AEF_TIMER_HANDLE hTimer)
{
    bool bResult = false;
    struct itimerspec its;
    TimerMap::iterator it;
    AEF_TIMER_INFO* pTimerInfo;
    if (m_mutexTimerMap.Lock() == true)
    {
        if ((it = m_timerMap.find(hTimer)) != m_timerMap.end())
        {
            pTimerInfo = (*it).second;
            if (pTimerInfo != NULL)
            {
                its.it_value.tv_sec = 0;
                its.it_value.tv_nsec = 0;
                its.it_interval.tv_sec = 0;
                its.it_interval.tv_nsec = 0;
                int nRc = timer_settime(pTimerInfo->timerId, 0, &its, NULL);
                nRc = timer_delete(pTimerInfo->timerId);
                delete pTimerInfo;
            }
            m_timerMap.erase(hTimer);
            bResult = true;
        }
        m_mutexTimerMap.Unlock();
    }
    return bResult;
}


/**************************************************************************/

/*!
 *	\brief Stop all current timers and delete their timer information
 *
 ***************************************************************************/
void CAefTimer::DeleteAllTimers()
{
    struct itimerspec its;
    TimerMap::iterator it;
    AEF_TIMER_INFO *pTimerInfo;

    if (m_mutexTimerMap.Lock() == true)
    {
        for (it = m_timerMap.begin(); it != m_timerMap.end(); it++)
        {
            pTimerInfo = (*it).second;
            if (pTimerInfo != NULL)
            {
                its.it_value.tv_sec = 0;
                its.it_value.tv_nsec = 0;
                its.it_interval.tv_sec = 0;
                its.it_interval.tv_nsec = 0;
                int nRc = timer_settime(pTimerInfo->timerId, 0, &its, NULL);
                timer_delete(pTimerInfo->timerId);
                delete pTimerInfo;
            }
        }
        m_timerMap.clear();
        m_mutexTimerMap.Unlock();
    }
}

/**************************************************************************/

/*!
 *	\brief Handle a timer firing.  Called from the task context 
 *
 *	This method will restart the watchdog timer if it was configured to be
 *	periodic, or it will delete the timer and associated data if it was
 *	a one-shot timer.
 *
 *	\param	hTimer 	timer handle
 *
 *	\return void
 ***************************************************************************/
void CAefTimer::HandleTimer(AEF_TIMER_HANDLE hTimer)
{
    TimerMap::iterator it;
    AEF_TIMER_INFO* pTimerInfo;
    if (m_mutexTimerMap.Lock() == true)
    {
        if ((it = m_timerMap.find(hTimer)) != m_timerMap.end())
        {
            pTimerInfo = (*it).second;
            if (pTimerInfo != NULL)
            {
                if (pTimerInfo->type != AEF_TIMER_TYPE_PERIODIC)
                {         
                    m_timerMap.erase(hTimer);
                    timer_delete(pTimerInfo->timerId);
                    delete pTimerInfo;
                }
                /*
                 * Periodic'ness lives in the timer when it is started.
                 * So there is no reason to restart a periodic timer here.
                 * This could easily be a bad idea. The timer will fire at
                 *  the time (delay) specified rather than firing after we
                 *  start, do some stuff, restart. Looks like the only difference
                 *  is going through the map.
                 */
            }
        }
        m_mutexTimerMap.Unlock();
    }
}

/**************************************************************************/

/*!
 *	\brief Static method used as the VxWorks Watchdog Timer callback function.
 *
 *	The method send the AEF_TM_TIMER message to the queue associated with 
 *	the timer instance.
 *
 *	\param	AEF_TIMER_INFO*	pTimerInfo		Application Timer Id.
 *	\return int					Returns VxWorks timer.  
 ***************************************************************************/
void CAefTimer::Fire(union sigval sv)
{
    int result;
    AEF_TIMER_INFO* pTimerInfo = (AEF_TIMER_INFO*)sv.sival_ptr;
    CAefTaskMessage tm(AEF_TM_TIMER);
    tm.SetData((unsigned char*) &pTimerInfo->hTimer, sizeof (AEF_TIMER_HANDLE));
    if ((result = mq_send(pTimerInfo->timerTaskId,
                          (char *) &tm,
                          sizeof (CAefTaskMessage),
                          MSG_PRI_NORMAL)) == AEF_ERROR)
    {
        // The timer could not send the message, so let's clean up.
    }
}

/**************************************************************************/

/*!
 *	\brief Verifies that timer is still active, and converts the handle into the client data associated with it.
 *
 *	\param	hTimer	Timer handle
 *	\param	timerId	task-specific timer ID (filled in by this method)
 *	\param	pVoid	void pointer passed in when timer was started (filled in by this method)
 *
 *	\return true if timer is valid.  timerId and pVoid arguments are also returned.  If invalid, false is returned
 ***************************************************************************/
bool CAefTimer::GetTimerClientInfo(AEF_TIMER_HANDLE hTimer, UNSIGNED32& timerId, void*& pVoid)
{
    bool result = false;
    TimerMap::iterator it;
    AEF_TIMER_INFO* pTimerInfo;
    if (m_mutexTimerMap.Lock() == true)
    {
        if ((it = m_timerMap.find(hTimer)) != m_timerMap.end())
        {
            pTimerInfo = (*it).second;
            timerId = pTimerInfo->id;
            pVoid = pTimerInfo->pVoid;
            result = true;
        }
        m_mutexTimerMap.Unlock();
    }
    return result;
}
