#include "robotmodelexception.h"

using namespace RobotModel;

RobotModelException::RobotModelException( const QString& msg )
{
	errStr = "RobotModelException: ";
	errStr.append(msg);
}
RobotModelException::~RobotModelException() throw()
{
}

const char* RobotModelException::what() const throw()
{
	return errStr.toStdString().c_str();
}