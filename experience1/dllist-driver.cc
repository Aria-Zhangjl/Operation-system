
#include "dllist.cc"
#include<iostream>
using namespace std;

//第thread个线程中插入n个结点
void InsertNitem(DLList* list, int n,int thread)
{
	int* k = new int[n];
	for (int i = 0; i < n; i++)
	{
		k[i] = i;
		void* item=&k[i];
		list->SortedInsert(item, k[i]);
	}
	cout << "Insert  " << n << "  item in thread  " << thread << "  successfully!" << endl;
}


void RemoveNitem(DLList* list, int n, int thread)
{
	void* item;
	int key;
	for (int i = 0; i < n; i++)
	{
		if (!list->IsEmpty())
		{
			cout << "List empty!" << endl;
			break;
		}
		item = list->Remove(&key);
	}
	cout << "Remove  "<<n<<"  item in thread  "<< thread<<"  successfully!" << endl;
}

