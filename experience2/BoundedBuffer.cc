#include"BoundedBuffer.h"

//创建一个缓冲区
BoundedBuffer::BoundedBuffer(int maxsize)
{
	sizeofbuffer = maxsize;
	usedsize = 0;
	buffer = new long[maxsize];
	for (int i = 0; i < maxsize; i++)
		buffer[i] = -1;
	const char* name1 = "s";
	const char* name2 = "n";
	const char* name3 = "e";
	s = new Semaphore((char*)name1, 1);
	n = new Semaphore((char*)name2, 0);
	e = new Semaphore((char*)name3, maxsize);
	write = read = 0;
}

//缓冲区的析构函数
BoundedBuffer::~BoundedBuffer()
{
	delete s;
	delete n;
	delete e;
	delete buffer;
}

void BoundedBuffer::Read(void* data, int size)
{
	if (size > sizeofbuffer || size < 0)
	{
		printf("Out of range!\n");
		return;
	}
	for (int i = 0; i < size; i++)
	{
		n->P();
		s->P();
		((long*)data)[i] = buffer[(read + i) % sizeofbuffer];
		s->V();
		e->V();
		printf("data[%d] is %ld.\n", i, ((long*)data)[i]);
		usedsize--;
	}
	read = (read + size) % sizeofbuffer;
}


void BoundedBuffer::Write(void* data, int size)
{
	if (size > sizeofbuffer || size < 0)
	{
		printf("Out of range!\n");
		return;
	}
	for (int i = 0; i < size; i++)
	{
		printf("%ld is writing in buffer.\n", ((long*)data)[i]);
		usedsize++;
		e->P();
		s->P();
		buffer[(write + i) % sizeofbuffer] = ((long*)data)[i];
		s->V();
		n->V();
	}
	write = (write + size) % sizeofbuffer;
}

void BoundedBuffer::Print()
{
	for (int i = 0; i < sizeofbuffer; i++)
		printf("%ld ", buffer[i]);
}