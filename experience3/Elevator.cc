#include"Elevator.h"

extern int ele_load;//电梯的载客量
extern int people;//要乘坐电梯的人
extern int floornum;//建筑高度
extern Building* building;//建筑

Lock* elevatorlock = new Lock((char*)"Elevator");
Lock* elevatorlock1 = new Lock((char*)"Elevator1");
Lock* listlock=new Lock((char*)"List");
Elevator::Elevator(char* debugName, int numFloors, int myID)
{
	name = new char[30];
	strcpy(name, debugName);
	elevator_highest = numFloors;
	ele_id = myID;
	currentfloor = 1;//先停留1楼
	occupancy = 0;
	load = ele_load;//电梯的载客量
	dir = stop;
	person_exit = new EventBarrier * [numFloors + 1];
	//每一个楼层创建一个事件栅栏
	//初始时所有的楼层都没有停靠的需要
	is_exit = new bool[numFloors + 1];
	for (int i = 1; i <= numFloors; i++)
	{
		person_exit[i] = new EventBarrier();
		is_exit[i] = false;
	}

}

Elevator::~Elevator()
{
	delete[]is_exit;
	for (int i = 0; i <= elevator_highest; i++)
	{
		delete person_exit[i];
	}
	delete person_exit;
}

//电梯的开门、关门、停靠操作

//电梯开门，如果到顶或到底
void Elevator::OpenDoors()//   signal exiters and enterers to action
{
	printf("Elevator %d open the door at %d.\n", ele_id, currentfloor);
	//如果这层楼有需要停靠
	if (is_exit[currentfloor])
	{
		//让所有的人都出电梯
		person_exit[currentfloor]->Signal();
		is_exit[currentfloor] = false;
	}
	//如果电梯方向向上，且该层楼有人要上升
	if (building->callup[currentfloor] && (dir == up || dir == stop))
	{
		//记录下当前电梯的ID
		building->elevatorup[currentfloor] = ele_id;
		//处理当前相同楼层相同方向的人数
		building->next_request_batch(currentfloor);
		//通知当前要上去的人可以进入
		building->uppeople[currentfloor]->Signal();
		//判断是否会超载
		//如果超载了
		//printf("now is %d.\n", occupancy);
		if (overload)
		{
			//printf("elevator overload!\n");
			building->callup[currentfloor] = true;
			overload = false;
		}
		else
		{
			building->callup[currentfloor] = false;
		}

	}
	if (building->calldown[currentfloor] && (dir == down || dir == stop))
	{
		building->elevatordown[currentfloor] = ele_id;
		//处理请求
		building->next_request_batch(-currentfloor);
		building->downpeople[currentfloor]->Signal();
		//printf("now is %d.\n", occupancy);
		//判断是否会超载
		if (overload)
		{
			//printf("elevator overload!\n");
			building->calldown[currentfloor] = true;
			overload = false;
		}
		else
		{
			building->calldown[currentfloor] = false;
		}
	}

}

//电梯关门，如果到顶或到底
void Elevator::CloseDoors()               //   after exiters are out and enterers are in
{
	printf("Elevator %d close the door at %d.\n", ele_id, currentfloor);
	/*
	if (dir == down && currentfloor == 1)
		dir = up;
	if (dir == up && currentfloor == elevator_highest)
		dir = down;
	*/
}

//电梯停靠
void Elevator::VisitFloor(int floor)      //   go to a particular floor
{
	currentfloor = floor;
	printf("Elevator %d visits at %d.\n", ele_id, currentfloor);
	//开门
	OpenDoors();
	//等所有上下门的乘客都做完
	currentThread->Yield();
	//关门
	CloseDoors();
}

//乘客
//乘客进入电梯
bool Elevator::Enter()
{
	//可以继续进电梯
	//printf("now elevator has %d.\n", occupancy);
	if (occupancy < load)
	{
		occupancy++;
		overload = false;
		if (dir == up)
			building->uppeople[currentfloor]->Complete();
		else
		{
			building->downpeople[currentfloor]->Complete();
		}
		return true;
	}
	else
	{
		overload = true;
		printf("Elevator overload! %s exits and waits.\n", currentThread->getName());
		if (dir == up)
		{
			listlock->Acquire();
			void* item;
			building->peoplerequestup->SortedInsert(item, currentfloor);
			listlock->Release();
			building->uppeople[currentfloor]->Complete();
		}
		else
		{
			listlock->Acquire();
			void* item;
			building->peoplerequestdown->SortedInsert(item, -currentfloor);
			listlock->Release();
			building->downpeople[currentfloor]->Complete();
		}
		return false;
	}
}

