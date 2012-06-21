#include "iCubController.h"
#include <time.h>
#include <iostream>
#include "roadmap.h"

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
		printf("Failed to open one or more body parts.\n");
		return 0;
	}
	numJoints = torso.getNumJoints() + right_arm.getNumJoints() + left_arm.getNumJoints();
	
	filterRpc.open("/filterCommand");
	if ( !yarp.connect("/filterCommand","/filterRpc") )
	{ 
		printf("failed to connect to /filterRpc\n");
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
	filterRpc.close();
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

bool iCubController::isWithinLimits( const std::vector<double>& poss )
{
	std::vector<double> torsoPoss, leftPoss, rightPoss;
	if ( !chop( poss, torsoPoss, rightPoss, leftPoss ) ) {
		printf("chop failed\n");
		return false;
	}
	
	bool returnVal = true;
	
	if ( !torso.isWithinLimits(torsoPoss) ) returnVal = false;
	else if ( !left_arm.isWithinLimits(leftPoss) ) returnVal = false;
	else if ( !right_arm.isWithinLimits(rightPoss) ) returnVal = false;
	
	if ( !returnVal ) {
		std::cout << Roadmap::CGAL_Vector(torsoPoss.size(),torsoPoss.begin(),torsoPoss.end()) << std::endl;
		std::cout << Roadmap::CGAL_Vector(leftPoss.size(),leftPoss.begin(),leftPoss.end()) << std::endl;
		std::cout << Roadmap::CGAL_Vector(rightPoss.size(),rightPoss.begin(),rightPoss.end()) << std::endl;
		return false;
	}
	
	return true;
}

std::vector<double> iCubController::withinLimits( const std::vector<double>& poss )
{
	std::vector<double> torsoPoss, leftPoss, rightPoss, result;
	if ( !chop( poss, torsoPoss, rightPoss, leftPoss ) ) {
		printf("chop failed\n");
		return result;
	}
	
	result = torso.withinLimits(torsoPoss);
	leftPoss = left_arm.withinLimits(leftPoss);
	rightPoss = right_arm.withinLimits(rightPoss);
	
	std::vector<double>::iterator i;
	for ( i = leftPoss.begin(); i != leftPoss.end(); ++i )
	{
		result.push_back(*i);
	}
	for ( i = rightPoss.begin(); i != rightPoss.end(); ++i )
	{
		result.push_back(*i);
	}
	
	if ( result.size() != (unsigned int)numJoints ) { result.clear(); }
	
	return result;
}

void iCubController::setRefVelocity( int v )
{
	torso.setRefVelocity(v);
	left_arm.setRefVelocity(v);
	right_arm.setRefVelocity(v);
}

void iCubController::setRefAcceleration( int a )
{
	torso.setRefAcceleration(a);
	left_arm.setRefAcceleration(a);
	right_arm.setRefAcceleration(a);
}

bool iCubController::velocityMove( std::vector<double> v )
{ 
	if ( !isValid() ) { return 0; }
	
	if ( v.size() != getNumJoints() )
		return 0;
	
	std::vector<double> torsoCmd, leftCmd, rightCmd;
	if ( !chop( v, torsoCmd, rightCmd, leftCmd ) ) { printf("chop failed\n"); return 0; }
	if ( !torso.velocityMove(torsoCmd) ) { printf("torso move failed\n"); return 0; }
	if ( !right_arm.velocityMove(rightCmd) ) { printf("right arm move failed\n"); return 0; }
	if ( !left_arm.velocityMove(leftCmd) ) { printf("left arm move failed\n"); return 0; }
	
	return 1;
}

bool iCubController::positionMove( std::vector<double> poss )
{ 
	if ( !isValid() ) { return 0; }
	
	if ( poss.size() != getNumJoints() )
		return 0;
	
	std::vector<double> torsoCmd, leftCmd, rightCmd;
	if ( !chop( poss, torsoCmd, rightCmd, leftCmd ) ) { printf("chop failed\n"); return 0; }
	if ( !torso.positionMove(torsoCmd) ) { printf("torso move failed\n"); return 0; }
	if ( !left_arm.positionMove(leftCmd) ) { printf("left arm move failed\n"); return 0; }
	if ( !right_arm.positionMove(rightCmd) ) { printf("right arm move failed\n"); return 0; }
	
	return 1;
}

bool iCubController::setJointMask( std::vector<bool> vals )
{
	jointMask = vals;
	std::vector<bool> torsoMask, leftMask, rightMask;
	if ( !chop( vals, torsoMask, rightMask, leftMask ) ) { return 0; }
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
	
	if ( !r1 || !r2 || !r3 )
	{ 
		printf("checkMotionDone() failed. was the connection interrupted?\n");
		return 0;
	}
	if ( f1 && f2 && f3 ) { *flag = 1; }
	
	return 1;
}

std::vector<double> iCubController::getRandomPose()
{
	std::vector<double> q = torso.getRandomPose(),
						qLeft = left_arm.getRandomPose(),
						qRight = right_arm.getRandomPose();
	
	std::vector<double>::iterator i;
	for ( i = qRight.begin(); i != qRight.end(); ++i )
	{
		q.push_back(*i);
	}
	for ( i = qLeft.begin(); i != qLeft.end(); ++i )
	{
		q.push_back(*i);
	}
	
	if ( q.size() != (unsigned int)numJoints ) { q.clear(); }
	
	emit newSample(q);
	return q;
}


double iCubController::maxDiff(std::vector<double> a,std::vector<double> b)
{
	double result = 0;
	std::vector<double>::iterator i,j;
	int count = 0;
	for ( i=a.begin(), j=b.begin();
		 i!=a.end() && j!=b.end();
		 ++i, ++j )
	{
		//if ( count < 10 || (count > 18 && count < 26) )
		if ( jointMask.at(count) )
		{
			if ( qAbs(*i-*j) > result )
				result = qAbs(*i-*j);
		}
		count++;
	}
	return result;
}

std::vector<double> iCubController::diff( std::vector<double> a )
{
	std::vector<double> cp = getCurrentPose(),
						result;
	std::vector<double>::iterator i;
	int count = 0;
	for ( i=a.begin(); i!=a.end(); ++i )
	{
		if ( jointMask.at(count) )
			result.push_back( a.at(count)-cp.at(count) );
		else
			result.push_back( 0 );
		
		count++;
	}
	return result;
}

//TODO This should pass by reference and return a bool
std::vector<double> iCubController::getCurrentPose()
{
	std::vector<double> q = torso.getCurrentPose(),
						qLeft = left_arm.getCurrentPose(),
						qRight = right_arm.getCurrentPose();
	
	std::vector<double>::iterator i;
	for ( i = qRight.begin(); i != qRight.end(); ++i )
	{
		q.push_back(*i);
	}
	for ( i = qLeft.begin(); i != qLeft.end(); ++i )
	{
		q.push_back(*i);
	}
	
	//std::cout << q.size() << ", " << qLeft.size() << ", " << qRight.size() << std:: endl;
	
	if ( q.size() != (unsigned int)getNumJoints() ) { q.clear(); }
	
	
	emit newSample(q);
	return q;
}
