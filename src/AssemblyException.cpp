#include "AssemblyException.h"
#include <iostream>
#include <exception>
#include <stdexcept>

AssemblyException::AssemblyException(std::string message) : std::runtime_error(message) {
	this->message = message;
}

AssemblyException::~AssemblyException() throw() {}

std::string AssemblyException::getMessage() {
	return this->message;
}