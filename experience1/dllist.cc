#include<iostream>
#include"dllist.h"
using namespace std;
int pre;
//��㹹�캯��
DLLElement::DLLElement(void* itemPtr, int sortKey)
{
	item = itemPtr;
	key = sortKey;
	next = NULL;
	prev = NULL;
}

//˫������Ĺ��캯��
DLList::DLList()
{
	first = NULL;
	last = NULL;
}

//˫���������������
DLList::~DLList()
{
	while (IsEmpty())
	{
		int key;
		Remove(&key);
	}
}

//˫������ǰ����
void DLList::Prepend(void* item)
{
	//����Ϊ��
	DLLElement* ins;
	if (!IsEmpty())
	{
		ins=new DLLElement (item, 0);
		first = ins;
		last = ins;
		//cout << "Prepend item " << (int)ins->item << "  with key " << 0 << endl;
	}
	//��������Ϊ��
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
//˫������������
void DLList::Append(void* item)
{
	//�������Ϊ��
	if (!IsEmpty())
	{
		DLLElement* ins = new DLLElement(item, 0);
		first = ins;
		last = ins;
		cout << "Apend item " << (long)ins->item << "  with key " << ins->key << endl;
	}
	//�����Ϊ��
	else
	{
		DLLElement* ins = new DLLElement(item, last->key + 1);
		last->next = ins;
		ins->prev = last;
		last = ins;
		cout << "Apend item " << (long)ins->item << "  with key " << ins->key << endl;
	}
}
//ɾ��ͷ���
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
		//���ɾ��������Ϊ��
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


//�ж������Ƿ�Ϊ��,��Ϊ���򷵻�true
bool DLList::IsEmpty()
{
	if (first == NULL)
		return false;
	return true;
}

//����key������
void DLList::SortedInsert(void* item, int sortKey)
{
	//�������Ϊ��
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
		//�����Ҫ�ڵ�һ������
		if (p == first)
		{
			first->prev = ins;
			ins->next = first;
			first = ins;
		}
		//�����Ҫ�����һ������
		else if (p == NULL)
		{
			last->next = ins;
			ins->prev = last;
			last = ins;
		}
		//����
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

//����keyɾ�����
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
		//û���ҵ�
		if (pre == NULL)
			return NULL;
		else
		{
			void* item;
			//�ǵ�һ��
			if (pre == first)
			{
				DLLElement* f2 = first;
				first = f2->next;
				first->prev = NULL;
				item = f2->item;
				delete f2;
			}
			//��������һ��
			else if(pre==last)
			{
				DLLElement* l2 = last;
				last = last->prev;
				last->next = NULL;
				item = l2->item;
				delete l2;
			}
			//����
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