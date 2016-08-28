#include "RelocNode.h"
#include "Constants.h"

RelocNode::RelocNode(int mem) : RelocNode(mem, -1) {}
RelocNode::RelocNode(int mem, int offset) : RelocNode(mem, offset, "UND") {}
RelocNode::RelocNode(int mem, int offset, std::string section) : RelocNode(mem, offset, section, Constants::R_386_32) {}
RelocNode::RelocNode(int mem, int offset, std::string section, std::string type) : RelocNode(mem, offset, section, type, -1) {}
RelocNode::RelocNode(int mem, int offset, std::string section, std::string type, int number) : RelocNode(mem, offset, section, type, number, Constants::INVALID_SYMBOL) {}
RelocNode::RelocNode(int mem, int offset, std::string section, std::string type, int number, Symbol sym) : RelocNode(mem, offset, section, type, number, sym, nullptr) {}
RelocNode::RelocNode(int mem, int offset, std::string section, std::string type, int number, Symbol sym, RelocNode* next) {
	this->memoryLocation = mem;
	this->offset = offset;
	this->section = section;
	this->type = type;
	this->number = number;
	this->sym = sym;
	this->next = next;
}
int RelocNode::getMemoryLocation() {
	return this->memoryLocation;
}
void RelocNode::setMemoryLocation(int loc) {
	this->memoryLocation = loc;
}
int RelocNode::getOffset() {
	return this->offset;
}
void RelocNode::setOffset(int offset) {
	this->offset = offset;
}
std::string RelocNode::getSectionName() {
	return this->section;
}
void RelocNode::setSectionName(std::string section) {
	this->section = section;
}
std::string RelocNode::getType() {
	return this->type;
}
void RelocNode::setType(std::string t) {
	this->type = t;
}
int RelocNode::getNumber() {
	return this->number;
}
void RelocNode::setNumber(int number) {
	this->number = number;
}
Symbol RelocNode::getSymbol() {
	return this->sym;
}
void RelocNode::setSymbol(Symbol s) {
	this->sym = s;
}
RelocNode* RelocNode::getNext() {
	return this->next;
}
void RelocNode::setNext(RelocNode* nextNode) {
	this->next = nextNode;
}