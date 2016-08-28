#include "Memory.h"
#include "AssemblyException.h"
#include <string>

Memory::Memory() : Memory("") {}

Memory::Memory(std::string name) {
	this->bytes = std::vector<int>();
	this->section = name;
	this->next = nullptr;
}

int Memory::size() {
	return this->bytes.size();
}

int Memory::get(int location) {
	if (location < 0 || location >= size()) {
		throw new AssemblyException("Nepripadajuća adresa " + std::to_string(location) + " je zatražena. Memorija prevedenog programa je veličine " + std::to_string(size()));
	}
	return this->bytes[location];
}

void Memory::set(int value) {
	this->bytes.push_back(value & 255);
	while (value >= 256) {
		this->bytes.push_back(value & 255);
		value = value >> 8;
	}
}

void Memory::set(int value, int location) {
	if (location < 0 || location >= size()) {
		throw new AssemblyException("Nepripadajuća adresa " + std::to_string(location) + " je zatražena. Memorija prevedenog programa je veličine " + std::to_string(size()));
	}
	this->bytes[location] = value;
}

void Memory::setWithFixedSize(int value, int size) {
	int memValue = value % 256;
	for (int i = 0; i < size; i++) {
		this->bytes.push_back(memValue);
		value = value >> 8;
		memValue = value % 256;
	}
	
}

void Memory::setWithFixedSize(int value, int location, int s) {
	if (location < 0 || location >= this->size()) {
		throw new AssemblyException("Nepripadajuća adresa " + std::to_string(location) + " je zatražena. Memorija prevedenog programa je veličine " + std::to_string(size()));
	}
	int memValue = value % 256;
	for (int i = 0; i < s; i++) {
		this->bytes[location + i] = memValue;
		value = value >> 8;
		memValue = value % 256;
	}
}

void Memory::setWithFixedSizeInstruction(int value, int size) {
	this->bytes.push_back((value & 0xff000000) >> 24);
	this->bytes.push_back((value & 0x00ff0000) >> 16);
	this->bytes.push_back((value & 0x0000ff00) >> 8);
	this->bytes.push_back(value & 0x000000ff);
}

void Memory::setWithFixedSizeInstruction(int value, int location, int s) {
	if (location < 0 || location >= this->size()) {
		throw new AssemblyException("Nepripadajuća adresa " + std::to_string(location) + " je zatražena. Memorija prevedenog programa je veličine " + std::to_string(size()));
	}
	int memValue = value % 256;
	for (int i = 0; i < s; i++) {
		this->bytes[location + 3 - i] = memValue;
		value = value >> 8;
		memValue = value % 256;
	}
}

void Memory::set(unsigned int value) {
	this->bytes.push_back(value & 255);
	while (value >= 256) {
		this->bytes.push_back(value & 255);
		value = value >> 8;
	}
}

void Memory::set(unsigned int value, int location) {
	if (location < 0 || location >= size()) {
		throw new AssemblyException("Nepripadajuća adresa " + std::to_string(location) + " je zatražena. Memorija prevedenog programa je veličine " + std::to_string(size()));
	}
	this->bytes[location] = value;
}

void Memory::setWithFixedSize(unsigned int value, int size) {
	int memValue = value % 256;
	for (int i = 0; i < size; i++) {
		this->bytes.push_back(memValue);
		value = value >> 8;
		memValue = value % 256;
	}

}

void Memory::setWithFixedSize(unsigned int value, int location, int s) {
	if (location < 0 || location >= this->size()) {
		throw new AssemblyException("Nepripadajuća adresa " + std::to_string(location) + " je zatražena. Memorija prevedenog programa je veličine " + std::to_string(size()));
	}
	int memValue = value % 256;
	for (int i = 0; i < s; i++) {
		this->bytes[location + i] = memValue;
		value = value >> 8;
		memValue = value % 256;
	}
}

void Memory::setWithFixedSizeInstruction(unsigned int value, int size) {
	this->bytes.push_back((value & 0xff000000) >> 24);
	this->bytes.push_back((value & 0x00ff0000) >> 16);
	this->bytes.push_back((value & 0x0000ff00) >> 8);
	this->bytes.push_back(value & 0x000000ff);
}

void Memory::setWithFixedSizeInstruction(unsigned int value, int location, int s) {
	if (location < 0 || location >= this->size()) {
		throw new AssemblyException("Nepripadajuća adresa " + std::to_string(location) + " je zatražena. Memorija prevedenog programa je veličine " + std::to_string(size()));
	}
	int memValue = value % 256;
	for (int i = 0; i < s; i++) {
		this->bytes[location + 3 - i] = memValue;
		value = value >> 8;
		memValue = value % 256;
	}
}

std::string Memory::getSectionName() {
	return this->section;
}

void Memory::setSectionName(std::string name) {
	this->section = name;
}

Memory* Memory::getNext() {
	return this->next;
}

void Memory::setNext(Memory* node) {
	this->next = node;
}