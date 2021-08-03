#include<iostream>
#include"dllist.h"
using namespace std;
int pre;
//结点构造函数
DLLElement::DLLElement(void* itemPtr, int sortKey)
{
	item = itemPtr;
	key = sortKey;
	next = NULL;
	prev = NULL;
}

//双向链表的构造函数
DLList::DLList()
{
	first = NULL;
	last = NULL;
}

//双向链表的析构函数
DLList::~DLList()
{
	while (IsEmpty())
	{
		int key;
		Remove(&key);
	}
}

//双向链表前插结点
void DLList::Prepend(void* item)
{
	//链表为空
	DLLElement* ins;
	if (!IsEmpty())
	{
		ins=new DLLElement (item, 0);
		first = ins;
		last = ins;
		//cout << "Prepend item " << (int)ins->item << "  with key " << 0 << endl;
	}
	//否则链表不为空
	else
	{
		ins = new DLLElement(item, first->key - 1);
		first->prev = ins;
		ins->next = first;
		first = ins;
		//cout << "Prepend item " << (int)ins->item << "  with key " << ins->key << endl;
	}
	cout << "Prepend item " << (long)ins->item << "  with key " << ins->key << endl;
}
//双向链表后插入结点
void DLList::Append(void* item)
{
	//如果链表为空
	if (!IsEmpty())
	{
		DLLElement* ins = new DLLElement(item, 0);
		first = ins;
		last = ins;
		cout << "Apend item " << (long)ins->item << "  with key " << ins->key << endl;
	}
	//如果不为空
	else
	{
		DLLElement* ins = new DLLElement(item, last->key + 1);
		last->next = ins;
		ins->prev = last;
		last = ins;
		cout << "Apend item " << (long)ins->item << "  with key " << ins->key << endl;
	}
}
//删除头结点
void* DLList::Remove(int* keyPtr)
{
	if (!IsEmpty())
	{
		cout <<"List empty!" << endl;
		return NULL;
	}
	else
	{
		*keyPtr = first->key;
		DLLElement* p = first;
		first = p->next;
		//如果删除后链表为空
		if (first == NULL)
			last = NULL;
		else
		{
			first->prev = NULL;
		}
		void* item = p->item;
		cout << "Remove item  " << (long)p->item << "  with key  " << p->key << endl;
		delete p;
		return item;
	}
}


//判断链表是否为空,不为空则返回true
bool DLList::IsEmpty()
{
	if (first == NULL)
		return false;
	return true;
}

//根据key插入结点
void DLList::SortedInsert(void* item, int sortKey)
{
	//如果链表为空
	DLLElement* ins = new DLLElement(item, sortKey);
	if (!IsEmpty())
	{
		first = ins;
		last = ins;
	}
	else
	{
		DLLElement* p = first;
		for (; p != NULL; p = p->next)//find the location
		{
			if (p->key > sortKey)
			{
				break;
			}
		}
		//如果需要在第一个插入
		if (p == first)
		{
			first->prev = ins;
			ins->next = first;
			first = ins;
		}
		//如果需要在最后一个插入
		else if (p == NULL)
		{
			last->next = ins;
			ins->prev = last;
			last = ins;
		}
		//否则
		else
		{
			ins->prev = p->prev;
			ins->next = p;
			ins->prev->next = ins;
			p->prev = ins;
		}
	}
	cout << "Insert item  " << (long)item << "  with key  " << sortKey << "  successfully!" << endl;
}

//根据key删除结点
void* DLList::SortedRemove(int sortKey)
{
	if (!IsEmpty())
		return NULL;
	else
	{
		DLLElement* pre = first;
		while (pre!=NULL)
		{
			if (pre->key == sortKey)
				break;
			else
			{
				pre = pre->next;
			}
		}
		//没有找到
		if (pre == NULL)
			return NULL;
		else
		{
			void* item;
			//是第一个
			if (pre == first)
			{
				DLLElement* f2 = first;
				first = f2->next;
				first->prev = NULL;
				item = f2->item;
				delete f2;
			}
			//如果是最后一个
			else if(pre==last)
			{
				DLLElement* l2 = last;
				last = last->prev;
				last->next = NULL;
				item = l2->item;
				delete l2;
			}
			//否则
			else
			{
				DLLElement* p1 = pre->prev, * l1 = pre->next;
				item = pre->item;
				p1->next = l1;
				l1->prev = p1;
				delete pre;
			}
			return item;
		}
	}
}