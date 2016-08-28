#ifndef reloc_node
#define reloc_node

#include "Symbol.h"

class RelocNode {
private:
	int memoryLocation;
	int offset;
	std::string section;
	std::string type;
	int number;
	Symbol sym;
	RelocNode* next;
public:
	RelocNode(int mem);
	RelocNode(int mem, int offset);
	RelocNode(int mem, int offset, std::string section);
	RelocNode(int mem, int offset, std::string section, std::string type);
	RelocNode(int mem, int offset, std::string section, std::string type, int number);
	RelocNode(int mem, int offset, std::string section, std::string type, int number, Symbol sym);
	RelocNode(int mem, int offset, std::string section, std::string type, int number, Symbol sym, RelocNode* next);
	int getMemoryLocation();
	void setMemoryLocation(int loc);
	int getOffset();
	void setOffset(int offset);
	std::string getSectionName();
	void setSectionName(std::string section);
	std::string getType();
	void setType(std::string t);
	int getNumber();
	void setNumber(int number);
	Symbol getSymbol();
	void setSymbol(Symbol s);
	RelocNode* getNext();
	void setNext(RelocNode* nextNode);
};

#endif