//乘客退出电梯
void Elevator::Exit()
{
	occupancy--;
	person_exit[currentfloor]->Complete();
}

//乘客给出目的地
void Elevator::RequestFloor(int floor)
{
	is_exit[floor] = true;
	person_exit[floor]->Wait();
}

//电梯持续运行
void Elevator::keepworking()
{
	while (true)
	{
		elevatorlock1->Acquire();
		//当前电梯没有被使用
		if (!occupancy)
		{
			int next = building->next_request(dir);
			//没有工作了
			if (next == 0)
			{
				dir = stop;
				printf("%s gose to sleep.\n",currentThread->getName());
				currentfloor = 1;
				building->sleepelevator->Wait(elevatorlock1);
				printf("%s wakes up again.\n", currentThread->getName());
				elevatorlock1->Release();
			}
			else
			{
				elevatorlock1->Release();
				elevatorlock->Acquire();
				if (next < 0)
				{
					dir = down;
					next = -next;
				}
				else
				{
					dir = up;
				}
				//printf("nextfloor=%d.\n", next);
				VisitFloor(next);
				elevatorlock->Release();
				currentThread->Yield();
			}
		}
		else
		{
			elevatorlock1->Release();
			elevatorlock->Acquire();
			if (dir == up)
			{
				for (int i = currentfloor + 1; i <= elevator_highest; i++)
				{
					if (is_exit[i] || building->callup[i])
					{
						VisitFloor(i);
						break;
					}
				}
			}
			else if (dir == down)
			{
				for (int i = currentfloor - 1; i >= 1; i--)
				{
					if (is_exit[i] || building->calldown[i])
					{
						VisitFloor(i);
						break;
					}
				}
			}
			elevatorlock->Release();
			currentThread->Yield();
		}
	}
}

//创建一个电梯
void Createlevator(int which)
{
	char name[30] = "elevator ";
	name[9] = which + '0';
	name[10] = '\0';
	building->elevator[which] = new Elevator((char*)name, floornum, which);
	//一开始电梯是沉睡的
	elevatorlock1->Acquire();
	building->sleepelevator->Wait(elevatorlock1);
	elevatorlock1->Release();
	//重新被唤醒
	building->elevator[which]->keepworking();
}

//建筑
Building::Building(char* debugname, int numFloors, int numElevators)
{
	peoples = people;
	name = new char[30];
	strcpy(name, debugname);
	Buildingh = numFloors;
	Elevator_num = numElevators;
	uppeople = new EventBarrier * [numFloors + 1];
	downpeople = new EventBarrier * [numFloors + 1];
	//创建两个事件栅栏
	for (int i = 0; i < numFloors + 1; i++)
	{
		uppeople[i] = new EventBarrier;
		downpeople[i] = new EventBarrier;
	}
	//初始时没有人请求电梯
	callup = new int[numFloors + 1];
	calldown = new int[numFloors + 1];
	for (int i = 1; i <= numFloors; i++)
	{
		callup[i] = calldown[i] = 0;
	}
	//初始时所有电梯都静止
	elevatorup = new int[numFloors + 1];
	elevatordown = new int[numFloors + 1];
	for (int i = 1; i <= numFloors; i++)
	{
		elevatorup[i] = elevatordown[i] = -1;
	}
	peoplerequestup = new DLList;
	peoplerequestdown = new DLList;
	//创建悬挂沉睡电梯的条件变量
	sleepelevator = new Condition((char*)"sleep_elevator");
	//printf("wrong here.\n");
	elevator = new Elevator * [numElevators];
	for (int i = 0; i < numElevators; i++)
	{
		//创建电梯线程
		Thread* t;
		char name[30] = "Elevator ";
		name[9] = i+'0';
		name[10] = '\0';
		t = new Thread((char*)name);
		t->Fork(Createlevator, i);
	}
}

Building::~Building()
{
	//删除掉事件栅栏
	for (int i = 1; i <= Buildingh; i++)
	{
		delete uppeople[i];
		delete downpeople[i];
	}
	delete[]uppeople;
	delete[]downpeople;
	//删除掉四个记录的数组
	delete callup;
	delete calldown;
	delete elevatorup;
	delete elevatordown;
}

//请求电梯向上走
void Building::CallUp(int fromFloor)
{
	callup[fromFloor] = true;
	//记录下该乘客的方向
	//call(fromFloor,peo_up);
	//唤醒所有沉睡的电梯
	elevatorlock1->Acquire();
	sleepelevator->Broadcast(elevatorlock1);
	elevatorlock1->Release();
	//printf("finish.\n");
}

