#include "EventBarrier.h"
EventBarrier::EventBarrier()
{
	waiting_threads = 0;
	state = false;
	EBlock = new Lock((char*)"EventBarrier_lock");
	Event_signal = new Condition((char*)"Event_signaled");
	Waiting_complete = new Condition((char*)"Thread_completed");
	Thread_completed = new Condition((char*)"Thread_all_completed");
}
EventBarrier::~EventBarrier()
{
	waiting_threads = 0;
	state = false;
	delete EBlock;
	delete Event_signal;
	delete Waiting_complete;
	delete Thread_completed;
}

void EventBarrier::Wait()
{
	EBlock->Acquire();//先申请锁
	waiting_threads++;//等待线程数加一
	if (state)//此时已经signal
	{
		EBlock->Release();//释放锁
		return;
	}
	printf("***%s is waiting.\n", currentThread->getName());
	Event_signal->Wait(EBlock);//等待被signal
	EBlock->Release();//释放锁
}

void EventBarrier::Signal()
{
	EBlock->Acquire();//先申请锁
	state = true;
	Event_signal->Broadcast(EBlock);//通知线程可以开始进行应答
	while(waiting_threads)
	{
		Thread_completed->Wait(EBlock);
	}
	state = false;//所有的线程都应答，状态变为unsignaled
	Waiting_complete->Broadcast(EBlock);//所有的线程都应答，可以同步向下
	EBlock->Release();//释放锁
}

void EventBarrier::Complete()
{
	EBlock->Acquire();
	waiting_threads--;//当前线程已经应答,等待线程数少一
	if (!waiting_threads)//如果都应答，所有线程同步，有关事件的线程可以完成并修改状态
	{
		Thread_completed->Signal(EBlock);
	}
    Waiting_complete->Wait(EBlock);//放入等待的条件变量中
	EBlock->Release();
}

int EventBarrier::Waiters()
{
	return waiting_threads;
}