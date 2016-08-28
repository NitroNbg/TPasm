#include "MainAssemblyUnit.h"
#include "AssemblyException.h"
#include "Constants.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>

MainAssemblyUnit::MainAssemblyUnit() {

}

MainAssemblyUnit::MainAssemblyUnit(std::string input, std::string output) {
	this->input = new std::ifstream(input, std::ios::binary);
	this->output = new std::ofstream(output, std::ofstream::trunc);
}

void MainAssemblyUnit::setInput(std::string input) {
	this->input = new std::ifstream(input, std::ios::binary);
}

void MainAssemblyUnit::setOutput(std::string filename) {
	this->output = new std::ofstream(filename, std::ofstream::trunc);
}

std::string MainAssemblyUnit::readLine() {
	std::string line;
	std::getline(*(this->input), line);
	return line;
}

void MainAssemblyUnit::processFile(std::string filename) {
	this->setInput(filename);
	if (this->input->good()) {
		this->setOutput(filename.append(".etf"));
		std::cout << "Program će započeti sa prevođenjem zadate datoteke " << filename << std::endl;
		processFile();
	}
	else {
		std::cout << "Nije moguće čitati iz datotek " << filename << ". Proverite da li ste pravilno uneli ime." << std::endl;
	}
	
}

void MainAssemblyUnit::processFile() {
	std::cout << "Program će započeti sa prevođenjem zadate datoteke" << std::endl;
	try {
		this->symtab = new SymbolTable();
		this->parser = new DirectiveParser();
		this->memory = new MemoryList();
		this->relocationList = new RelocationList();
		this->passOne();
		std::cout << "Prvi prolaz je uspešno završen" << std::endl;
		this->passTwo();
		std::cout << "Drugi prolaz je uspešno završen" << std::endl;
		this->printToOutput();
		std::cout << "Upisivanje u datoteku je završeno" << std::endl;
		processCompleted(true, "");
	}
	catch (AssemblyException* e) {
		processCompleted(false, e->getMessage());
	}
	this->input->close();
	this->output->flush();
	this->output->close();
}

