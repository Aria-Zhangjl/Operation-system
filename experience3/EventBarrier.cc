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
	EBlock->Acquire();//��������
	waiting_threads++;//�ȴ��߳�����һ
	if (state)//��ʱ�Ѿ�signal
	{
		EBlock->Release();//�ͷ���
		return;
	}
	printf("***%s is waiting.\n", currentThread->getName());
	Event_signal->Wait(EBlock);//�ȴ���signal
	EBlock->Release();//�ͷ���
}

void EventBarrier::Signal()
{
	EBlock->Acquire();//��������
	state = true;
	Event_signal->Broadcast(EBlock);//֪ͨ�߳̿��Կ�ʼ����Ӧ��
	while(waiting_threads)
	{
		Thread_completed->Wait(EBlock);
	}
	state = false;//���е��̶߳�Ӧ��״̬��Ϊunsignaled
	Waiting_complete->Broadcast(EBlock);//���е��̶߳�Ӧ�𣬿���ͬ������
	EBlock->Release();//�ͷ���
}

void EventBarrier::Complete()
{
	EBlock->Acquire();
	waiting_threads--;//��ǰ�߳��Ѿ�Ӧ��,�ȴ��߳�����һ
	if (!waiting_threads)//�����Ӧ�������߳�ͬ�����й��¼����߳̿�����ɲ��޸�״̬
	{
		Thread_completed->Signal(EBlock);
	}
    Waiting_complete->Wait(EBlock);//����ȴ�������������
	EBlock->Release();
}

int EventBarrier::Waiters()
{
	return waiting_threads;
}