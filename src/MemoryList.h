#ifndef mem_list
#define mem_list

#include "Memory.h"

class MemoryList {
private:
	Memory* head;
public:
	MemoryList();
	MemoryList(Memory* head);
	~MemoryList();
	void addNode(Memory* node);
	Memory* peek();
	Memory* pop();
};

#endif