void MainAssemblyUnit::passOne() {
	int symbolNumber = 0;
	Memory* currentMemoryUnit = nullptr;
	std::string line = readLine();
	line = this->parser->trimLine(line);
	while (line.find(Constants::END_FILE_OPERAND) != 0) {
		std::string label = this->parser->getLabelName(line);
		if (label.compare("") != 0) {
			Symbol sym = Constants::INVALID_SYMBOL;
			if (this->symtab->contains(label)) {
				sym = this->symtab->get(label);
				if (sym.getDefined()) {
					throw new AssemblyException("Promenljiva [" + label + "] je prethodno već definisana");
				}
				sym.setDefined(true);
				sym.setSectionName(this->parser->getCurrentSection());
				sym.setOffset(this->parser->getCurrentSectionOffset());
				if (sym.getNumber() == -1){
					sym.setNumber(++symbolNumber);
				}
			}
			else {
				sym = Symbol(label, this->parser->getCurrentSection());
				sym.setDefined(true);
				sym.setOffset(this->parser->getCurrentSectionOffset());
				sym.setNumber(++symbolNumber);
			}
			this->symtab->put(label, sym);
			line.erase(0, label.length() + 1);
			line = this->parser->trimLine(line);
		}
		if (line.find(Constants::GLOBAL_DECLARATION) == 0) {
			//Line contains a global declaration(s) directive
			std::string operands = line.substr(Constants::GLOBAL_DECLARATION.length());
			std::vector<std::string> operandValues = this->parser->getOperandValues(operands);
			int numberOfSymbols = operandValues.size();
			for (int i = 0; i < numberOfSymbols; i++) {
				//Declare each symbol in SymbolTable
				if (this->symtab->contains(operandValues[i])) {
					throw new AssemblyException("Promenljiva [" + operandValues[i] + "] je prethodno već deklarisana");
				}
				Symbol* s = new Symbol(operandValues[i]);
				s->setVisibility(global);
				this->symtab->put(operandValues[i], *s);
			}
		}
		else if (line.find(Constants::EXTERNAL_DECLARATION) == 0) {
			//Line contains an external declaration(s) directive
			std::string operands = line.substr(Constants::EXTERNAL_DECLARATION.length());
			std::vector<std::string> operandValues = this->parser->getOperandValues(operands);
			int numberOfSymbols = operandValues.size();
			for (int i = 0; i < numberOfSymbols; i++) {
				//Declare each symbol in SymbolTable
				if (this->symtab->contains(operandValues[i])) {
					throw new AssemblyException("Promenljiva [" + operandValues[i] + "] je prethodno već deklarisana");
				}
				Symbol* s = new Symbol(operandValues[i]);
				s->setVisibility(global);
				s->setNumber(++symbolNumber);
				s->setOffset(0);
				this->symtab->put(operandValues[i], *s);
			}
		}
		else if (line.find(".") == 0) {
			//Line contains a new section declaration or an assembly directive
			std::string extract = this->parser->extractBeforeWhitespace(line);
			if (line.find(Constants::DATA_SECTION) == 0 || line.find(Constants::TEXT_SECTION) == 0 || line.find(Constants::BSS_SECTION) == 0) {
				//It is a new section declaration
				if (this->symtab->contains(extract)) {
					if (this->symtab->get(extract).getDefined()) {
						throw new AssemblyException("Više puta je definisana sekcija [" + extract + "]");
					}
				}
				Symbol section = Symbol(extract, extract);
				section.setOffset(0);
				section.setDefined(true);
				this->parser->incNumberOfSections();
				section.setNumber(this->parser->getNumberOfSections());
				section.setRelocNumber(this->parser->getNumberOfSections());
				this->symtab->put(extract, section);
				this->parser->setCurrentSection(extract);
				this->parser->resetCurrentSectionOffset();
				currentMemoryUnit = new Memory(extract);
				this->memory->addNode(currentMemoryUnit);
			}
			else {
				//It is an assembly directive
				int directiveLength = this->parser->getOpcode(extract);
				std::string operands = this->parser->getOperands(line);
				std::vector<std::string> operandValues = this->parser->getOperandValues(operands);
				int numberOfArguments = operandValues.size();
				if (numberOfArguments == 0) {
					throw new AssemblyException("Naredba [" + extract + "] se ne može pozvati bez barem jednog argumenta.");
				}
				if (directiveLength == 0) {
					//It is a ".skip" or ".align" directive
					if (extract == ".align") {
						int alignWidth = std::stoi(operandValues[0], nullptr, 0);
						if (alignWidth != 2 && alignWidth != 4 && alignWidth != 8 && alignWidth != 16 && alignWidth != 32 && alignWidth != 64 && alignWidth != 128) {
							throw new AssemblyException("Naredba [" + extract + "] prihvata samo sledeće argumente: 2, 4, 8, 16, 32, 64, 128");
						}
						if (this->parser->getCurrentSectionOffset() % alignWidth != 0) {
							int appendValue = 0;
							if (operandValues.size() > 1) {
								appendValue = std::stoi(operandValues[1], nullptr, 0);
							}
							int paddingAmmount = alignWidth - this->parser->getCurrentSectionOffset() % alignWidth;
							for (int i = 0; i < paddingAmmount; i++) {
								currentMemoryUnit->set(appendValue);
							}
							this->parser->incCurrentSectionOffset(paddingAmmount);
						}
					}
					else if (extract == ".skip") {
						int count = std::stoi(operandValues[0], nullptr, 0);
						if (count < 0) {
							throw new AssemblyException("Naredba [" + extract + "] prima samo cele brojeve veće od nule");
						}
						int appendValue = 0;
						if (operandValues.size() > 1) {
							appendValue = std::stoi(operandValues[1], nullptr, 0);

						}
						for (int i = 0; i < count; i++) {
							currentMemoryUnit->set(appendValue);
						}
						this->parser->incCurrentSectionOffset(count);
					}
				}
				else {
					//It is a memory allocation directive
					int varSize = 0;
					if (extract == ".long") {
						varSize = 4;
					}
					else if (extract == ".word") {
						varSize = 2;
					}
					else if (extract == ".char") {
						varSize = 1;
					}
					for (unsigned int i = 0; i < operandValues.size(); i++) {
						if (this->parser->isNumber(operandValues[i])) {
							//It is a literal value
							unsigned long value = std::stoul(operandValues[i], nullptr, 0);
							if (value >= std::pow(256, varSize)) {
								throw new AssemblyException("Vrednost [" + std::to_string(value) + "] ne može da stane u " + std::to_string(varSize) + " bajta.");
							}
							unsigned int mem = value & 0xffffffff;
							currentMemoryUnit->setWithFixedSize(mem, varSize);
						}
						else {
							//It is a label
							std::string type = Constants::R_386_32;
							if (varSize == 1) {
								type = Constants::R_386_8;
							}
							else if (varSize == 2) {
								type = Constants::R_386_16;
							}
							if (!this->symtab->contains(operandValues[i])) {
								if (operandValues[i].find(".") == 0) {
									//It is a reference to an unknown section
									this->symtab->put(operandValues[i], Symbol(operandValues[i], operandValues[i]));
								}
								else {
									this->symtab->put(operandValues[i], Symbol(operandValues[i], "UND"));
								}
							}
							Symbol sym = this->symtab->get(operandValues[i]);
							this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), type, sym.getNumber(), sym));
							currentMemoryUnit->setWithFixedSize(0, varSize); //Should be rewritten with symbol (label) value afterwards
						}
					}
					this->parser->incCurrentSectionOffset(varSize * operandValues.size());
				}
			}
		}
		else if (!line.empty()) {
			std::string instruction = this->parser->getInstruction(line);
			if (this->parser->isPseudoLdcInstruction(instruction)) {
				//Write consecutive "ldch" and "ldcl" instructions with appropriate "c" arguments
				std::string operands = this->parser->getOperands(line);
				std::vector<std::string> operandValues = this->parser->getOperandValues(operands);
				if (operandValues.size() != 2) {
					throw new AssemblyException("Pseudo naredba [ldc] prihvata samo dva argumenta.");
				}
				if (!this->parser->isRegister(operandValues[0])) {
					throw new AssemblyException("Prvi argument pseudo naredbe [ldc] mora biti CPU registar.");
				}
				if (!this->parser->isNumber(operandValues[1])) {
					throw new AssemblyException("Drugi argument pseudo naredbe [ldc] mora biti ceo broj, u decimalnom ili heksadecimalnom zapisu.");
				}
				int reg = this->parser->getRegisterNumber(operandValues[0]);
				int argument = std::stoi(operandValues[1], nullptr, 0);
				unsigned int opcode = this->parser->getOpcode("ldch");
				opcode |= reg << 20;
				opcode |= argument >> 16;
				currentMemoryUnit->setWithFixedSizeInstruction(opcode, 4);
				opcode = this->parser->getOpcode("ldcl");
				opcode |= reg << 20;
				opcode |= (argument & 0xffff);
				currentMemoryUnit->setWithFixedSizeInstruction(opcode, 4);
				this->parser->incTotalByteCount(8);
				this->parser->incCurrentSectionOffset(8);
			}
			else {
				unsigned int opcode = this->parser->getOpcode(instruction);
				std::string operands = this->parser->getOperands(line);
				std::vector<std::string> operandValues = this->parser->getOperandValues(operands);
				int instructionBytes = (opcode & (15 << 24)) >> 24;
				if (instructionBytes == 0) {
					//"int" instruction format: src:4, nu:20 
					if (operandValues.size() != 1) {
						throw new AssemblyException("Naredba [int] prihvata samo jedan argument.");
					}
					if (this->parser->isNumber(operandValues[0])) {
						int arg = std::stoi(operandValues[0], nullptr, 0);
						if (arg > 15) {
							throw new AssemblyException("Računar sadrži 16 ulaza u IVT-u. Ne sme se pozvati prekidna rutina na ulazu " + operandValues[0]);
						}
						opcode |= arg << 20;
					}
					else {
						//The argument is label
						if (!this->symtab->contains(operandValues[0])) {
							if (operandValues[0].find(".") == 0) {
								//It is a reference to an unknown section
								this->symtab->put(operandValues[0], Symbol(operandValues[0], operandValues[0]));
							}
							else {
								this->symtab->put(operandValues[0], Symbol(operandValues[0], "UND"));
							}
							Symbol sym = this->symtab->get(operandValues[0]);
							this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_8, sym.getNumber(), sym));
							currentMemoryUnit->setWithFixedSize(0, 1); //Should be rewritten with symbol (label) value afterwards
						}
						/*else if (this->symtab->get(operandValues[0]).getDefined()) {
							int arg = this->symtab->get(operandValues[0]).getOffset();
							if (arg > 15) {
								throw new AssemblyException("Računar sadrži 16 ulaza u IVT-u. Ne sme se pozvati prekidna rutina na ulazu " + operandValues[0]);
							}
							opcode |= arg << 20;
						}
						else {*/
							Symbol sym = this->symtab->get(operandValues[0]);
							this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_8, sym.getNumber(), sym));
						//}
					}
				}
				else if (instructionBytes > 0 && instructionBytes < 6) {
					//"add", "sub", "mul", "div" and "cmp" instructions format: dst:5, 0:1, src:5*1, nu:13; dst:5, 1:1, imm(18)
					if (operandValues.size() != 2) {
						throw new AssemblyException("Naredba [" + instruction + "] prihvata samo dva argumenta.");
					}
					if (!this->parser->isRegister(operandValues[0])) {
						throw new AssemblyException("Prvi argument naredbe [" + instruction + "] mora biti CPU registar.");
					}
					int reg = this->parser->getRegisterNumber(operandValues[0]);
					opcode |= (reg & 31) << 19;
					if (this->parser->isNumber(operandValues[1])) {
						//instruction format is dst:5, 1:1, imm(18)
						opcode |= 1 << 18;
						int immedValue = std::stoi(operandValues[1], nullptr, 0);
						if (immedValue > 262143) {
							throw new AssemblyException("Najveća vrednost za immed argument u naredbi [" + instruction + "] je 262143 (2 ^ 18 - 1) .");
						}
						opcode |= (immedValue & 262143);
					}
					else if (this->parser->isRegister(operandValues[1])) {
						//instruction format is dst:5, 0:1, src:5, nu:13
						int regValue = this->parser->getRegisterNumber(operandValues[1]);
						opcode |= (reg & 31) << 13;
					}
					else {
						//The argument is a label
						//instruction format is dst:5, 1:1, imm(18)
						opcode |= 1 << 18;
						if (!this->symtab->contains(operandValues[1])) {
							if (operandValues[1].find(".") == 0) {
								//It is a reference to an unknown section
								this->symtab->put(operandValues[1], Symbol(operandValues[1], operandValues[1]));
							}
							else {
								this->symtab->put(operandValues[1], Symbol(operandValues[1], "UND"));
							}
							Symbol sym = this->symtab->get(operandValues[1]);
							this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_24, sym.getNumber(), sym));
							currentMemoryUnit->setWithFixedSize(0, 1); //Should be rewritten with symbol (label) value afterwards
						}
						/*else if (this->symtab->get(operandValues[1]).getDefined()) {
							int immedValue = this->symtab->get(operandValues[1]).getOffset();
							if (immedValue > 262143) {
								throw new AssemblyException("Najveća vrednost za immed argument u naredbi [" + instruction + "] je 262143 (2 ^ 18 - 1) .");
							}
							opcode |= (immedValue & 262143);
						}
						else {*/
							Symbol sym = this->symtab->get(operandValues[1]);
							this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_24, sym.getNumber(), sym));
						//}
					}
				}
				else if (instructionBytes >= 6 && instructionBytes < 10) {
					//"and", "or", "not", "test" instructions format: dst:5, src:5, nu:14
					if (operandValues.size() != 2) {
						throw new AssemblyException("Naredba [" + instruction + "] prihvata samo dva argumenta.");
					}
					if (!this->parser->isRegister(operandValues[0]) || !this->parser->isRegister(operandValues[1])) {
						throw new AssemblyException("Oba argumenta naredbe [" + instruction + "] moraju biti CPU registri.");
					}
					int dstReg = this->parser->getRegisterNumber(operandValues[0]);
					int srcReg = this->parser->getRegisterNumber(operandValues[1]);
					if (dstReg == Constants::REG_PC || dstReg == Constants::REG_LR || dstReg == Constants::REG_PSW || srcReg == Constants::REG_PC || srcReg == Constants::REG_LR || srcReg == Constants::REG_PSW) {
						throw new AssemblyException("PC, LR i PSW registri nisu ispravni argumenti za naredbu [" + instruction + "] .");
					}
					opcode |= (dstReg & 31) << 19;
					opcode |= (srcReg & 31) << 14;
				}
				else if (instructionBytes == 10) {
					//"ldr", "str" instructions format: a:5, r:5, f:3, l/s:1, imm:10
					int addrReg = 0;
					int destReg = 0;
					int regFluct = 0;
					int immedOffset = 0;
					if (operandValues.size() != 2 && operandValues.size() != 4) {
						throw new AssemblyException("Naredba [" + instruction + "] prihvata samo dva ili četiri argumenta.");
					}
					if (!this->parser->isRegister(operandValues[0]) || !this->parser->isRegister(operandValues[1])) {
						throw new AssemblyException("Argumenti u pozivu naredbe [" + instruction + "] sa dva argumenta moraju biti CPU registri.");
					}
					addrReg = this->parser->getRegisterNumber(operandValues[0]);
					if (addrReg == Constants::REG_PSW) {
						throw new AssemblyException("Registar PSW ne može biti adresni registar za naredbu [" + instruction + "] .");
					}
					destReg = this->parser->getRegisterNumber(operandValues[1]);
					if (operandValues.size() == 4) {
						if (this->parser->isNumber(operandValues[2])) {
							regFluct = std::stoi(operandValues[2], nullptr, 0);
							if (regFluct > 7) {
								throw new AssemblyException("Argument f naredbe [" + instruction + "] mora imati celobrojnu vrednost od nula do sedam.");
							}
							if (addrReg == Constants::REG_PC && regFluct != 0) {
								throw new AssemblyException("Argument f naredbe [" + instruction + "] mora imati vrednost nula kada se PC koristi kao adresni registar");
							}
						}
						else {
							throw new AssemblyException("Argument f naredbe [" + instruction + "] mora imati celobrojnu vrednost od nula do sedam.");
						}
						if (this->parser->isNumber(operandValues[3])) {
							immedOffset = std::stoi(operandValues[3], nullptr, 0);
							if (immedOffset > 1023) {
								throw new AssemblyException("Argument imm naredbe [" + instruction + "] mora imati celobrojnu vrednost u opsegu [0, 1023] .");
							}
						}
						else {
							//immedOffset argument is a label
							if (!this->symtab->contains(operandValues[3])) {
								if (operandValues[3].find(".") == 0) {
									//It is a reference to an unknown section
									this->symtab->put(operandValues[3], Symbol(operandValues[3], operandValues[3]));
								}
								else {
									this->symtab->put(operandValues[3], Symbol(operandValues[3], "UND"));
								}
								Symbol sym = this->symtab->get(operandValues[3]);
								this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_16, sym.getNumber(), sym));
							}
							/*else if (this->symtab->get(operandValues[3]).getDefined()) {
								immedOffset = this->symtab->get(operandValues[3]).getOffset();
								if (immedOffset > 1023) {
									throw new AssemblyException("Argument imm naredbe [" + instruction + "] mora imati celobrojnu vrednost u opsegu [0, 1023] .");
								}
							}
							else {*/
								Symbol sym = this->symtab->get(operandValues[3]);
								this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_16, sym.getNumber(), sym));
							//}
						}
					}
					opcode |= (addrReg & 31) << 19;
					opcode |= (destReg & 31) << 14;
					opcode |= (regFluct & 7) < 11;
					opcode |= (immedOffset & 1023);
				}
				else if (instructionBytes == 12) {
					//"call" instruction format: dst:5, imm:19
					if (operandValues.size() <= 0 || operandValues.size() > 2) {
						throw new AssemblyException("Naredba [" + instruction + "] prihvata samo jedan ili dva argumenta.");
					}
					int dstReg = 0;
					int immedValue = 0;
					if (this->parser->isRegister(operandValues[0])) {
						dstReg = this->parser->getRegisterNumber(operandValues[0]);
					}
					if (operandValues.size() == 2) {
						if (this->parser->isNumber(operandValues[1])) {
							immedValue = std::stoi(operandValues[1], nullptr, 0);
							if (immedValue > 524287) {
								throw new AssemblyException("Argument immed naredbe [" + instruction + "] mora imati celobrojnu vrednost u opsegu [0, 524287] .");
							}
						}
						else {
							//immedValue argument is a label
							if (!this->symtab->contains(operandValues[1])) {
								if (operandValues[1].find(".") == 0) {
									//It is a reference to an unknown section
									this->symtab->put(operandValues[1], Symbol(operandValues[1], operandValues[1]));
								}
								else {
									this->symtab->put(operandValues[1], Symbol(operandValues[1], "UND"));
								}
								Symbol sym = this->symtab->get(operandValues[1]);
								this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_24, sym.getNumber(), sym));
								currentMemoryUnit->setWithFixedSize(0, 1); //Should be rewritten with symbol (label) value afterwards
							}
							/*else if (this->symtab->get(operandValues[1]).getDefined()) {
								immedValue = this->symtab->get(operandValues[1]).getOffset();
								if (immedValue > 524287) {
									throw new AssemblyException("Argument immed naredbe [" + instruction + "] mora imati celobrojnu vrednost u opsegu [0, 524287] .");
								}
							}
							else {*/
								Symbol sym = this->symtab->get(operandValues[1]);
								this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_24, sym.getNumber(), sym));
							//}
						}
					}
					opcode |= (dstReg & 31) << 19;
					opcode |= immedValue & 524287;
				}
				else if (instructionBytes == 13) {
					//"in", "out" instructions format: dst:4, src:4, i/o:1, nu:15
					if (operandValues.size() != 2) {
						throw new AssemblyException("Naredba [" + instruction + "] prihvata samo dva argumenta.");
					}
					int dstReg = 0;
					int srcReg = 0;
					if (!this->parser->isRegister(operandValues[0]) || !this->parser->isRegister(operandValues[1])) {
						throw new AssemblyException("Oba argumenta naredbe [" + instruction + "] moraju biti opštenamenski registi.");
					}
					dstReg = this->parser->getRegisterNumber(operandValues[0]);
					srcReg = this->parser->getRegisterNumber(operandValues[1]);
					if (dstReg > 15 || srcReg > 15) {
						throw new AssemblyException("Oba argumenta naredbe [" + instruction + "] moraju biti opštenamenski registi.");
					}
					opcode |= (dstReg & 15) << 20;
					opcode |= (srcReg & 15) << 16;
				}
				else if (instructionBytes == 14) {
					//"mov", "shr", "shl" instructions format: dst:5, src:5, imm:5, l/r: 1, nu:8
					if (operandValues.size() != 2 && operandValues.size() != 3) {
						throw new AssemblyException("Naredba [" + instruction + "] prihvata samo dva ili tri argumenta.");
					}
					int dstReg = 0;
					int srcReg = 0;
					int immedValue = 0;
					if (!this->parser->isRegister(operandValues[0]) || !this->parser->isRegister(operandValues[1])) {
						throw new AssemblyException("Oba argumenta naredbe [" + instruction + "] moraju biti CPU registri.");
					}
					dstReg = this->parser->getRegisterNumber(operandValues[0]);
					srcReg = this->parser->getRegisterNumber(operandValues[1]);
					if (operandValues.size() == 3) {
						if (this->parser->isNumber(operandValues[2])) {
							immedValue = std::stoi(operandValues[2], nullptr, 0);
							if (immedValue > 31) {
								throw new AssemblyException("Argument immed naredbe [" + instruction + "] mora imati celobrojnu vrednost u opsegu [0, 31] .");
							}
						}
						else {
							//immedValue argument is a label
							if (!this->symtab->contains(operandValues[2])) {
								if (operandValues[2].find(".") == 0) {
									//It is a reference to an unknown section
									this->symtab->put(operandValues[2], Symbol(operandValues[2], operandValues[0]));
								}
								else {
									this->symtab->put(operandValues[2], Symbol(operandValues[2], "UND"));
								}
								Symbol sym = this->symtab->get(operandValues[2]);
								this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_8, sym.getNumber(), sym));
								currentMemoryUnit->setWithFixedSize(0, 1); //Should be rewritten with symbol (label) value afterwards
							}
							/*else if (this->symtab->get(operandValues[2]).getDefined()) {
								immedValue = this->symtab->get(operandValues[2]).getOffset();
								if (immedValue > 31) {
									throw new AssemblyException("Argument immed naredbe [" + instruction + "] mora imati celobrojnu vrednost u opsegu [0, 31] .");
								}
							}
							else {*/
								Symbol sym = this->symtab->get(operandValues[2]);
								this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_8, sym.getNumber(), sym));
							//}
						}
					}
					opcode |= (dstReg & 31) << 19;
					opcode |= (srcReg & 31) << 14;
					opcode |= (immedValue & 31) << 9;
				}
				else if (instructionBytes == 15) {
					//"ldch", "ldcl" instructions format: dst:4, h/l:1, nu:3, c:16
					if (operandValues.size() != 2) {
						throw new AssemblyException("Naredba [" + instruction + "] prihvata samo dva argumenta.");
					}
					if (!this->parser->isRegister(operandValues[0])) {
						throw new AssemblyException("Prvi argument naredbe [" + instruction + "] mora biti opštenamenski registar.");
					}
					int dstReg = this->parser->getRegisterNumber(operandValues[0]);
					int c = 0;
					if (dstReg > 15) {
						throw new AssemblyException("Prvi argument naredbe [" + instruction + "] mora biti opštenamenski registar.");
					}
					if (this->parser->isNumber(operandValues[1])) {
						c = std::stoi(operandValues[1], nullptr, 0);
						if (c > 65535) {
							throw new AssemblyException("Argument c naredbe [" + instruction + "] mora imati celobrojnu vrednost u opsegu [0, 65535] .");
						}
					}
					else {
						//c argument is a label
						if (!this->symtab->contains(operandValues[1])) {
							if (operandValues[1].find(".") == 0) {
								//It is a reference to an unknown section
								this->symtab->put(operandValues[1], Symbol(operandValues[1], operandValues[1]));
							}
							else {
								this->symtab->put(operandValues[1], Symbol(operandValues[1], "UND"));
							}
							Symbol sym = this->symtab->get(operandValues[1]);
							this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_16, sym.getNumber(), sym));
						}
						/*else if (this->symtab->get(operandValues[1]).getDefined()) {
							c = this->symtab->get(operandValues[1]).getOffset();
							if (c > 65535) {
								throw new AssemblyException("Argument c naredbe [" + instruction + "] mora imati celobrojnu vrednost u opsegu [0, 65535] .");
							}
						}
						else {*/
							Symbol sym = this->symtab->get(operandValues[1]);
							this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), this->parser->getCurrentSection(), Constants::R_386_16, sym.getNumber(), sym));
						//}
					}
					opcode |= (dstReg & 15) << 20;
					opcode |= (c & 65535);
				}
				//for (unsigned int i = 0; i < operandValues.size(); i++) {
				//	if (this->parser->isNumber(operandValues[i])) {
				//		//It is a literal value
				//		int value = std::stoi(operandValues[i], nullptr, 0);
				//		if (value >= std::pow(256, 4)) {
				//			throw new AssemblyException("Vrednost [" + std::to_string(value) + "] ne može da stane u " + std::to_string(4) + " bajta.");
				//		}

				//	}
				//	else if (this->parser->isRegister(operandValues[i])) {

				//	}
				//	else {
				//		//It is a label
				//		if (!this->symtab->contains(operandValues[i])) {
				//			this->symtab->put(operandValues[i], Symbol(operandValues[i], "UND"));
				//		}
				//		Symbol sym = this->symtab->get(operandValues[i]);
				//		if (instruction == "ldch") {
				//			this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), "R_386_16_H", sym.getNumber(), sym));
				//		}
				//		else if (instruction == "ldcl") {
				//			this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), "R_386_16_L", sym.getNumber(), sym));
				//		}
				//		else {
				//			this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), "R_386_32", sym.getNumber(), sym));
				//		}
				//		this->relocationList->addNode(new RelocNode(currentMemoryUnit->size(), this->parser->getCurrentSectionOffset(), "R_386_32", sym.getNumber(), sym));
				//		//currentMemoryUnit->setWithFixedSize(0, 4); //Should be rewritten with symbol (label) value afterwards
				//	}
				//}
				currentMemoryUnit->setWithFixedSizeInstruction(opcode, 4);
				this->parser->incTotalByteCount(4);
				this->parser->incCurrentSectionOffset(4);
			}
		}
		if (this->input->eof()) {
			throw new AssemblyException("Ne postoji naredba .end");
		}
		line = readLine();
		line = this->parser->trimLine(line);
	}
}

