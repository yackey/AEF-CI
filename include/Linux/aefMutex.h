/***************************************************************************/
/*!
 **	\file	aefLinuxMutex.h 
 **	\brief	VxWorks-specific members of the AEF Mutex Object Definition
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
#ifndef _aefLinuxMutex_h_
#define _aefLinuxMutex_h_

#include <aefLinuxTypes.h>
#include <aefMutexInterface.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

class CAefMutex : public aefMutexInterface
{
public:
    CAefMutex();
    virtual ~CAefMutex();
    virtual bool Lock(uint32_t msWait = AEF_WAIT_FOREVER);
    virtual bool Unlock();

    virtual int32_t GetLockCount()
    {
        return m_lockCount;
    };

protected:
    int32_t m_lockCount;
    pthread_mutex_t m_semMutex;
    pthread_mutexattr_t m_mtAttr;
    struct timespec m_semWait;

};

#endif // _aefLinuxMutex_h_
