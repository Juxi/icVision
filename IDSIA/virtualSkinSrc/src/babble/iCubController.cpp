#include "iCubController.h"

ICubController::ICubController() //: runThread(true)
{
}
ICubController::~ICubController()
{
	printf("Deleting %s\n", robotName.toStdString().c_str());
	//if ( isRunning() ) { stop(); }
	//statusPort.close();
}

bool ICubController::open( const char* _robotName )
{
	robotName = _robotName;
	return open();
}

bool ICubController::open()
{
	if ( !torso.open( robotName.toStdString().c_str(), "torso" )) { return 0; }
	if ( !head.open( robotName.toStdString().c_str(), "head" )) { return 0; }
	if ( !leftArm.open( robotName.toStdString().c_str(), "left_arm" )) { return 0; }
	if ( !rightArm.open( robotName.toStdString().c_str(), "right_arm" )) { return 0; }

	return 1;
}

void ICubController::close()
{
	torso.close();
	head.close();
	rightArm.close();
	leftArm.close();
}