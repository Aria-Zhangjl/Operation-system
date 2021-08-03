#pragma once
#ifndef EVENTBARRIER_H_INCLUDED
#define EVENTBARRIER_H_INCLUDED

#include "system.h"
#include "copyright.h"
#include "synch.h"
class EventBarrier
{
public:
	EventBarrier();   //构造函数
	~EventBarrier();  //析构函数

	void Wait();      //让线程等待
	void Signal();    //唤醒等待的进程后，继续阻塞直到所有的线程都对事件产生应答
	void Complete();  //所有的线程都对发生的事件有应答
	int Waiters();    //返回正在等待的线程数

private:
	bool state;//事件是否发生，true已发生，false未发生
	int waiting_threads;//等待事件发生的线程数
	Lock* EBlock;//配合条件变量使用
	Condition* Event_signal;//事件是否发生
	Condition* Waiting_complete;//线程等待同步
	Condition* Thread_completed;//所有线程都已经应答
};
#endif // EVENTBARRIER_H_INCLUDED