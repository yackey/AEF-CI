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

#include "aefTypes.h"
#include "aefSerialBase.h"
#include "aefQueueTask.h"

class CAefSerial : public CAefTask, public CAefSerialBase
{
    // Class Methods
public:
    CAefSerial();
    CAefSerial(const char *name, int priority, int options, int stack);
    virtual ~CAefSerial();

    virtual AEF_STATUS Configure(const char* pDevName, // device Init
                                 CAefQueueTask* pTask, UNSIGNED32 dataRcvdId, // msg  Init
                                 UNSIGNED32 shutdownMsgId, CPacket* pPktObj,
                                 UNSIGNED32 bufsize, int timeout = 0, int keepalive = 0);
    virtual AEF_STATUS Send(UNSIGNED8* pData, UNSIGNED32 dataLen);
    virtual inline bool IsConfigured(){return m_bConfigured;};
    virtual void SetKeepalive(int timeout, int keepalive);
    virtual AEF_STATUS ConfigurePort(UNSIGNED32 baudRate, UNSIGNED8 dataBits, UNSIGNED8 parity, UNSIGNED8 stopBits);

protected:
    virtual AEF_STATUS Run();
    virtual void Construct();    \
    virtual AEF_STATUS Init(void* pTaskData = NULL); \
    virtual AEF_STATUS Exit(AEF_STATUS exitCode); \
    virtual void ReceiveTimeoutCheck(void); \
    virtual void KeepAliveCheck(void);  \
    virtual UNSIGNED32 TicksFromMs(UNSIGNED32 ms);         \
    virtual UNSIGNED32 TicksFromTOD();

protected:
    CAefQueueTask* m_pParent; // parent class
    UNSIGNED32 m_ShutdownId; // Shutdown event message ID
    UNSIGNED32 m_DataRcvdId; // Data Received message ID
    CPacket* m_pPktObj;

    bool m_bConfigured; // If TRUE, object is configured
    UNSIGNED32 m_BufSize; // Read buffer size
    UNSIGNED8* m_pBuf;
    UNSIGNED32 m_ClientTimeout; // Milliseconds before a non-responding
    // client is terminated.
    UNSIGNED32 m_KeepAliveTimeout;

    UNSIGNED32 m_baudRate;
    UNSIGNED8 m_dataBits;
    UNSIGNED8 m_parity;
    UNSIGNED8 m_stopBits;

    // TODO: this needs to be an UNSIGNED32 in Linux'ville
    // so I am moving it to the platform specific class part
    // the part where it says VxWorks SIO is a hint its platform specific
    // I am not messing with the VxWorks item in source control
    //  because I cannot test it. So when the compile/link fails (whenever)
    //  then moved this to aefVxSerial.h
    //  UNSIGNED8		m_sioOpts;			// VxWorks SIO h/w option bits

    UNSIGNED32 m_sioOpts;
    UNSIGNED32 m_ntickspersec;
    UNSIGNED32 m_LastSend;
    UNSIGNED32 m_LastRcvd;
    SIGNED32 m_exitReason;
    SEM_ID m_semDev;
    const char* m_pDevName;
    int m_fdSerial;
    bool m_bPortUpdate;

};




// zzzzzzzzzzzzzzzz
#endif // _aefLinuxSerial_h_
