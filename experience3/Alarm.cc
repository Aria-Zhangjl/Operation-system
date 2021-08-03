
#include"Alarm.h"
using namespace std;

static void Check(int i)
{
	Alarm* p = (Alarm*)i;
	p->Awake();
}
Alarm::Alarm()
{
	sleep_thread = 0;
	thread_list = new List;
}

Alarm::~Alarm()
{
	while (!thread_list->IsEmpty())
		delete thread_list->Remove();
	delete thread_list;
}

void Find(int i)
{
	int* a;
	a = (int*)i;
	currentThread->Yield();
	while (*a != 0)
		currentThread->Yield();//生成的新线程一直执行切换
}
void Alarm::Pause(int howLong)
{
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
	sleep_thread++;
	//沉睡线程为空
	if (sleep_thread == 1)
	{
		Thread* t = new Thread((char*)"new thread");//生成一个新线程执行切换，在不断的切换过程中去唤醒睡眠时间已到的线程，
											//当无睡眠线程时，则结束。
		t->Fork(Find, (int)&sleep_thread);
	}
	int wakeup = stats->totalTicks + howLong;
	thread_list->SortedInsert((void*)currentThread, wakeup);
	interrupt->Schedule(Check, (int)this, wakeup, TimerInt);// Schedule an interrupt to occur at time ``when''.  This is called by the hardware device simulators.
	currentThread->Sleep();//当前线程进入睡眠
	(void)interrupt->SetLevel(oldLevel);
}

void Alarm::Awake()
{
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
	int wakeup_time;
	Thread* t = (Thread*)thread_list->SortedRemove(&wakeup_time);
	if (t != NULL)
	{
		sleep_thread--;
		scheduler->ReadyToRun(t);
	}
	(void)interrupt->SetLevel(oldLevel);
}