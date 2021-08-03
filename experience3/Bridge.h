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
	int state;//�ŵ�״̬��0��ʾû�г���1��ʾ����г���2��ʾ�ұ��г�
	int carl;//��ߵĳ���
	int carr;//�ұߵĳ���
	int next;//��һ�εȴ��ĳ���
	direction nextdir;//��һ�εķ���
	int onbridge;//���ڹ��ŵĳ�
	Lock* bridgelock;
	Condition* lockr;//�ұ߳�������������
	Condition* lockl;//�ұ߳�������������
};
