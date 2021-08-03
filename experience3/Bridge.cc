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
	//这个时候没有车，车辆直接通行
	if (state == freestate)
	{
		//来自左边的车
		if (i == leftside)
		{
			state = leftbusy;
		}
		//来自右边的车
		else
		{
			state = rightbusy;
		}
	}
	//前面已经有左边的车
	else if (state == leftbusy)
	{
		//如果这辆车也是从左边来，判断是否可以上桥
		if (i == leftside)
		{
			if (onbridge == 3 || carl || (next>0 && nextdir==rightside))
			{
				carl++;
				printf("***%s from %s side is waiting.\n", currentThread->getName(),dispdir);
				lockl->Wait(bridgelock);
			}
		}
		//如果从右边来，直接等待
		else
		{
			carr++;
			printf("***%s from %s side is waiting.\n", currentThread->getName(),dispdir);
			lockr->Wait(bridgelock);
		}
	}
	//已经有右边来的车
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
	//输出过桥的信息
	printf("***%s is crossing the bridge from %s to %s.\n", currentThread->getName(), dispdir, dispdir2);
	onbridge++;
	aclock->Pause(100+ stats->totalTicks);
	//currentThread->Yield();
}

void Bridge::ExitBridge(direction dir)
{
	bridgelock->Acquire();
	int flag = 0;
	//当前左边准备过桥
	if (state == leftbusy)
	{
		//先把左边的车通过少于3辆
		if (onbridge)
		{
			onbridge--;
			//如果右边有车，下一次先让右边的车通过
			if (carr)
			{
				carr--;
				nextdir = rightside;
				next++;
				//lockr->Signal(bridgelock);
				flag = 2;
			}
			//如果没有，则左边继续通过
			//排除右边通过两辆车后左边加上一辆车的情况
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
	//桥上的车都已经通过了
	if (!onbridge)
	{
		if (nextdir == leftside)
		{
			state = leftbusy;
			//如果左边还有车，可以继续上
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