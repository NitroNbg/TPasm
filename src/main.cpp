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
		std::cout << "Niste uneli putanju ka datoteci koju želite da prevedete" << std::endl;
	}
	std::getchar();
}