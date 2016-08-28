#ifndef d_parser
#define d_parser

#include <string>
#include <vector>

class DirectiveParser {
private:
	int totalByteCount;
	int sectionByteOffset;
	int numberOfSections;
	std::string currentSection;
public:
	DirectiveParser();
	~DirectiveParser();
	std::string getLabelName(std::string line);
	std::string getInstruction(std::string line);
	std::string getOperands(std::string line);
	std::string trimLine(std::string line);
	int getOpcode(std::string operation);
	std::string extractBeforeWhitespace(std::string line);
	std::vector<std::string> getOperandValues(std::string operands);
	std::vector<std::string> getOperandValues(std::string operands, char delim);
	bool isNumber(std::string input);
	bool isRegister(std::string input);
	bool isPseudoLdcInstruction(std::string input);
	int getRegisterNumber(std::string input);
	void setCurrentSection(std::string section);
	std::string getCurrentSection();
	void incCurrentSectionOffset(int ammount);
	int getCurrentSectionOffset();
	void resetCurrentSectionOffset();
	void incTotalByteCount(int ammount);
	int getTotalByteCount();
	void incNumberOfSections();
	void incNumberOfSections(int number);
	int getNumberOfSections();
};

#endif