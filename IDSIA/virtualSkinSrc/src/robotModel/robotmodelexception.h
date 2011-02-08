#ifndef ROBOTMODELEXCEPTION_H
#define ROBOTMODELEXCEPTION_H

#include <QString>

class RobotModelException
{
public:
    RobotModelException( const QString& msg = "" );
    ~RobotModelException();
	
	const char* what();

private:
	QString errStr;
};

#endif // ROBOTMODELEXCEPTION_H
