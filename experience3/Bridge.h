#include "copyright.h"
#include "list.h"
#include "system.h"
#include"synch.h"
#include"Alarm.h"
extern Alarm *aclock;
enum bridgestate { freestate, leftbusy, rightbusy };
enum direction { none,rightside, leftside };
class Bridge
{
public:
	Bridge();
	~Bridge();
	void ArriveBridge(direction dir);
	void ExitBridge(direction dir);
	void CrossBridge(direction dir);

private:
	int state;//桥的状态，0表示没有车，1表示左边有车，2表示右边有车
	int carl;//左边的车辆
	int carr;//右边的车辆
	int next;//下一次等待的车辆
	direction nextdir;//下一次的方向
	int onbridge;//正在过桥的车
	Lock* bridgelock;
	Condition* lockr;//右边车辆的条件变量
	Condition* lockl;//右边车辆的条件变量
};