void MainAssemblyUnit::passTwo() {
	int numberOfSections = this->parser->getNumberOfSections();
	for (std::map<std::string, Symbol>::iterator it = this->symtab->begin(); it != this->symtab->end(); it++) {
		Symbol& sym = it->second;
		if (sym.getSectionName() != sym.getName()) {
			sym.setNumber(sym.getNumber() + numberOfSections);
			if (sym.getVisibility() == local) {
				sym.setRelocNumber(this->getSectionNumberForSymbol(sym));
			}
			else {
				sym.setRelocNumber(sym.getNumber());
			}
		}
	}

	RelocNode* node = this->relocationList->peek();
	while (node != nullptr) {
		Symbol sym = Constants::INVALID_SYMBOL;
		for (std::map<std::string, Symbol>::iterator it = this->symtab->begin(); it != this->symtab->end() && sym == Constants::INVALID_SYMBOL; it++) {
			if (it->second.getName() == node->getSymbol().getName()) {
				sym = it->second;
			}
		}
		if (sym == Constants::INVALID_SYMBOL) {
			throw new AssemblyException("Pronađena je neuvezena i nedefinisana labela [" + node->getSymbol().getName() + "]");
		}
		node->setSymbol(sym);
		node->setNumber(sym.getRelocNumber());
		node = node->getNext();
	}
	node = this->relocationList->peek();
	while (node != nullptr) {
		if (node->getSymbol().getDefined()) {
			int memoryLocation = node->getMemoryLocation();
			int memorySize = 1;
			if (node->getType() == Constants::R_386_16) {
				memorySize = 2;
			}
			if (node->getType() == Constants::R_386_32 || node->getType() == Constants::R_386_PC32) {
				memorySize = 4;
			}
		}
		node = node->getNext();
	}
}

