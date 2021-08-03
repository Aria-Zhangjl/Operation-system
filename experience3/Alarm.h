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
	List* thread_list;//沉睡的线程
	int sleep_thread;//沉睡线程数
};