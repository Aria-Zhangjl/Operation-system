/*

Here are the method signatures for the Elevator and Building classes.
You should feel free to add to these classes, but do not change the
existing interfaces.

*/
#pragma once
#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "system.h"
#include "synch.h"
#include "EventBarrier.h"
#include"dllist.h"

enum ele_direction
{
    up, down, stop
};
enum peo_direction
{
    peo_up, peo_down
};
class Elevator {
public:
    Elevator(char* debugName, int numFloors, int myID);
    ~Elevator();
    char* getName() { return name; }

    // elevator control interface: called by Elevator thread
    void OpenDoors();                //   signal exiters and enterers to action
    void CloseDoors();               //   after exiters are out and enterers are in
    void VisitFloor(int floor);      //   go to a particular floor

    // elevator rider interface (part 1): called by rider threads.
    bool Enter();                    //   get in
    void Exit();                     //   get out (iff destinationFloor)
    void RequestFloor(int floor);    //   tell the elevator our destinationFloor

    // insert your methods here, if needed

    void keepworking();//�õ���һֱ������ת

    ele_direction dir;//���ݵĹ�������

    int getcurrentFloor() { return currentfloor; }

private:
    char* name;

    int currentfloor;           // floor where currently stopped
    int occupancy;              // how many riders currently onboard
    int load;//��������ؿ���
    EventBarrier** person_exit;//��ǰ¥��Ҫ�����ݵĳ˿�ȫ���˳�

    bool* is_exit;//��ǰ¥���Ƿ�����Ҫ��ȥ
    int elevator_highest;
    int ele_id;//���ݵ����
    bool overload;//�����Ƿ�ᳬ��
    // insert your data structures here, if needed
};

class Building {
public:
    Building(char* debugname, int numFloors, int numElevators);
    ~Building();
    char* getName() { return name; }


    // elevator rider interface (part 2): called by rider threads
    void CallUp(int fromFloor);      //   signal an elevator we want to go up
    void CallDown(int fromFloor);    //   ... down
    Elevator* AwaitUp(int fromFloor); // wait for elevator arrival & going up
    Elevator* AwaitDown(int fromFloor); // ... down

    Elevator** elevator;         // the elevators in the building (array)

    int* callup;//������Ҫ�����ĵ���
    int* calldown;//������Ҫ�½��ĵ���

    int* elevatorup;//�����ĵ��ݺ�
    int* elevatordown;//�½��ĵ��ݺ�

    EventBarrier** uppeople;//��֤ͬһ¥�����е��˶��ϵ���
    EventBarrier** downpeople;//��֤ͬһ¥�����е��˶��µ���

    int next_request(ele_direction edir);//������һ������
    void next_request_batch(int nextfloor);//��ͬ¥��ĳ˿�һ��������
    void call(int floor, peo_direction pdir);//��¼�˿͵�Ҫ��
    DLList* peoplerequestup;//����˿͵�Ҫ��
    DLList* peoplerequestdown;//����˿͵�Ҫ��

    Condition* sleepelevator;//��˯�ĵ���
    int Elevator_num;//������

private:
    char* name;


    // insert your data structures here, if needed
    int Buildingh;//¥��߶�
    int peoples;//Ҫ�������ݵ�����


};

// here's a sample portion of a rider thread body showing how we
// expect things to be called.
//
// void rider(int id, int srcFloor, int dstFloor) {
//    Elevator *e;
//  
//    if (srcFloor == dstFloor)
//       return;
//  
//    DEBUG('t',"Rider %d travelling from %d to %d\n",id,srcFloor,dstFloor);
//    do {
//       if (srcFloor < dstFloor) {
//          DEBUG('t', "Rider %d CallUp(%d)\n", id, srcFloor);
//          building->CallUp(srcFloor);
//          DEBUG('t', "Rider %d AwaitUp(%d)\n", id, srcFloor);
//          e = building->AwaitUp(srcFloor);
//       } else {
//          DEBUG('t', "Rider %d CallDown(%d)\n", id, srcFloor);
//          building->CallDown(srcFloor);
//          DEBUG('t', "Rider %d AwaitDown(%d)\n", id, srcFloor);
//          e = building->AwaitDown(srcFloor);
//       }
//       DEBUG('t', "Rider %d Enter()\n", id);
//    } while (!e->Enter()); // elevator might be full!
//  
//    DEBUG('t', "Rider %d RequestFloor(%d)\n", id, dstFloor);
//    e->RequestFloor(dstFloor); // doesn't return until arrival
//    DEBUG('t', "Rider %d Exit()\n", id);
//    e->Exit();
//    DEBUG('t', "Rider %d finished\n", id);
// }

#endif