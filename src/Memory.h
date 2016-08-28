#ifndef memory_def
#define memory_def

#include <vector>
#include <string>

class Memory {
private:
	std::vector<int> bytes;
	std::string section;
	Memory* next;
public:
	Memory();
	Memory(std::string section);
	int size();
	int get(int location);
	void set(int value);
	void set(int value, int location);
	void setWithFixedSize(int value, int size);
	void setWithFixedSize(int value, int location, int size);
	void set(unsigned int value);
	void set(unsigned int value, int location);
	void setWithFixedSize(unsigned int value, int size);
	void setWithFixedSize(unsigned int value, int location, int size);
	void setWithFixedSizeInstruction(int value, int size);
	void setWithFixedSizeInstruction(int value, int location, int size);
	void setWithFixedSizeInstruction(unsigned int value, int size);
	void setWithFixedSizeInstruction(unsigned int value, int location, int size);
	std::string getSectionName();
	void setSectionName(std::string name);
	Memory* getNext();
	void setNext(Memory* node);
};

#endif