#include "robotmodelexception.h"

RobotModelException::RobotModelException( const QString& msg )
{
	errStr = "RobotModelException:";
	errStr.append(msg);
}
RobotModelException::~RobotModelException()
{
}

const char* RobotModelException::what()
{
    return errStr.toStdString().c_str();
}