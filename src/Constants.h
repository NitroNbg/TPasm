#ifndef constants
#define constants

#include "Symbol.h"
#include <string>
#include <map>

class Constants {
private:
	static std::map<std::string, int> dictionary;
public:
	static std::string END_FILE_OPERAND;
	static std::string GLOBAL_DECLARATION;
	static std::string EXTERNAL_DECLARATION;
	static std::string UNDEFINED_SECTION_KEY;
	static std::string DATA_SECTION;
	static std::string BSS_SECTION;
	static std::string TEXT_SECTION;
	static int REG_PC;
	static int REG_LR;
	static int REG_SP;
	static int REG_PSW;
	static std::string R_386_8;
	static std::string R_386_16;
	static std::string R_386_24;
	static std::string R_386_32;
	static std::string R_386_PC32;
	static void initMapper();
	static std::map<std::string, int> getMapper();
	static Symbol INVALID_SYMBOL;

	//OUTPUT CONSTANS
	static std::string SYMBOL_TABLE_HEADER;
	static std::string MEMORY_HEADER;
	static std::string RELOCATION_HEADER;
	static std::string RELOCATION_TABLE_HEADER;
};

#endif