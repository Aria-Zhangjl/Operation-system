#pragma once
#ifndef EVENTBARRIER_H_INCLUDED
#define EVENTBARRIER_H_INCLUDED

#include "system.h"
#include "copyright.h"
#include "synch.h"
class EventBarrier
{
public:
	EventBarrier();   //���캯��
	~EventBarrier();  //��������

	void Wait();      //���̵߳ȴ�
	void Signal();    //���ѵȴ��Ľ��̺󣬼�������ֱ�����е��̶߳����¼�����Ӧ��
	void Complete();  //���е��̶߳��Է������¼���Ӧ��
	int Waiters();    //�������ڵȴ����߳���

private:
	bool state;//�¼��Ƿ�����true�ѷ�����falseδ����
	int waiting_threads;//�ȴ��¼��������߳���
	Lock* EBlock;//�����������ʹ��
	Condition* Event_signal;//�¼��Ƿ���
	Condition* Waiting_complete;//�̵߳ȴ�ͬ��
	Condition* Thread_completed;//�����̶߳��Ѿ�Ӧ��
};
#endif // EVENTBARRIER_H_INCLUDED