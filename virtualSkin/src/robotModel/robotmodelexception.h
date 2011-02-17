#ifndef ROBOTMODELEXCEPTION_H
#define ROBOTMODELEXCEPTION_H

#include <QString>

namespace RobotModel {

class RobotModelException
{
public:
    RobotModelException( const QString& msg = "" );
    ~RobotModelException();
	
	const char* what();

private:
	QString errStr;
};
	
}

#endif
