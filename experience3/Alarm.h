#pragma once
#include"thread.h"
#include"copyright.h"
#include"list.h"
#include"system.h"

class Alarm
{
public:
	Alarm();
	~Alarm();

	void Pause(int howLong);
	void Awake();
private:
	List* thread_list;//��˯���߳�
	int sleep_thread;//��˯�߳���
};