void MainAssemblyUnit::processCompleted(bool success, std::string message) {
	if (success) {
		std::cout << "Zadatak je uspešno završen\n";
	}
	else {
		std::cout << "Došlo je do greške prilikom prevođenja ulaza\n";
		std::cout << "Greška: " << message << std::endl;
	}
}

void MainAssemblyUnit::printToOutput() {
	*output << Constants::SYMBOL_TABLE_HEADER;
	*output << "0" << "\t\t" << Constants::UNDEFINED_SECTION_KEY << "\t\t" << Constants::UNDEFINED_SECTION_KEY << "\t\t" << "0" << "\t\t" << "global" << std::endl;
	for (int i = 1; i <= this->symtab->size(); i++) {
		for (std::map<std::string, Symbol>::iterator it = this->symtab->begin(); it != this->symtab->end(); it++) {
			if (it->second.getNumber() == i) {
				Symbol s = it->second;
				*output << this->outputToHexFormatLimitless(s.getNumber()) << "\t\t" << s.getName() << "\t" << s.getSectionName() << "\t" << this->outputToHexFormatLimitless(s.getOffset()) << "\t\t" << s.printVisibility() << std::endl;
			}
		}
	}
	*output << std::endl << std::endl;
	Memory* mem;
	while ((mem = this->memory->pop()) != nullptr) {
		*output << Constants::MEMORY_HEADER << mem->getSectionName() << "[" << mem->size() << "]" << std::endl;
		for (int i = 0; i < mem->size(); i++) {
			*output << this->outputToHexFormat(mem->get(i)) << "\t";
			if ((i + 1) % 4 == 0) {
				*output << "\n";
			}
		}
		if (mem->size() % 4 != 0) {
			*output << std::endl;
		}
		*output << Constants::RELOCATION_HEADER << mem->getSectionName() << std::endl;
		*output << Constants::RELOCATION_TABLE_HEADER;
		RelocNode* node = this->relocationList->peek();
		while (node != nullptr) {
			if (node->getSectionName() == mem->getSectionName()) {
				*output << this->outputToHexFormatLimitless(node->getOffset()) << "\t" << node->getType() << "\t" << this->outputToHexFormatLimitless(node->getNumber()) << std::endl;
				node = this->relocationList->deleteNode(node);
			}
			else {
				node = node->getNext();
			}
		}
		
		if (this->memory->peek() != nullptr) {
			*output << std::endl << std::endl;
		}
	}
}

