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

    void keepworking();//让电梯一直保持运转

    ele_direction dir;//电梯的工作方向

    int getcurrentFloor() { return currentfloor; }

private:
    char* name;

    int currentfloor;           // floor where currently stopped
    int occupancy;              // how many riders currently onboard
    int load;//电梯最大载客量
    EventBarrier** person_exit;//当前楼层要出电梯的乘客全部退出

    bool* is_exit;//当前楼层是否有人要出去
    int elevator_highest;
    int ele_id;//电梯的序号
    bool overload;//电梯是否会超载
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

    int* callup;//有人需要上升的电梯
    int* calldown;//有人需要下降的电梯

    int* elevatorup;//上升的电梯号
    int* elevatordown;//下降的电梯号

    EventBarrier** uppeople;//保证同一楼层所有的人都上电梯
    EventBarrier** downpeople;//保证同一楼层所有的人都下电梯

    int next_request(ele_direction edir);//处理下一个请求
    void next_request_batch(int nextfloor);//相同楼层的乘客一起进入电梯
    void call(int floor, peo_direction pdir);//记录乘客的要求
    DLList* peoplerequestup;//上面乘客的要求
    DLList* peoplerequestdown;//下面乘客的要求

    Condition* sleepelevator;//沉睡的电梯
    int Elevator_num;//电梯数

private:
    char* name;


    // insert your data structures here, if needed
    int Buildingh;//楼层高度
    int peoples;//要乘坐电梯的人数


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