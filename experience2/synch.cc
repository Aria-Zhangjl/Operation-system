// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//-------------------------------------- --------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) 
{
    name = debugName;
    //��ǰ����û�б�ռ��
    state = true;
    prethread = NULL;
    queue = new List;
}
Lock::~Lock() 
{
    delete queue;
}
void Lock::Acquire() 
{
    //�ж����Ƿ��б���ǰ���߳�ռ�죬���û�У�����ִ���������
    ASSERT(!isHeldByCurrentThread());
    //�ر��ж�
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    //�������æ���Ͱ��߳̽�����У�ͬʱ��˯
    while (!state)
    {
       // printf(" \t%s can't get the lock %s,threads go to sleep!\n",currentThread->getName(),name);
        queue->Append((void*)currentThread);
        currentThread->Sleep();
    }
    //printf("\t%s get the lock %s!\n",currentThread->getName(),name);
    //����ǰ��ǰ�̻߳����������ռ��
    state = false;
    prethread = currentThread;
    (void)interrupt->SetLevel(oldLevel);
}
//�ͷ���
void Lock::Release() 
{
    //�����ǰ�����б�ռ�죬�����ִ��������ͷŲ���
    ASSERT(isHeldByCurrentThread());
    Thread *thread;
    //���ж�
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    thread = (Thread *)queue->Remove();
    if (thread != NULL)	 
        scheduler->ReadyToRun(thread);
    state = true;
    prethread = NULL;
    //���ж�
    (void) interrupt->SetLevel(oldLevel);
    //printf("\t%s release the lock!\n", currentThread->getName());
}
bool Lock::isHeldByCurrentThread()
{
    if (prethread == currentThread)
        return true;
    return false;
}
Condition::Condition(char* debugName) 
{
    name = debugName;
    queue = new List;
}
Condition::~Condition() 
{
    delete queue;
}
void Condition::Wait(Lock* conditionLock) 
{ 
    //ȷ����ǰ���߳�����
    ASSERT(conditionLock->isHeldByCurrentThread()); 
    //���ж�
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    //�ͷ���
    conditionLock->Release();
    //����ǰ�̷߳������
    queue->Append((void*)currentThread);
    //�߳̽���˯��
    currentThread->Sleep();
    //���ж�
    (void)interrupt->SetLevel(oldLevel);
    //�ȴ��ٴ�ӵ����
    conditionLock->Acquire();
}
void Condition::Signal(Lock* conditionLock)
{
    //ȷ����ǰ���߳�����
    ASSERT(conditionLock->isHeldByCurrentThread());
    //���ж�
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    //���Ѷ����е�ͷ���߳�
    Thread* newthread = (Thread*)queue->Remove();
    //�����������е�һ���߳̽������
    if (newthread != NULL)
        scheduler->ReadyToRun(newthread);
    //���ж�
    (void)interrupt->SetLevel(oldLevel);
}
void Condition::Broadcast(Lock* conditionLock) 
{ 
    //ȷ����ǰ���߳�����
    ASSERT(conditionLock->isHeldByCurrentThread());
    //���ж�
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    //�����������������̻߳���
    Thread* newthread = (Thread*)queue->Remove();
    while (newthread != NULL)
    {
        scheduler->ReadyToRun(newthread);
       newthread = (Thread*)queue->Remove();
    }
    //���ж�
    (void)interrupt->SetLevel(oldLevel);
}
