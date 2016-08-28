#ifndef a_exception
#define a_exception

#include <stdexcept>

class AssemblyException : public std::runtime_error {
private:
	std::string message;
public:
	AssemblyException(std::string message);
	virtual ~AssemblyException() throw();
	std::string getMessage();
};

#endif