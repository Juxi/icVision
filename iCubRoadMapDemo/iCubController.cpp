#include "iCubController.h"
#include <time.h>
#include <iostream>

iCubController::iCubController() : numJoints(0)
{
}

iCubController::~iCubController()
{
	close();
}

bool iCubController::open( const char* robotName )
{
	if ( !torso.open( robotName, "torso" ) || !right_arm.open( robotName, "right_arm" ) || !left_arm.open( robotName, "left_arm" ) )
	{
		return 0;
	}
	numJoints = torso.getNumJoints() + right_arm.getNumJoints() + left_arm.getNumJoints();
	
	filterRpc.open("/filterCommand");
	if ( !yarp.connect("/filterCommand","/filterRpc") )
	{ 
		return 0;
	}
	
	return 1;
}

bool iCubController::isValid()
{
	if ( !torso.isValid() || !right_arm.isValid() || !left_arm.isValid() )
	{
		return 0;
	}
	return 1;
}

void iCubController::close()
{
	torso.close();
	left_arm.close();
	right_arm.close();
}

bool iCubController::setWaypoint()
{
	yarp::os::Bottle b;
	b.addString("setWP");
	return filterRpc.write( b );
	//return false;
}

bool iCubController::stop()
{ 
	if ( !torso.stop() || !right_arm.stop() || !left_arm.stop() )
	{
		return 0;
	}
	return 1;
}

int	iCubController::getNumJoints()
{
	return torso.getNumJoints() + left_arm.getNumJoints() + right_arm.getNumJoints();
}

bool iCubController::positionMove( std::vector<double> poss )
{ 
	if ( !isValid() ) { return 0; }
	
	std::vector<double> torsoCmd, leftCmd, rightCmd;
	if ( !chop( poss, torsoCmd, leftCmd, rightCmd ) ) { printf("chop failed\n"); return 0; }
	if ( !torso.positionMove(torsoCmd) ) { printf("torso move failed\n"); return 0; }
	if ( !left_arm.positionMove(leftCmd) ) { printf("left arm move failed\n"); return 0; }
	if ( !right_arm.positionMove(rightCmd) ) { printf("right arm move failed\n"); return 0; }
	
	return 1;
}

bool iCubController::setJointMask( std::vector<bool> vals )
{
	std::vector<bool> torsoMask, leftMask, rightMask;
	if ( !chop( vals, torsoMask, leftMask, rightMask ) ) { return 0; }
	if ( !torso.setJointMask(torsoMask) ) { return 0; }
	if ( !left_arm.setJointMask(leftMask) ) { return 0; }
	if ( !right_arm.setJointMask(rightMask) ) { return 0; }
	
	return 1;
}

bool iCubController::checkMotionDone( bool* flag )
{
	*flag = 0;
	
	bool	r1 = false, r2 = false, r3 = false,
			f1 = false, f2 = false, f3 = false;
	
	r1 = torso.checkMotionDone( &f1 );
	r2 = left_arm.checkMotionDone( &f2 );
	r3 = right_arm.checkMotionDone( &f3 );
	
	if ( !r1 || !r2 || !r3 ) { return 0; }
	if ( f1 && f2 && f3 ) { *flag = 1; }
	
	return 1;
}

std::vector<double> iCubController::getRandomPose()
{
	std::vector<double> q = torso.getRandomPose(),
						qLeft = left_arm.getRandomPose(),
						qRight = right_arm.getRandomPose();
	
	std::vector<double>::iterator i;
	for ( i = qLeft.begin(); i != qLeft.end(); ++i )
	{
		q.push_back(*i);
	}
	for ( i = qRight.begin(); i != qRight.end(); ++i )
	{
		q.push_back(*i);
	}
	
	if ( q.size() != (unsigned int)numJoints ) { q.clear(); }
	
	emit newSample(q);
	return q;
}


std::vector<double> iCubController::getCurrentPose()
{
	std::vector<double> q = torso.getCurrentPose(),
						qLeft = left_arm.getCurrentPose(),
						qRight = right_arm.getCurrentPose();
	
	std::vector<double>::iterator i;
	for ( i = qLeft.begin(); i != qLeft.end(); ++i )
	{
		q.push_back(*i);
	}
	for ( i = qRight.begin(); i != qRight.end(); ++i )
	{
		q.push_back(*i);
	}
	
	//std::cout << q.size() << ", " << qLeft.size() << ", " << qRight.size() << std:: endl;
	
	if ( q.size() != (unsigned int)getNumJoints() ) { q.clear(); }
	
	
	emit newSample(q);
	return q;
}
