/*
 * TimerTests.cpp
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

TEST(TimerTest, TimerStart_WithOneShot_ReturnValidHandle)
{
	int nTimerId = 10;
	CAefTimer* pAefTimer = new CAefTimer();
	pAefTimer->Init(55);
	AEF_TIMER_HANDLE nTh = pAefTimer->Start(nTimerId, 3000, AEF_TIMER_TYPE_ONE_SHOT);
	ASSERT_NE(nTh, AEF_INVALID_TIMER_HANDLE);
}

TEST(TimerTest, TimerStart_WithOneShotAndStop_ReturnStopSuccess)
{
	int nTimerId = 10;
	CAefTimer* pAefTimer = new CAefTimer();
	pAefTimer->Init(55);
	AEF_TIMER_HANDLE nTh = pAefTimer->Start(nTimerId, 5000, AEF_TIMER_TYPE_ONE_SHOT);
	EXPECT_NE(nTh, AEF_INVALID_TIMER_HANDLE);
	//sleep(2);
	bool bRet = pAefTimer->Stop(nTh);
	ASSERT_TRUE(bRet);
}

TEST(TimerTest, TimerStart_WithOneShot_ReturnClientInfo)
{
	int nTimerId = 10;
	CAefTimer* pAefTimer = new CAefTimer();
	pAefTimer->Init(55);
	AEF_TIMER_HANDLE nTh = pAefTimer->Start(nTimerId, 5000, AEF_TIMER_TYPE_ONE_SHOT);
	EXPECT_NE(nTh, AEF_INVALID_TIMER_HANDLE);

	UNSIGNED32 outId = 0;
	void*	pVoid;
	bool bRet = pAefTimer->GetTimerClientInfo(nTh, outId, pVoid);
	ASSERT_TRUE(bRet);
}

TEST(TimerTest, TimerStart_WithOneShot_ReturnTimerId)
{
	UNSIGNED32 nTimerId = 10;
	CAefTimer* pAefTimer = new CAefTimer();
	pAefTimer->Init(55);
	AEF_TIMER_HANDLE nTh = pAefTimer->Start(nTimerId, 5000, AEF_TIMER_TYPE_ONE_SHOT);
	EXPECT_NE(nTh, AEF_INVALID_TIMER_HANDLE);

	UNSIGNED32 outId = 0;
	void*	pVoid;
	bool bRet = pAefTimer->GetTimerClientInfo(nTh, outId, pVoid);
	ASSERT_EQ(nTimerId, nTimerId);
}