std::string MainAssemblyUnit::outputToHexFormat(int value) {
	assert(value >= 0 && value < 256);
	int firstDigit = value / 16;
	int secondDigit = value % 16;
	return integerToChar(firstDigit).append(integerToChar(secondDigit));
}

std::string MainAssemblyUnit::outputToHexFormatLimitless(int value) {
	std::string output = "";
	while (value > 255) {
		output = this->outputToHexFormat(value & 255) + output;
		value = value >> 8;
	}
	return this->outputToHexFormat(value & 255) + output;
}

std::string MainAssemblyUnit::integerToChar(int value) {
	assert(value >= 0 && value < 16);
	switch (value)
	{
	case 0:
		return "0";
	case 1:
		return "1";
	case 2:
		return "2";
	case 3:
		return "3";
	case 4:
		return "4";
	case 5:
		return "5";
	case 6:
		return "6";
	case 7:
		return "7";
	case 8:
		return "8";
	case 9:
		return "9";
	case 10:
		return "A";
	case 11:
		return "B";
	case 12:
		return "C";
	case 13:
		return "D";
	case 14:
		return "E";
	case 15:
		return "F";
	default:
		return "X";
	}
}

int MainAssemblyUnit::getSectionNumberForSymbol(Symbol sym) {
	int number = -1;
	for (std::map<std::string, Symbol>::iterator it = this->symtab->begin(); it != this->symtab->end(); it++) {
		if (sym.getSectionName() == it->second.getName()) {
			number = it->second.getNumber();
		}
	}
	assert(number != -1);
	return number;
}