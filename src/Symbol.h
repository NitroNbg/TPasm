#ifndef symbol
#define symbol

#include <string>

enum Visibility {local, global};

class Symbol {
private:
	std::string name;
	std::string sectionName;
	int offset;
	Visibility visibility;
	int number;
	int relocNumber;
	bool defined;
public:
	Symbol();
	Symbol(std::string name);
	Symbol(std::string name, std::string section);
	std::string getName();
	void setName(std::string name);
	std::string getSectionName();
	void setSectionName(std::string sectionName);
	int getOffset();
	void setOffset(int offset);
	Visibility getVisibility();
	void setVisibility(Visibility visibility);
	std::string printVisibility();
	int getNumber();
	void setNumber(int number);
	int getRelocNumber();
	void setRelocNumber(int number);
	bool getDefined();
	void setDefined(bool value);
	bool operator ==(const Symbol s2) const;
	bool operator !=(const Symbol s2) const;
};

#endif