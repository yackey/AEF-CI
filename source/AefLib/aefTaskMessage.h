/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   aefLinuxTaskMessage.h
 * Author: yackey
 *
 * Created on July 20, 2017, 4:12 PM
 */

#ifndef AEFLINUXTASKMESSAGE_H
#define AEFLINUXTASKMESSAGE_H

#include <aefTypes.h>
#include "aefTaskMessageBase.h"

#include <string.h>
#define MSG_PRI_NORMAL  10 // 0 - 31 required to be supported

class CAefTaskMessage : public CAefTaskMessageBase
{
public:
    CAefTaskMessage(int id = 0);
    virtual ~CAefTaskMessage();
    virtual void SetId(int id);
    virtual int GetId();
    virtual unsigned char* GetData(unsigned int* pDataLen = NULL);
    virtual AEF_STATUS GetData(unsigned char* pBuf, unsigned int bufLen, unsigned int* pDataLen = NULL);
    virtual AEF_STATUS SetDataPtr(unsigned char *pData, unsigned int dataLen, bool bFreeDataPtr = false);
    virtual AEF_STATUS SetData(unsigned char *pMsgBuf, unsigned int bufLen);
    virtual void Cleanup();
private:
		TASK_MESSAGE_STRUCT	m_message;
};

#endif /* AEFLINUXTASKMESSAGE_H */

