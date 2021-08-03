#include"Elevator.h"

extern int ele_load;//���ݵ��ؿ���
extern int people;//Ҫ�������ݵ���
extern int floornum;//�����߶�
extern Building* building;//����

Lock* elevatorlock = new Lock((char*)"Elevator");
Lock* elevatorlock1 = new Lock((char*)"Elevator1");
Lock* listlock=new Lock((char*)"List");
Elevator::Elevator(char* debugName, int numFloors, int myID)
{
	name = new char[30];
	strcpy(name, debugName);
	elevator_highest = numFloors;
	ele_id = myID;
	currentfloor = 1;//��ͣ��1¥
	occupancy = 0;
	load = ele_load;//���ݵ��ؿ���
	dir = stop;
	person_exit = new EventBarrier * [numFloors + 1];
	//ÿһ��¥�㴴��һ���¼�դ��
	//��ʼʱ���е�¥�㶼û��ͣ������Ҫ
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

//���ݵĿ��š����š�ͣ������

//���ݿ��ţ���������򵽵�
void Elevator::OpenDoors()//   signal exiters and enterers to action
{
	printf("Elevator %d open the door at %d.\n", ele_id, currentfloor);
	//������¥����Ҫͣ��
	if (is_exit[currentfloor])
	{
		//�����е��˶�������
		person_exit[currentfloor]->Signal();
		is_exit[currentfloor] = false;
	}
	//������ݷ������ϣ��Ҹò�¥����Ҫ����
	if (building->callup[currentfloor] && (dir == up || dir == stop))
	{
		//��¼�µ�ǰ���ݵ�ID
		building->elevatorup[currentfloor] = ele_id;
		//����ǰ��ͬ¥����ͬ���������
		building->next_request_batch(currentfloor);
		//֪ͨ��ǰҪ��ȥ���˿��Խ���
		building->uppeople[currentfloor]->Signal();
		//�ж��Ƿ�ᳬ��
		//���������
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
		//��������
		building->next_request_batch(-currentfloor);
		building->downpeople[currentfloor]->Signal();
		//printf("now is %d.\n", occupancy);
		//�ж��Ƿ�ᳬ��
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

//���ݹ��ţ���������򵽵�
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

//����ͣ��
void Elevator::VisitFloor(int floor)      //   go to a particular floor
{
	currentfloor = floor;
	printf("Elevator %d visits at %d.\n", ele_id, currentfloor);
	//����
	OpenDoors();
	//�����������ŵĳ˿Ͷ�����
	currentThread->Yield();
	//����
	CloseDoors();
}

//�˿�
//�˿ͽ������
bool Elevator::Enter()
{
	//���Լ���������
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

//�˿��˳�����
void Elevator::Exit()
{
	occupancy--;
	person_exit[currentfloor]->Complete();
}

//�˿͸���Ŀ�ĵ�
void Elevator::RequestFloor(int floor)
{
	is_exit[floor] = true;
	person_exit[floor]->Wait();
}

//���ݳ�������
void Elevator::keepworking()
{
	while (true)
	{
		elevatorlock1->Acquire();
		//��ǰ����û�б�ʹ��
		if (!occupancy)
		{
			int next = building->next_request(dir);
			//û�й�����
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

//����һ������
void Createlevator(int which)
{
	char name[30] = "elevator ";
	name[9] = which + '0';
	name[10] = '\0';
	building->elevator[which] = new Elevator((char*)name, floornum, which);
	//һ��ʼ�����ǳ�˯��
	elevatorlock1->Acquire();
	building->sleepelevator->Wait(elevatorlock1);
	elevatorlock1->Release();
	//���±�����
	building->elevator[which]->keepworking();
}

//����
Building::Building(char* debugname, int numFloors, int numElevators)
{
	peoples = people;
	name = new char[30];
	strcpy(name, debugname);
	Buildingh = numFloors;
	Elevator_num = numElevators;
	uppeople = new EventBarrier * [numFloors + 1];
	downpeople = new EventBarrier * [numFloors + 1];
	//���������¼�դ��
	for (int i = 0; i < numFloors + 1; i++)
	{
		uppeople[i] = new EventBarrier;
		downpeople[i] = new EventBarrier;
	}
	//��ʼʱû�����������
	callup = new int[numFloors + 1];
	calldown = new int[numFloors + 1];
	for (int i = 1; i <= numFloors; i++)
	{
		callup[i] = calldown[i] = 0;
	}
	//��ʼʱ���е��ݶ���ֹ
	elevatorup = new int[numFloors + 1];
	elevatordown = new int[numFloors + 1];
	for (int i = 1; i <= numFloors; i++)
	{
		elevatorup[i] = elevatordown[i] = -1;
	}
	peoplerequestup = new DLList;
	peoplerequestdown = new DLList;
	//�������ҳ�˯���ݵ���������
	sleepelevator = new Condition((char*)"sleep_elevator");
	//printf("wrong here.\n");
	elevator = new Elevator * [numElevators];
	for (int i = 0; i < numElevators; i++)
	{
		//���������߳�
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
	//ɾ�����¼�դ��
	for (int i = 1; i <= Buildingh; i++)
	{
		delete uppeople[i];
		delete downpeople[i];
	}
	delete[]uppeople;
	delete[]downpeople;
	//ɾ�����ĸ���¼������
	delete callup;
	delete calldown;
	delete elevatorup;
	delete elevatordown;
}

//�������������
void Building::CallUp(int fromFloor)
{
	callup[fromFloor] = true;
	//��¼�¸ó˿͵ķ���
	//call(fromFloor,peo_up);
	//�������г�˯�ĵ���
	elevatorlock1->Acquire();
	sleepelevator->Broadcast(elevatorlock1);
	elevatorlock1->Release();
	//printf("finish.\n");
}

//�������������
void Building::CallDown(int fromFloor)
{
	calldown[fromFloor] = true;
	//call(fromFloor,peo_down);
	elevatorlock1->Acquire();
	sleepelevator->Broadcast(elevatorlock1);
	elevatorlock1->Release();
	//printf("finish.\n");
}

//���ؿ��Խ���ĵ���
Elevator* Building::AwaitUp(int fromFloor)
{
	//�ȴ�ĳһ����ͣ����¥��
	uppeople[fromFloor]->Wait();
	//���ص�ǰ�����¥���������ĵ���
	int id = elevatorup[fromFloor];
	printf("%s choose elevator %d.\n", currentThread->getName(), id);
	return elevator[id];
}

//���ؿ��Խ���ĵ���
Elevator* Building::AwaitDown(int fromFloor)
{
	downpeople[fromFloor]->Wait();
	int id = elevatordown[fromFloor];
	printf("%s choose elevator %d.\n", currentThread->getName(), id);
	return elevator[id];
}

//(����һ��ʼ)��¼�˿͵�Ҫ��
//�ֳ���������Ҫ�����ϵĺ�Ҫ�����µ�
void Building::call(int floor, peo_direction pdir)
{
	//Ҫ������,�ӵ͵���
	//printf("wrong here.\n");
	if (pdir == peo_up)
	{
		listlock->Acquire();
		void* item;
		peoplerequestup->SortedInsert(item, floor);
		listlock->Release();
	}
	//�����ߣ��Ӹߵ��ͣ�Ϊ�˿�������ʹ�ø�����
	else if (pdir == peo_down)
	{
		listlock->Acquire();
		void* item;
		peoplerequestdown->SortedInsert(item, -floor);
		listlock->Release();
	}
}

//ʹ������C-SCAN�ķ���ȡ����һ��
//���ȵ���Ҫ�����ϵ���ײ㣬һֱ������߲��Ŀ�ĵ�֮��ǰ��Ҫ�����µ���߲㣬һֱ������ײ��Ŀ�ĵ�
//���Ƕ������������һ�ν����Ϻ��½��棬�������
int Building::next_request(ele_direction edir)
{
	int nextfloor = 0;
	listlock->Acquire();
	void* item;
	//�ȵ�����ײ�
	//�����ǰ�����������һ���������������й��������¡�
	if (edir == up)
	{
		if (peoplerequestdown->IsEmpty())
		{
			item = peoplerequestdown->Remove(&nextfloor);
			//nextfloor = -nextfloor;
		}
		//�������û������������
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

//ȡ��ͬһ��¥����ͬ����ĳ˿�
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

void rider(int id, int srcFloor, int dstFloor)//������ţ��˿͵���ʼ¥�㡢Ŀ��¥��
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
			building->CallUp(srcFloor);//�е���
			printf("Rider %d AwaitUp      (%d)\n", id, srcFloor);
			e = building->AwaitUp(srcFloor);//�ȵ��ݵ���������
		}
		else
		{
			printf("Rider %d CallDown     (%d)\n", id, srcFloor);
			building->CallDown(srcFloor);
			printf("Rider %d AwaitDown    (%d)\n", id, srcFloor);
			e = building->AwaitDown(srcFloor);//�ȵ��ݵ���������
			//printf("@@@here %s.\n", e->getName());
		}
		printf("Rider %d Enter\n", id);

	} while (!e->Enter());	// ����Ҫ������ʣ�µ��˾�һֱ�е���
	printf("Rider %d RequestFloor (%d)\n", id, dstFloor);
	e->RequestFloor(dstFloor);
	printf("Rider %d Exit         (%d)\n", id, dstFloor);
	e->Exit();
	printf("Rider %d finished\n", id);
}