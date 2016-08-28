#ifndef symtable
#define symtable

#include "Symbol.h"
#include <map>

class SymbolTable {
private:
	std::map<std::string, Symbol>* symbols;
public:
	SymbolTable();
	~SymbolTable();
	void put(std::string key, Symbol sym);
	Symbol get(std::string key);
	bool contains(std::string key);
	int size();
	std::map<std::string, Symbol>::iterator begin();
	std::map<std::string, Symbol>::iterator end();
};

#endif