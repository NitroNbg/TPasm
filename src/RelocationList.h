#ifndef reloc_list
#define reloc_list

#include "RelocNode.h"
#include "Symbol.h"

class RelocationList {
private:
	RelocNode* head;
public:
	RelocationList();
	RelocationList(RelocNode* head);
	~RelocationList();
	void addNode(RelocNode* node);
	RelocNode* peek();
	RelocNode* pop();
	RelocNode* findByValue(Symbol s);
	RelocNode* deleteNode(RelocNode* node);
};

#endif