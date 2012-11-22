#include "exception.h"

using namespace MoBeE;

MoBeEException::MoBeEException( const QString& msg )
{
	errStr = "MoBeEException: ";
	errStr.append(msg);
}
MoBeEException::~MoBeEException() throw()
{
}

const char* MoBeEException::what() const throw()
{
	return errStr.toStdString().c_str();
}