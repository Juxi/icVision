#include "virtualskinexception.h"

using namespace VirtualSkin;

VirtualSkinException::VirtualSkinException( const QString& msg )
{
	errStr = "VirtualSkinException: ";
	errStr.append(msg);
}
VirtualSkinException::~VirtualSkinException() throw()
{
}

const char* VirtualSkinException::what() const throw()
{
	return errStr.toStdString().c_str();
}