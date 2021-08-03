#include"Table.h"
#include"synch.h"
#include"copyright.h"
#include"system.h"

Table::Table(int size)
{
	const char* name1 = "Table lock";
	const char* name2 = "empty condition";
	const char* name3 = "full condition";
	table_lock = new Lock((char*)name1);
	table_empty = new Condition((char*)name2);
	table_full = new Condition((char*)name3);
	table_size = size;
	table = new void* [size + 1];
	for (int i = 0; i < table_size; i++)
	{
		table[i] = NULL;
	}
	present_size = 0;
}

//删除掉表格
Table::~Table()
{
	delete[]table;
	present_size = 0;
	table_size = 0;
	delete table_empty;
	delete table_full;
	delete table_lock;
}

//在表格中分配一块内存空间
int Table::Alloc(void* object)
{
	table_lock->Acquire();
	//如果表格已满，等待
	while (present_size==table_size)
	{
		table_full->Wait(table_lock);
	}
	int res=-1;
	//表格没满
	for(int i = 0; i < table_size; i++)
	{
		if (table[i] == NULL)
		{
			table[i] = object;
			printf("table[%d] is %ld\n", i, (long)table[i]);
			present_size++;
			res = i;
			break;
		}
	}
	table_empty->Signal(table_lock);
	table_lock->Release();
	return res;
}

//获取指定下标的内容
void* Table::Get(int index)
{
	table_lock->Acquire();
	void* ret;
	if (index > table_size || index < 0 || table[index] == NULL)
	{
		ret = NULL;
		printf("table[%d] get object error!\n", index);
	}
	else
	{
		ret = table[index];
		printf("table[%d] is %ld\n", index,(long)ret);
	}
	table_lock->Release();
	return ret;
}


void Table::Release(int index)
{
	table_lock->Acquire();
	while (present_size == 0)
		table_empty->Wait(table_lock);
	if (index > table_size || index < 0)
	{
		printf("\tIndex out of table range!\n");
	}
	else if(table[index]!=NULL)
	{
		table[index] = NULL;
		printf("\tRelease table[%d] successfully!\n",index);
		table_full->Signal(table_lock);
		present_size--;
	}
	else
	{
		printf("The data has been released before!\n");
	}
	table_lock->Release();
}

void Table::Print_Table()
{
	printf("Table size is %d, and now the table has %d objects.\n", table_size,present_size);
	printf("Now print the information of these objects are:\n");
	for (int i = 0; i < table_size; i++)
	{
		printf("***table[%d]:",i);
		if (table[i] == NULL)
		{
			printf("the slot has no object\n");
		}
		else
		{
			printf("the object's data is:%ld\n", (long)table[i]);
		}
	}
}

int Table::Getsize()
{
	return present_size;
}