//请求电梯向下走
void Building::CallDown(int fromFloor)
{
	calldown[fromFloor] = true;
	//call(fromFloor,peo_down);
	elevatorlock1->Acquire();
	sleepelevator->Broadcast(elevatorlock1);
	elevatorlock1->Release();
	//printf("finish.\n");
}

//返回可以进入的电梯
Elevator* Building::AwaitUp(int fromFloor)
{
	//等待某一电梯停靠该楼层
	uppeople[fromFloor]->Wait();
	//返回当前到达该楼层且上升的电梯
	int id = elevatorup[fromFloor];
	printf("%s choose elevator %d.\n", currentThread->getName(), id);
	return elevator[id];
}

//返回可以进入的电梯
Elevator* Building::AwaitDown(int fromFloor)
{
	downpeople[fromFloor]->Wait();
	int id = elevatordown[fromFloor];
	printf("%s choose elevator %d.\n", currentThread->getName(), id);
	return elevator[id];
}

//(尝试一开始)记录乘客的要求
//分成两个链表，要求向上的和要求向下的
void Building::call(int floor, peo_direction pdir)
{
	//要向上走,从低到高
	//printf("wrong here.\n");
	if (pdir == peo_up)
	{
		listlock->Acquire();
		void* item;
		peoplerequestup->SortedInsert(item, floor);
		listlock->Release();
	}
	//向下走，从高到低（为了可以排序，使用负数）
	else if (pdir == peo_down)
	{
		listlock->Acquire();
		void* item;
		peoplerequestdown->SortedInsert(item, -floor);
		listlock->Release();
	}
}

//使用类似C-SCAN的方法取出下一个
//首先到达要求向上的最底层，一直到达最高层的目的地之后，前往要求向下的最高层，一直到达最底层的目的地
//考虑多种情况，电梯一次进行上和下交替，避免饿死
int Building::next_request(ele_direction edir)
{
	int nextfloor = 0;
	listlock->Acquire();
	void* item;
	//先到达最底层
	//如果当前电梯向上完成一批任务后，如果向下有工作，向下。
	if (edir == up)
	{
		if (peoplerequestdown->IsEmpty())
		{
			item = peoplerequestdown->Remove(&nextfloor);
			//nextfloor = -nextfloor;
		}
		//如果向下没工作，就向上
		else if (peoplerequestup->IsEmpty())
		{
			item = peoplerequestup->Remove(&nextfloor);
		}
	}
	else
	{
		if (peoplerequestup->IsEmpty())
		{
			item = peoplerequestup->Remove(&nextfloor);
		}
		else if (peoplerequestdown->IsEmpty())
		{
			item = peoplerequestdown->Remove(&nextfloor);
			//nextfloor = -nextfloor;
		}
	}
	listlock->Release();
	return nextfloor;
}

//取出同一个楼层相同方向的乘客
void Building::next_request_batch(int nextfloor)
{
	listlock->Acquire();
	if (nextfloor < 0)
	{
		void* item;
		item = peoplerequestdown->SortedRemove(nextfloor);
		while (item != NULL)
		{
			item = peoplerequestdown->SortedRemove(nextfloor);
		}
	}
	else if (nextfloor > 0)
	{
		void* item;
		item = peoplerequestup->SortedRemove(nextfloor);
		while (item != NULL)
		{
			item = peoplerequestup->SortedRemove(nextfloor);
		}
	}
	listlock->Release();
}

void rider(int id, int srcFloor, int dstFloor)//电梯序号，乘客的起始楼层、目的楼层
{
	Elevator* e;
	int flag = 0;
	if (srcFloor == dstFloor)
		return;
	//printf("%d %d %d.\n", id, srcFloor, dstFloor);
	do
	{
		if (srcFloor < dstFloor)
		{
			printf("Rider %d CallUp       (%d)\n", id, srcFloor);
			building->CallUp(srcFloor);//叫电梯
			printf("Rider %d AwaitUp      (%d)\n", id, srcFloor);
			e = building->AwaitUp(srcFloor);//等电梯到来并向上
		}
		else
		{
			printf("Rider %d CallDown     (%d)\n", id, srcFloor);
			building->CallDown(srcFloor);
			printf("Rider %d AwaitDown    (%d)\n", id, srcFloor);
			e = building->AwaitDown(srcFloor);//等电梯到来并向下
			//printf("@@@here %s.\n", e->getName());
		}
		printf("Rider %d Enter\n", id);

	} while (!e->Enter());	// 电梯要是满了剩下的人就一直叫电梯
	printf("Rider %d RequestFloor (%d)\n", id, dstFloor);
	e->RequestFloor(dstFloor);
	printf("Rider %d Exit         (%d)\n", id, dstFloor);
	e->Exit();
	printf("Rider %d finished\n", id);
}