#ifndef muhdefined
#define muhdefined

#include "SymbolTable.h"
#include "DirectiveParser.h"
#include "MemoryList.h"
#include "RelocationList.h"
#include <string>
#include <fstream>

class MainAssemblyUnit {
private:
	std::ifstream* input;
	std::ofstream* output;
	SymbolTable* symtab;
	DirectiveParser* parser;
	MemoryList* memory;
	RelocationList* relocationList;
public:
	MainAssemblyUnit();
	MainAssemblyUnit(std::string input, std::string output);
	void processCompleted(bool success, std::string message);
	std::string readLine();
	void setInput(std::string input);
	void setOutput(std::string filename);
	void processFile();
	void processFile(std::string filename);
	void passOne();
	void passTwo();
	void printToOutput();
	std::string outputToHexFormatLimitless(int value);
	std::string outputToHexFormat(int value);
	std::string integerToChar(int value);
	int getSectionNumberForSymbol(Symbol sym);
};

#endif