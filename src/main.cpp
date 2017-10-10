#include "MainAssemblyUnit.h"
#include "Constants.h"
#include <stdio.h>
#include <iostream>
#include <string.h>

int main(int argc, char* argv[]) {
	std::string inputFile = "";
	if (argc > 1) {
		std::string argv1 = argv[1];
		if (argv1 == "-batch") {
			//Batch mode
			MainAssemblyUnit assemblyUnit = MainAssemblyUnit();
			for (int i = 2; i < argc; i++) {
				std::string filename = argv[i];
				Constants::initMapper();
				assemblyUnit.processFile(filename);
				std::cout << "\n.....................................................\n";
			}
		}
		else {
			inputFile = argv[1];
			std::string outputFile = inputFile + ".etf";
			if (argc > 2) {
				outputFile = argv[2];
			}
			Constants::initMapper();
			MainAssemblyUnit assemblyUnit = MainAssemblyUnit(inputFile, outputFile);
			assemblyUnit.processFile();
		}
	}
	else {
		std::cout << "You have to specify assembly file for processing." << std::endl;
		std::cout << "Instruction format: |tpasm FILENAME|" << std::endl;
		std::cout << "Batch mode instruction format: |tpasm -batch FILENAME1, FILENAME2, ...|" << std::endl;
		std::cout << "The program will close...";
	}
	std::getchar();
}