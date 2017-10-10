#include "DirectiveParser.h"
#include "AssemblyException.h"
#include "Constants.h"
#include <sstream>

DirectiveParser::DirectiveParser() {
	this->currentSection = "UND";
	this->sectionByteOffset = 0;
	this->totalByteCount = 0;
	this->numberOfSections = 0;
}

std::string DirectiveParser::trimLine(std::string line) {
	while (iswspace(line[0])) {
		line.erase(0, 1);
	}
	int index = line.find(";");
	if (index != std::string::npos) {
		return line.erase(index, line.size());
	}
	return line;
}

std::string DirectiveParser::getLabelName(std::string line) {
	if (line.find(":") == std::string::npos) {
		return "";
	}
	if (line.find(" ") < line.find(":")) {
		throw new AssemblyException("[" + line + "] : Invalid label name. No whitespaces are allowed.\n");
	}
	return line.substr(0, line.find(":"));
}

std::string DirectiveParser::getInstruction(std::string line) {
	std::string instructionPart = line;
	if (getLabelName(line).length() > 0) {
		instructionPart = line.substr(getLabelName(line).length() + 1);
	}
	while (isspace(instructionPart[0])) {
		instructionPart.erase(0, 1);
	}
	int whitespacePos = instructionPart.find(" ");
	if (whitespacePos == std::string::npos) {
		throw new AssemblyException("[" + line + "] : Invalid instruction declaration. No argument provided for the given instruction.\n");
	}
	return instructionPart.substr(0, whitespacePos);
}

std::string DirectiveParser::getOperands(std::string line) {
	int operandStartIndex = line.find(getInstruction(line)) + getInstruction(line).length();
	int i = 0;
	while (isspace(line[operandStartIndex + i])) {
		i++;
	}
	operandStartIndex += i;
	return line.substr(operandStartIndex);
}

int DirectiveParser::getOpcode(std::string operation) {
	std::string instruction = operation;
	if (instruction == ".skip" || instruction == ".align" || instruction == ".char" || instruction == ".word" || instruction == ".long") {
		return Constants::getMapper()[instruction];
	}
	int opcode = 0;
	if (Constants::getMapper().count(instruction.substr(instruction.length() - 2)) == 1) {
		opcode = Constants::getMapper().at(instruction.substr(instruction.length() - 2));
		if (opcode != 0 && (opcode == Constants::getMapper()["or"] || opcode == Constants::getMapper()["in"])) {
			//Correction for "or" and "in" instructions
			opcode = Constants::getMapper()["al"];
		}
		else {
			instruction.erase(instruction.length() - 2, 2);
		}
	}
	else {
		opcode = Constants::getMapper()["al"];
	}
	if (Constants::getMapper().count(instruction) == 1) {
		opcode |= Constants::getMapper().at(instruction);
		return opcode;
	}
	throw new AssemblyException("Unsupported instruction [" + operation + "]");
}

std::string DirectiveParser::extractBeforeWhitespace(std::string line) {
	std::string returnValue = "";
	int len = line.length();
	for (int i = 0; i < len && !isspace(line[i]); i++) {
		returnValue = returnValue + line[i];
	}
	return returnValue;
}

std::vector<std::string> DirectiveParser::getOperandValues(std::string operands) {
	std::vector<std::string> v;
	std::stringstream stream(operands);
	std::string item;
	while (std::getline(stream, item, ',')) {
		while (iswspace(item[0])) {
			item.erase(0, 1);
		}
		v.push_back(item);
	}
	for (unsigned int i = 0; i < v.size(); i++) {
		std::string operand = v[i];
		for (int j = 0; j < operand.length();) {
			if (iswspace(operand[j])) {
				operand.erase(j, 1);
			}
			else {
				j++;
			}
		}
		v[i] = operand;
	}
	return v;
}

std::vector<std::string> DirectiveParser::getOperandValues(std::string operands, char delim) {
	std::vector<std::string> v;
	std::stringstream stream(operands);
	std::string item;
	while (std::getline(stream, item, delim)) {
		v.push_back(item);
	}
	return v;
}

bool DirectiveParser::isNumber(std::string input) {
	if (input.empty()) return false;

	char * nonDigitChar;
	strtol(input.c_str(), &nonDigitChar, 0);

	return (*nonDigitChar == 0); //If it's not \0 (end of string character) , the input has a non-digit character
}

bool DirectiveParser::isRegister(std::string input) {
	if (input.empty()) return false;

	return (input == "r0" || input == "r1" || input == "r2" || input == "r3" || input == "r4" || input == "r5" || input == "r6" || input == "r7" || input == "r8" || input == "r9" || input == "r10" || input == "r11" || input == "r12" || input == "r13" || input == "r14" || input == "r15" || input == "PC" || input == "pc" || input == "LR" || input == "lr" || input == "SP" || input == "sp" || input == "PSW" || input == "psw");
}

bool DirectiveParser::isPseudoLdcInstruction(std::string input) {
	return input.find("ldc") == 0 && input.find("ldch") == std::string::npos && input.find("ldcl") == std::string::npos;
}

int DirectiveParser::getRegisterNumber(std::string input) {
	if (input[0] == 'r') {
		return std::stoi(input.substr(1));
	}
	if (input == "pc" || input == "PC") {
		return Constants::REG_PC;
	}
	if (input == "lr" || input == "LR") {
		return Constants::REG_LR;
	}
	if (input == "sp" || input == "SP") {
		return Constants::REG_SP;
	}
	if (input == "psw" || input == "PSW") {
		return Constants::REG_PSW;
	}
	return 20;
}

void DirectiveParser::setCurrentSection(std::string sectionName) {
	this->currentSection = sectionName;
	this->sectionByteOffset = 0;
}

std::string DirectiveParser::getCurrentSection() {
	return this->currentSection;
}

void DirectiveParser::incCurrentSectionOffset(int ammount) {
	this->sectionByteOffset += ammount;
}

int DirectiveParser::getCurrentSectionOffset() {
	return this->sectionByteOffset;
}

void DirectiveParser::resetCurrentSectionOffset() {
	this->sectionByteOffset = 0;
}

void DirectiveParser::incTotalByteCount(int ammount) {
	this->totalByteCount += ammount;
}

int DirectiveParser::getTotalByteCount() {
	return this->totalByteCount;
}

void DirectiveParser::incNumberOfSections() {
	this->incNumberOfSections(1);
}

void DirectiveParser::incNumberOfSections(int number) {
	this->numberOfSections += number;
}

int DirectiveParser::getNumberOfSections() {
	return this->numberOfSections;
}