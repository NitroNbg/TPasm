#include "MemoryList.h"

MemoryList::MemoryList() {
	this->head = nullptr;
}
MemoryList::MemoryList(Memory* head) {
	this->head = head;
}
MemoryList::~MemoryList() {
	Memory* current = this->head;
	while (current != nullptr) {
		Memory* next = current->getNext();
		delete current;
		current = next;
	}
	this->head = nullptr;
}
void MemoryList::addNode(Memory* nextNode) {
	if (this->head == nullptr) {
		this->head = nextNode;
		return;
	}
	Memory* temp = this->head;
	while (temp->getNext() != nullptr) {
		temp = temp->getNext();
	}
	temp->setNext(nextNode);
}
Memory* MemoryList::peek() {
	return this->head;
}
Memory* MemoryList::pop() {
	if (this->head == nullptr) {
		return nullptr;
	}
	Memory* temp = this->head;
	if (this->head != nullptr) {
		this->head = this->head->getNext();
	}
	temp->setNext(nullptr);
	return temp;
}