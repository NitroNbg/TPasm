#include "Symbol.h"

Symbol::Symbol() : Symbol("") {}

Symbol::Symbol(std::string name) : Symbol(name, "UND") {}

Symbol::Symbol(std::string name, std::string section) {
	this->name = name;
	this->defined = false;
	this->number = -1;
	this->offset = -1;
	this->sectionName = section;
	this->visibility = local;
}

std::string Symbol::getName() {
	return this->name;
}
void Symbol::setName(std::string name) {
	this->name = name;
}
std::string Symbol::getSectionName() {
	return this->sectionName;
}
void Symbol::setSectionName(std::string sectionName) {
	this->sectionName = sectionName;
}
int Symbol::getOffset() {
	return this->offset;
}
void Symbol::setOffset(int offset) {
	this->offset = offset;
}
Visibility Symbol::getVisibility() {
	return this->visibility;
}
void Symbol::setVisibility(Visibility visibility) {
	this->visibility = visibility;
}
std::string Symbol::printVisibility() {
	if (this->visibility == local) {
		return "local";
	}
	return "global";
}
int Symbol::getNumber() {
	return this->number;
}
void Symbol::setNumber(int number) {
	this->number = number;
}
int Symbol::getRelocNumber() {
	return this->relocNumber;
}
void Symbol::setRelocNumber(int number) {
	this->relocNumber = number;
}
bool Symbol::getDefined() {
	return this->defined;
}
void Symbol::setDefined(bool value) {
	this->defined = value;
}
bool Symbol::operator==(const Symbol s2) const {
	return this->name == s2.name;
}
bool Symbol::operator!=(const Symbol s2) const {
	return this->name != s2.name;
}