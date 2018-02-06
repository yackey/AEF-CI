/*
 * MutexTests.cpp
 *
 *  Created on: Dec 17, 2017
 *      Author: yackey
 */

#include <stdio.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "aefTypes.h"
#include "aefTimer.h"

//#include "aefTask.h"
#include "aefTaskMessage.h"
#include "aefQueueTask.h"

#include "aefTask.h"
#include "aefTaskMessage.h"
#include "aefQueueTask.h"

#include "aefMutex.h"

TEST(MutexTest, LockMutex_WithNoWaitSpecified_ReturnLocked)
{
	CAefMutex* pAefMutex = new CAefMutex();
	bool bRet = pAefMutex->Lock();
	ASSERT_TRUE(bRet);
}

TEST(MutexTest, UnLockMutex_WithNoWaitSpecified_ReturnUnLocked)
{
	CAefMutex* pAefMutex = new CAefMutex();
	pAefMutex->Lock();
	bool bRet = pAefMutex->Unlock();
	ASSERT_TRUE(bRet);
}

TEST(MutexTest, LockMutex_WithSingleLock_ReturnLockCount)
{
	CAefMutex* pAefMutex = new CAefMutex();
	pAefMutex->Lock();
	int nCount = pAefMutex->GetLockCount();
	ASSERT_EQ(nCount, 1);
}

TEST(MutexTest, LockMutex_WithMultipleRecursiveLock_ReturnLockCount)
{
	CAefMutex* pAefMutex = new CAefMutex();
	pAefMutex->Lock();
	pAefMutex->Lock();
	int nCount = pAefMutex->GetLockCount();
	ASSERT_EQ(nCount, 2);
}



