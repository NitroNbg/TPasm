#include "SymbolTable.h"

SymbolTable::SymbolTable() {
	this->symbols = new std::map<std::string, Symbol>();
}

SymbolTable::~SymbolTable() {
	delete this->symbols;
}

void SymbolTable::put(std::string key, Symbol sym) {
	if (!this->symbols->insert(std::pair<std::string, Symbol>(key, sym)).second) {
		this->symbols->erase(key);
		this->symbols->insert(std::pair<std::string, Symbol>(key, sym));
	}
}

Symbol SymbolTable::get(std::string key) {
	return this->symbols->at(key);
}

bool SymbolTable::contains(std::string key) {
	return this->symbols->find(key) != this->symbols->end();
}

int SymbolTable::size() {
	return this->symbols->size();
}

std::map<std::string, Symbol>::iterator SymbolTable::begin() {
	return this->symbols->begin();
}

std::map<std::string, Symbol>::iterator SymbolTable::end() {
	return this->symbols->end();
}