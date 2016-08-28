#include "RelocationList.h"

RelocationList::RelocationList() {
	this->head = nullptr;
}
RelocationList::RelocationList(RelocNode* head) {
	this->head = head;
}
RelocationList::~RelocationList() {
	RelocNode* current = this->head;
	while (current != nullptr) {
		RelocNode* next = current->getNext();
		delete current;
		current = next;
	}
	this->head = nullptr;
}
void RelocationList::addNode(RelocNode* nextNode) {
	if (this->head == nullptr) {
		this->head = nextNode;
		return;
	}
	RelocNode* temp = this->head;
	while (temp->getNext() != nullptr) {
		temp = temp->getNext();
	}
	temp->setNext(nextNode);
}
RelocNode* RelocationList::peek() {
	return this->head;
}
RelocNode* RelocationList::pop() {
	if (this->head == nullptr) {
		return nullptr;
	}
	RelocNode* temp = this->head;
	if (this->head != nullptr) {
		this->head = this->head->getNext();
	}
	temp->setNext(nullptr);
	return temp;
}
RelocNode* RelocationList::findByValue(Symbol s) {
	RelocNode* temp = this->head;
	while (temp != nullptr) {
		if (s == temp->getSymbol()) {
			return temp;
		}
	}
	return temp;
}
RelocNode* RelocationList::deleteNode(RelocNode* node) {
	if (node == nullptr) return nullptr;
	if (node == this->head) {
		this->head = node->getNext();
		return this->head;
	}
	RelocNode* temp = this->head;
	while (temp != nullptr) {
		if (temp->getNext() == node) {
			temp->setNext(node->getNext());
			return temp;
		}
		temp = temp->getNext();
	}
	return nullptr;
}