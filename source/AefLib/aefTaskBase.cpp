///***************************************************************************/
///*!
//**	\file  aefLinuxTask.cpp
//**	\brief Source for VxTask Class implementation
//**
//**	Avtec, Inc.
//**	100 Innovation Place
//**	Lexington, SC 29072 USA
//**	(803) 358-3600
//**
//**	Copyright (C) Avtec, Inc., 1990-present.
//**	All rights reserved. Copying, compilation, modification, distribution
//**	or any other use whatsoever of this material is strictly prohibited
//**	without the written consent of Avtec, Inc.
//*****************************************************************************/
#include "aefTypesBase.h"
#include "aefTaskBase.h"

/*************************************************************************/
/*!
*	\brief	Constructor for CAefTaskBase class.
*
*	Default Constructor that initializes default settings for tasks in VxWorks.
*
***************************************************************************/
CAefTaskBase::CAefTaskBase()
{

}

/**************************************************************************/
/*!
*	\brief	Deconstructor for CAefTaskBase class.
*
*	Cleans up CAefTaskBase object
*
***************************************************************************/
CAefTaskBase::~CAefTaskBase()
{
}

///**************************************************************************/
///*!
//*	\brief	Stores a name to associated with this class/task
//*
//*	\param	name	task name to be used.
//*
//***************************************************************************/
//void CAefTaskBase::SetTaskName(const char *name)
//{
//    if (name != NULL) 
//    {
//        // truncate the name if it's larger than the string variable
//        // we're using to store it
//        if (strlen(name) < TASK_NAME_LENGTH) 
//        {
//                strcpy(m_TaskName, name);
//        }
//        else 
//        {
//                strncpy(m_TaskName, name, TASK_NAME_LENGTH-1);
//        }
//    }
//}
//
///**************************************************************************/
///*!
//*	\brief	Attempt to become the task that "owns" the watchdog and is required to check it periodically
//*
//*	\return AEF_SUCCESS if task successfully registers as the owner, AEF_ERROR if another task already owns it
//***************************************************************************/
//AEF_STATUS CAefTaskBase::TakeWatchdogOwnership()
//{
//    AEF_STATUS	rc = AEF_ERROR;
//
//    // to take ownership, our task must be running (i.e., we must have
//    // a valid task ID), and no one else can have already taken ownership
//    // yet.
//    if (m_mutexTStatusMap.Lock() == true)
//    {
//        if ((IsRunning() == true) && (m_WatchdogTaskId == AEF_INVALID_TASK_ID)) {
//                m_WatchdogTaskId = m_TaskID;
//                rc = AEF_SUCCESS;
//        }
//        m_mutexTStatusMap.Unlock();
//    }
//
//    return rc;
//}
//
///**************************************************************************/
///*!
//*	\brief	Relenquish ownership of the watchdog.
//*
//*   Note: This call should be made after the task is running (e.g., in the Init() routine, perhaps)!!!
//*
//*	\return AEF_SUCCESS if task successfully relenquishes ownership, AEF_ERROR if it fails
//***************************************************************************/
//AEF_STATUS CAefTaskBase::ReleaseWatchdogOwnership()
//{
//    AEF_STATUS	rc = AEF_ERROR;
//
//    // if task is shutting down, this method must be called
//    // before the task ID is erased!
//    if (m_mutexTStatusMap.Lock() == true)
//    {
//        if (m_WatchdogTaskId == m_TaskID) 
//        {
//                m_WatchdogTaskId = AEF_INVALID_TASK_ID;
//                rc = AEF_SUCCESS;
//        }
//        m_mutexTStatusMap.Unlock();
//    }
//
//    return rc;
//}
//
///**************************************************************************/
///*!
//*	\brief	Register this task's intention to report it's "alive" status periodically
//*
//*	\return AEF_SUCCESS if task is successfully registered, AEF_ERROR otherwise
//***************************************************************************/
//AEF_STATUS CAefTaskBase::RegisterWatchdogKick()
//{
//    AEF_STATUS	rc = AEF_ERROR;
//    if (m_bKickTheDog == false) 
//    {
//        m_bKickTheDog = true;
//        if (IsRunning() == true) 
//        {
//            // go ahead and kick it for the first time
//            WatchdogReport();
//        }
//        rc = AEF_SUCCESS;
//    }
//    // if task is already registered, then this registration attempt fails
//    return rc;
//}
//
///**************************************************************************/
///*!
//*	\brief	Remove this task from the list of tasks reporting periodic status
//*
//*	\return AEF_SUCCESS if task is removed from the list, AEF_ERROR otherwise
//***************************************************************************/
//AEF_STATUS CAefTaskBase::UnregisterWatchdogKick()
//{
//    AEF_STATUS	rc = AEF_ERROR;
//
//    if (m_bKickTheDog == true) 
//    {
//        if (m_mutexTStatusMap.Lock() == true)
//        {
//                m_TStatusMap.erase(m_TaskID);
//                m_mutexTStatusMap.Unlock();
//                m_bKickTheDog = false;
//                rc = AEF_SUCCESS;
//        }
//    }
//    // if task isn't currently registered, then this unregistration attempt fails
//
//    return rc;
//}
//
//
///**************************************************************************/
///*!
//*	\brief	Method to initialize the task just after it has started
//*
//*	\return AEF_SUCCESS if task was Initialized successfully, AEF_ERROR otherwise
//*
//***************************************************************************/
//AEF_STATUS CAefTaskBase::Init(void* pTaskData)
//{
//    std::string	sTaskName = m_TaskName;
//
//    // add this task to the list of running tasks
//    if (m_mutexTQMap.Lock() == true) 
//    {
//        // this base class doesn't have a message queue, but maybe a derived class does?
//        m_TQMap[sTaskName] = (MSG_Q_ID)pTaskData;
//        m_mutexTQMap.Unlock();
//    }
//
//    return (AEF_SUCCESS);
//}
//
///**************************************************************************/
///*!
//*	\brief	Method to clean up the task just before it exists
//*
//*	\return AEF_SUCCESS, unless task class is actually deleted as part of this call
//*
//***************************************************************************/
//AEF_STATUS CAefTaskBase::Exit(AEF_STATUS exitCode) 
//{
//    std::string sTaskName = m_TaskName;
//
//    // remove this task to the list of running tasks
//    if (m_mutexTQMap.Lock() == true) 
//    {
//        m_TQMap.erase(sTaskName);
//        m_mutexTQMap.Unlock();
//    }
//
//    // call this before we invalidate the task ID, just in case
//    // the derived class hasn't called it yet...
//    ReleaseWatchdogOwnership();
//
//    m_TaskID = AEF_INVALID_TASK_ID;
//
//    return (AEF_SUCCESS);
//}
//
///**************************************************************************/
//
///*!
// *	\brief Base-class WatchdogReport method.
// *
// ***************************************************************************/
//void CAefTaskBase::WatchdogReport()
//{
//    if (m_bKickTheDog == true)
//    {
//        if (m_mutexTStatusMap.Lock() == true)
//        {
//            m_TStatusMap[m_TaskID] |= AEF_TASK_REPORTING;
//            m_mutexTStatusMap.Unlock();
//        }
//    }
//}
//
///**************************************************************************/
///*!
//*	\brief Base-class WatchdogCheck method.
//*
//***************************************************************************/
//bool CAefTaskBase::WatchdogCheck(UNSIGNED32 *task_id)
//{
//    bool bCheck = true;
//    TASK_STATUS_MAP::iterator it;
//    int reporting;
//    if (m_mutexTStatusMap.Lock() == true)
//    {
//        if (m_WatchdogTaskId == m_TaskID)
//        {
//            // Update ourselves if we're supposed to be kicking too
//            if (m_bKickTheDog == true)
//            {
//                m_TStatusMap[m_TaskID] |= AEF_TASK_REPORTING;
//            }
//            // printf("%s: WatchdogCheck\n", m_TaskName);
//
//            for (it = m_TStatusMap.begin(); it != m_TStatusMap.end(); ++it)
//            {
//                // printf("Checking Task: %d\n", it->first);
//                reporting = it->second;
//                // Check if the reporting bit is off, if so then the bit is
//                // off since the last time we checked the status.
//                if (!(reporting & AEF_TASK_REPORTING))
//                {
//                    //We have one task not reporting, we will need to restart
//                    //printf("Stop Kicking Hardware Watchdog (%s died)\n", taskName(it->first));
//                    bCheck = false;
//                    if (task_id != NULL)
//                    {
//                        *task_id = it->first;
//                    }
//                }
//                else
//                {
//                    // The bit is still set, so let's clear it and check
//                    // next time.
//                    (it->second) &= ~(AEF_TASK_REPORTING);
//                }
//            }
//        }
//        m_mutexTStatusMap.Unlock();
//    }
//    return bCheck;
//}
//
///**************************************************************************/
///*!
//*   \brief Method to delay the task while allowing other tasks to take the CPU
//*
//*   \param  ms  The number of milliseconds to delay/sleep for
//*
//*   \return void
//*
//***************************************************************************/
//void CAefTaskBase::TaskDelay(UNSIGNED16 ms)
//{
//    useconds_t nUsecDelay = (ms * 1000);
//    // TODO: is this even almost correct
//    // could calculate ticks per second; but for now assume 20 ms is close to one tick
//    for ( int nMsPerTick = 20 ; nUsecDelay > 0 ; )
//    {
//        pthread_testcancel();
//        usleep(nMsPerTick);
//        nUsecDelay -= nMsPerTick;
//    }
//}
