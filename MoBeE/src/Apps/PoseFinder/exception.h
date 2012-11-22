#ifndef __ISNAN__
#define __ISNAN__

#include <string>

class NanException : public std::exception {
};

class StringException : public std::exception {
	std::string d_msg;
public:
	StringException(std::string msg): d_msg(msg){}

	virtual const char *what() const throw () {
		return d_msg.c_str();
	}

	~StringException() throw (){}
};

#endif
