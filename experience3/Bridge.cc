#include"Bridge.h"
#include<string.h>
Bridge::Bridge()
{
	state = freestate;
	carl = 0;
	carr = 0;
	onbridge = 0;
	next = 0;
	nextdir = none;
	bridgelock = new Lock((char*)"Bridge");
	lockr = new Condition((char*)"Right car");
	lockl = new Condition((char*)"Left car");
}

Bridge::~Bridge()
{
	delete bridgelock;
	delete lockl;
	delete lockr;
}

void Bridge::ArriveBridge(direction i)
{
	bridgelock->Acquire();
	char* dispdir = new char[20];
	if (i == leftside)
		strcpy(dispdir, "left");
	else
	{
		strcpy(dispdir, "right");
	}
	printf("***%s is arriving at the bridge at %s side at %d.\n", currentThread->getName(), dispdir, stats->totalTicks);
	//���ʱ��û�г�������ֱ��ͨ��
	if (state == freestate)
	{
		//������ߵĳ�
		if (i == leftside)
		{
			state = leftbusy;
		}
		//�����ұߵĳ�
		else
		{
			state = rightbusy;
		}
	}
	//ǰ���Ѿ�����ߵĳ�
	else if (state == leftbusy)
	{
		//���������Ҳ�Ǵ���������ж��Ƿ��������
		if (i == leftside)
		{
			if (onbridge == 3 || carl || (next>0 && nextdir==rightside))
			{
				carl++;
				printf("***%s from %s side is waiting.\n", currentThread->getName(),dispdir);
				lockl->Wait(bridgelock);
			}
		}
		//������ұ�����ֱ�ӵȴ�
		else
		{
			carr++;
			printf("***%s from %s side is waiting.\n", currentThread->getName(),dispdir);
			lockr->Wait(bridgelock);
		}
	}
	//�Ѿ����ұ����ĳ�
	else if(state == rightbusy)
	{
		if (i == leftside)
		{
			carl++;
			printf("***%s from %s side is waiting.\n", currentThread->getName(), dispdir);
			lockl->Wait(bridgelock);
		}
		else
		{
			if (onbridge == 3 || carr ||(next>0 && nextdir==leftside))
			{
				carr++;
				printf("***%s from %s side is waiting.\n",  currentThread->getName(), dispdir);
				lockr->Wait(bridgelock);
			}
		}
	}
	bridgelock->Release();
}

void Bridge::CrossBridge(direction dir)
{
	char* dispdir=new char[20];
	char* dispdir2 = new char[20];
	if (dir == leftside)
	{
		strcpy(dispdir,  "left");
		strcpy(dispdir2, "right");
	}
	else
	{
		strcpy(dispdir, "right");
		strcpy(dispdir2, "left");
	}
	//������ŵ���Ϣ
	printf("***%s is crossing the bridge from %s to %s.\n", currentThread->getName(), dispdir, dispdir2);
	onbridge++;
	aclock->Pause(100+ stats->totalTicks);
	//currentThread->Yield();
}

void Bridge::ExitBridge(direction dir)
{
	bridgelock->Acquire();
	int flag = 0;
	//��ǰ���׼������
	if (state == leftbusy)
	{
		//�Ȱ���ߵĳ�ͨ������3��
		if (onbridge)
		{
			onbridge--;
			//����ұ��г�����һ�������ұߵĳ�ͨ��
			if (carr)
			{
				carr--;
				nextdir = rightside;
				next++;
				//lockr->Signal(bridgelock);
				flag = 2;
			}
			//���û�У�����߼���ͨ��
			//�ų��ұ�ͨ������������߼���һ���������
			else if(carl && nextdir !=rightside)
			{
				carl--;
				nextdir = leftside;
				next++;
				//lockl->Signal(bridgelock);
				flag = 1;
			}
			else if(carl==0 && carr==0 && (nextdir!=rightside && nextdir!=leftside))
			{
				state = freestate;
				nextdir = none;
				flag = 0;
			}
		}
	}
	else if (state == rightbusy)
	{
		if (onbridge)
		{
			onbridge--;
			if (carl)
			{
				carl--;
				nextdir = leftside;
				next++;
				//lockl->Signal(bridgelock);
				flag = 1;
			}
			else if (carr && nextdir !=leftside)
			{
				carr--;
				//lockr->Signal(bridgelock);
				next++;
				flag = 2;
				nextdir = rightside;
			}
			else if (carl == 0 && carr == 0)
			{
				flag = 0;
				state = freestate;
				//nextdir = none;
			}
		}
	}
	//���ϵĳ����Ѿ�ͨ����
	if (!onbridge)
	{
		if (nextdir == leftside)
		{
			state = leftbusy;
			//�����߻��г������Լ�����
			while (next < 3 && carl>0)
			{
				next++;
				carl--;
				//lockl->Signal(bridgelock);
			}
			while (next)
			{
				lockl->Signal(bridgelock);
				next--;
			}
		}
		else if (nextdir == rightside)
		{
			state = rightbusy;
			while (next < 3 && carr>0)
			{
				next++;
				carr--;
				//printf("hrer");
				//lockr->Signal(bridgelock);
			}
			while (next)
			{
				lockr->Signal(bridgelock);
				next--;
			}
		}
		nextdir = none;
	}
	printf("***%s exit the bridge at %d.\n",currentThread->getName(), stats->totalTicks);
	bridgelock->Release();
}