/*****************************************************************************
 *
 *	aefLinuxSerial.h
 *
 *   Copyright (C) Avtec, Inc., 1990-2007.
 *
 *   Avtec, Inc.
 *   4335 Augusta Hwy.
 *   Gilbert, SC 29054 USA
 *   (803) 892-2181
 *
 *   All rights reserved. Copying, compilation, modification, distribution
 *   or any other use whatsoever of this material is strictly prohibited
 *   without the written consent of Avtec, Inc.
 *
 *****************************************************************************/
#ifndef _aefLinuxSerial_h_
#define _aefLinuxSerial_h_

#include "aefLinuxTypes.h"
#include "aefSerialInterface.h"
#include "aefQueueTask.h"

class CAefSerial : public CAefTask, public aefSerialInterface
{
    // Class Methods
public:
    CAefSerial();
    CAefSerial(const char *name, int priority, int options, int stack);
    virtual ~CAefSerial();

    virtual AEF_STATUS Configure(const char* pDevName, // device Init
                                 CAefQueueTask* pTask, uint32_t dataRcvdId, // msg  Init
                                 uint32_t shutdownMsgId, aefPacketInterface* pPktObj,
                                 uint32_t bufsize, int timeout = 0, int keepalive = 0);
    virtual AEF_STATUS Send(uint8_t* pData, uint32_t dataLen);
    virtual inline bool IsConfigured(){return m_bConfigured;};
    virtual void SetKeepalive(int timeout, int keepalive);
    virtual AEF_STATUS ConfigurePort(uint32_t baudRate, uint8_t dataBits, uint8_t parity, uint8_t stopBits);

protected:
    virtual AEF_STATUS Run();
    virtual void Construct(); 
    virtual AEF_STATUS Init(void* pTaskData = NULL);
    virtual AEF_STATUS Exit(AEF_STATUS exitCode);
    virtual void ReceiveTimeoutCheck(void);
    virtual void KeepAliveCheck(void); 
    virtual uint32_t TicksFromMs(uint32_t ms); 
    virtual uint32_t TicksFromTOD();

protected:
    CAefQueueTask* m_pParent; // parent class
    uint32_t m_ShutdownId; // Shutdown event message ID
    uint32_t m_DataRcvdId; // Data Received message ID
    aefPacketInterface* m_pPktObj;

    bool m_bConfigured; // If TRUE, object is configured
    uint32_t m_BufSize; // Read buffer size
    uint8_t* m_pBuf;
    uint32_t m_ClientTimeout; // Milliseconds before a non-responding
    // client is terminated.
    uint32_t m_KeepAliveTimeout;

    uint32_t m_baudRate;
    uint8_t m_dataBits;
    uint8_t m_parity;
    uint8_t m_stopBits;

    // TODO: this needs to be an uint32_t in Linux'ville
    // so I am moving it to the platform specific class part
    // the part where it says VxWorks SIO is a hint its platform specific
    // I am not messing with the VxWorks item in source control
    //  because I cannot test it. So when the compile/link fails (whenever)
    //  then moved this to aefVxSerial.h
    //  uint8_t		m_sioOpts;			// VxWorks SIO h/w option bits

    uint32_t m_sioOpts;
    uint32_t m_ntickspersec;
    uint32_t m_LastSend;
    uint32_t m_LastRcvd;
    int32_t m_exitReason;
    SEM_ID m_semDev;
    const char* m_pDevName;
    int m_fdSerial;
    bool m_bPortUpdate;

};




// zzzzzzzzzzzzzzzz
#endif // _aefLinuxSerial_h_
