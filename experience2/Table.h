/*

Table implements a simple fixed-size table, a common kernel data
structure.  The table consists of "size" entries, each of which
holds a pointer to an object.  Each object in the table can be
named by an index in the range [0..size-1], corresponding to its
position in the table.  Table::Alloc allocates a free entry,
stores an object pointer in it, and returns its index.  The
object pointer can be retrieved by passing its index to Table::Get.
An entry is released by passing its index to Table::Release.

Table knows nothing about the objects it indexes.  In particular,
it is the responsibility of the caller to delete each object when
it is no longer needed (some time after the table entry is released).

It is also the caller's responsibility to correctly handle the types
of the objects stored in the Table.  The object pointer in each Table
entry is untyped (void*).  It is necessary to cast an object pointer
to a (void *) before storing it in the table, and to cast it back to
its correct type (e.g., (Process *)) after retrieving it with Get.
A more sophisticated solution would use parameterized types.aux

In later assignments, the Table class may be used to implement internal
operating system tables of processes, threads, memory page frames, open
files, etc.

*/

#include"synch.h"

class Table {
   public:
     // create a table to hold at most 'size' entries.
     Table(int size);//(原来这里有错)
     //析构函数
     ~Table();
     // allocate a table slot for 'object'.
     // return the table index for the slot or -1 on error.
     int Alloc(void *object);
   
     // return the object from table index 'index' or NULL on error.
     // (assert index is in range).  Leave the table entry allocated
     // and the pointer in place.
     void *Get(int index);
   
     // free a table slot
     void Release(int index);

     //为了检验上述函数是否正确，打印出表格内的所有信息
     void Print_Table();

     //获得当前表的容量
     int Getsize();
   private:
       int table_size;//表内的空间大小
       int present_size;//表内现有的元素个数
       void** table;//表内每个元素又是一个指针，所以使用void**
       Lock* table_lock;//用锁和条件变量来保证安全
       Condition* table_full;//表格已经满了
       Condition* table_empty;//表格已经空了
     // Your code here.
};

