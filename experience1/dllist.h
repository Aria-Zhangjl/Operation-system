
class DLLElement {
public:
	DLLElement(void* itemPtr, int sortKey);
	DLLElement* next;
	DLLElement* prev;
	int key;
	void* item;
};
class DLList
{
public:
	DLList();
	~DLList();

	void Prepend(void* item);
	void Append(void* item);
	void* Remove(int* keyPtr);

	bool IsEmpty();

	void SortedInsert(void* item, int sortKey);
	void* SortedRemove(int sortKey);
private:
	DLLElement* first;
	DLLElement* last;
};

