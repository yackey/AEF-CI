/***************************************************************************/
/*!
**	\file	aefLinuxMutex.cpp 
**	\brief	VxWorks-specific implementation of the AEF Mutex Class
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
#include "aefMutex.h"
#include "aefException.h"
#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>
#include <sstream>

CAefMutex::CAefMutex()
{
    // we want a mutual-exclusion semaphore, i.e., recursive
    // TODO: is there a concept of failure necessary ?
    std::stringstream ss;   
    int rc = pthread_mutexattr_init(&m_mtAttr);
    if ( rc)
    {
        throw new CAefException("pthread_mutexattr_init: ", rc);
    }   
    
    rc = pthread_mutexattr_settype(&m_mtAttr, PTHREAD_MUTEX_RECURSIVE);
    if ( rc)
    {
        throw new CAefException("pthread_mutexattr_settype: ", rc);
    }    
    rc = pthread_mutex_init(&m_semMutex, &m_mtAttr);
    if ( rc)
    {
        throw new CAefException("pthread_mutex_init: ", rc);
    }
    m_lockCount = 0;
}


CAefMutex::~CAefMutex()
{
    int rc = pthread_mutexattr_destroy(&m_mtAttr);
    rc = pthread_mutex_destroy(&m_semMutex);
    if ( rc)
    {
        throw new CAefException("pthread_mutex_destroy: ", rc);
    }
}


bool CAefMutex::Lock(UNSIGNED32 msWait)
{
    int nRet;
    bool bResult;
    if (msWait != AEF_WAIT_FOREVER) 
    {
        m_semWait.tv_nsec = msWait/1000;
        m_semWait.tv_sec = (msWait % 1000) * 1000000;
        nRet = pthread_mutex_timedlock(&m_semMutex, &m_semWait);
    }
    else 
    {
        // TODO: if its wait forever can the tv stuff be zero
        //  does it matter or is it just clever
        nRet = pthread_mutex_lock(&m_semMutex);
    }

    if (nRet == 0) 
    {
        bResult = true;
        m_lockCount++;
    }
    else 
    {
        bResult = false;
    }

    return bResult;
}


bool CAefMutex::Unlock()
{
    bool bResult;
    int nRet = pthread_mutex_unlock(&m_semMutex);
    if (nRet == 0) 
    {
        bResult = true;
        m_lockCount--;
    }
    else {
        bResult = false;
    }
    return bResult;
}
