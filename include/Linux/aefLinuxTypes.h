/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   aefLinuxTypes.h
 * Author: yackey
 *
 * Created on August 27, 2017, 8:45 PM
 */

#ifndef AEFTYPES_H
#define AEFTYPES_H

#include <limits.h>
#include <semaphore.h>
#include <mqueue.h>
#include <signal.h>
#include <time.h>
#include <map>
#include <string>

// general
#define CONST_TEXT __based(__segname("CONST_TEXT"))
#define MENU_TEXT __based(__segname("MENU_TEXT"))

/*Macros */
#define DISABLE_INTERUPT(); _disable();
#define ENABLE_INTERUPT();  _enable();
#define IN8(r)    _inp(r)
#define IN16(r)    _inpw(r)
#define OUT8(r,d)   _outp(r,d)
#define OUT16(r,d)   _outpw(r,d)

#define PACKED1
#define PACKED2
#define PACKED4

typedef uint32_t* CALLBACK_DATA_PTR;

#define AEF_ERROR  -1
#define AEF_SUCCESS  0

#define AEF_WAIT_FOREVER ((uint32_t)ULONG_MAX)
#define AEF_DONT_WAIT  ((uint32_t)0)

typedef int32_t AEF_STATUS;

#define AEF_TIMER_TYPE_ONE_SHOT     1
#define AEF_TIMER_TYPE_PERIODIC     2

// timer
#define AEF_TIMER_HANDLE  uint32_t
#define AEF_INVALID_TIMER_HANDLE 0

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

// For VxWorks, the task that will receive the timer notification is
// represented by the Message Queue ID associated with that task (so
// if the task is an AEF task, it must be derived from CAefQueueTask).
#define AEF_TIMER_TASK_ID uint32_t

typedef struct
{
    AEF_TIMER_HANDLE hTimer; // internally generated
    timer_t timerId; // result of create
    uint32_t id;
    uint8_t type;
    AEF_TIMER_TASK_ID timerTaskId;
    struct itimerspec its;
    void* pVoid;
} AEF_TIMER_INFO;

typedef std::map<AEF_TIMER_HANDLE, AEF_TIMER_INFO*> TimerMap;

//  queue
#define AEF_MSG_TASK_ID mqd_t
#define MSG_Q_ID uint64_t
#define MSG_Q_ID_INVALID -1

//  task

#define TASK_NAME_LENGTH 64
// Map of all task and status bits
typedef std::map<int, int> TASK_STATUS_MAP;
#define AEF_TASK_REPORTING 0x01
typedef sem_t SEM_ID;
#define INVALID_THREAD_HANDLE -1
// defines the os-specific data type of the "value" for the Task Queue Map.
typedef std::map<std::string, MSG_Q_ID> TASK_QUEUE_MAP;
// defines what type of data that's returned as a task identifier
typedef pthread_t AEF_TASK_ID;
// ERROR is a known invalid task ID in VxWorks...
#define AEF_INVALID_TASK_ID AEF_ERROR

// queue task
//typedef void (CAefQueueTask::*AefCallback)(void *msg);
//typedef std::map<int, AefCallback> TASK_MESSAGE_MAP;

//  tcp server
typedef int AEF_TCP_CONN_ID;
#define INVALID_CONN_ID AEF_ERROR

typedef struct
{
    AEF_TCP_CONN_ID Socket; // Handle to the socket
    uint32_t LastRcv; // Last valid message received on socket
    uint32_t IPAddress;
    uint16_t Port;
    uint8_t bDelete; // indication that connection should be deleted
} VX_TCP_CLIENT_INFO;

typedef std::map<AEF_TCP_CONN_ID, VX_TCP_CLIENT_INFO *> CLIENTMAP;

#endif /* AEFTYPES_H */

