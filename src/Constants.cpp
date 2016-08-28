#include "Constants.h"

std::string Constants::END_FILE_OPERAND = ".end";
std::string Constants::GLOBAL_DECLARATION = ".global ";
std::string Constants::EXTERNAL_DECLARATION = ".extern ";
std::string Constants::UNDEFINED_SECTION_KEY = "UND";
std::string Constants::DATA_SECTION = ".data";
std::string Constants::TEXT_SECTION = ".text";
std::string Constants::BSS_SECTION = ".bss";

Symbol Constants::INVALID_SYMBOL = Symbol("iNvAlId");

std::string Constants::SYMBOL_TABLE_HEADER = "#SymbolTable\nNum\tIme\t\ts\t\tpom\t\tvid\n";
std::string Constants::MEMORY_HEADER = "#Memory::";
std::string Constants::RELOCATION_HEADER = "#Relocation::";
std::string Constants::RELOCATION_TABLE_HEADER = "offset\ttype\tnum\n";

int Constants::REG_PC = 16;
int Constants::REG_LR = 17;
int Constants::REG_SP = 18;
int Constants::REG_PSW = 19;

std::string Constants::R_386_8 = "R_386_8";
std::string Constants::R_386_16 = "R_386_16";
std::string Constants::R_386_24 = "R_386_24";
std::string Constants::R_386_32 = "R_386_32";
std::string Constants::R_386_PC32 = "R_386_PC32";

std::map<std::string, int> Constants::dictionary = std::map<std::string, int>();

void Constants::initMapper() {
	//Operations
	dictionary["int"] = 0x0 << 24;				//src:4,nu:20
	dictionary["add"] = 0x11 << 24;				//dst:5, 0:1, src:5, nu:13; dst:5, 1:1, imm(18)
	dictionary["sub"] = 0x12 << 24;				//dst:5, 0:1, src:5, nu:13; dst:5, 1:1, imm(18)
	dictionary["mul"] = 0x13 << 24;				//dst:5, 0:1, src:5, nu:13; dst:5, 1:1, imm(18)
	dictionary["div"] = 0x14 << 24;				//dst:5, 0:1, src:5, nu:13; dst:5, 1:1, imm(18)
	dictionary["cmp"] = 0x15 << 24;				//dst:5, 0:1, src:5, nu:13; dst:5, 1:1, imm(18)
	dictionary["and"] = 0x16 << 24;				//dst:5, src:5, nu:14
	dictionary["or"] = 0x17 << 24;				//dst:5, src:5, nu:14
	dictionary["not"] = 0x18 << 24;				//dst:5, src:5, nu:14
	dictionary["test"] = 0x19 << 24;			//dst:5, src:5, nu:14
	dictionary["ldr"] = 0xa << 24 | 1 << 10;	//a:5, r:5, f:3, l/s:1, imm:10
	dictionary["str"] = 0xa << 24 | 0 << 10;	//a:5, r:5, f:3, l/s:1, imm:10
	dictionary["call"] = 0xc << 24;				//dst:5, imm:19
	dictionary["in"] = 0xd << 24 | 1 << 15;		//dst:4, src:4, i/o:1, nu:15
	dictionary["out"] = 0xd << 24 | 0 << 15;	//dst:4, src:4, i/o:1, nu:15
	dictionary["mov"] = 0x1e << 24;				//dst:5,src:5,imm:5, l/r:1, nu:8
	dictionary["shr"] = 0x1e << 24 | 1 << 8;	//dst:5,src:5,imm:5, l/r:1, nu:8
	dictionary["shl"] = 0x1e << 24 | 0 << 8;	//dst:5,src:5,imm:5, l/r:1, nu:8
	dictionary["ldch"] = 0xf << 24 | 1 << 19;	//dst:4,h/l:1, nu:3,c:16
	dictionary["ldcl"] = 0xf << 24 | 0 << 19;	//dst:4,h/l:1, nu:3,c:16

	//Conditions
	dictionary["eq"] = 0x0 << 29;
	dictionary["ne"] = 0x1 << 29;
	dictionary["gt"] = 0x2 << 29;
	dictionary["ge"] = 0x3 << 29;
	dictionary["lt"] = 0x4 << 29;
	dictionary["le"] = 0x5 << 29;
	dictionary["al"] = 0x7 << 29;

	//Width and skip directives
	dictionary[".char"] = 1;
	dictionary[".word"] = 2;
	dictionary[".long"] = 4;
	dictionary[".align"] = 0;
	dictionary[".skip"] = 0;
}

std::map<std::string, int> Constants::getMapper() {
	return dictionary;
}