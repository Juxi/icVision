#include "iCubController.h"
#include <time.h>
#include <iostream>

using namespace std;

iCubController::iCubController() : numJoints(0)
{
}

iCubController::~iCubController()
{
	close();
}

bool iCubController::open( const char* robotName )
{
	std::cout << "jooo" << std::endl;
	if ( !torso.open( robotName, "torso" ) || !right_arm.open( robotName, "right_arm" ) || !left_arm.open( robotName, "left_arm" ) || !head.open( robotName, "head" ))
	{
		printf("failed to open one or more body parts\n");
		return 0;
	}
	std::cout << "jooo" << std::endl;
	numJoints = torso.getNumJoints() + right_arm.getNumJoints() + left_arm.getNumJoints() + head.getNumJoints();
	
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
	if ( !torso.stop() || !right_arm.stop() || !left_arm.stop() || !head.stop() )
	{
		return 0;
	}
	return 1;
}

int	iCubController::getNumJoints()
{
	return torso.getNumJoints() + left_arm.getNumJoints() + right_arm.getNumJoints() + head.getNumJoints();
}

std::vector<double>  iCubController::withinLimits( const std::vector<double>& poss )
{
	std::vector<double> torsoPoss, leftPoss, rightPoss, headPoss, result;
	if ( !chop( poss, torsoPoss, headPoss, rightPoss, leftPoss ) ) {
		printf("chop failed\n");
		return result;
	}
	result = torso.withinLimits(torsoPoss);
	leftPoss = left_arm.withinLimits(leftPoss);
	rightPoss = right_arm.withinLimits(rightPoss);
	headPoss = head.withinLimits(headPoss);

	std::vector<double>::iterator i;
	for ( i = headPoss.begin(); i != headPoss.end(); ++i )
	{
		result.push_back(*i);
	}

	for ( i = rightPoss.begin(); i != rightPoss.end(); ++i )
	{
		result.push_back(*i);
	}
		for ( i = leftPoss.begin(); i != leftPoss.end(); ++i )
	{
		result.push_back(*i);
	}

	if ( result.size() != (unsigned int)numJoints ) { result.clear(); }
	
	return result;
}

void iCubController::setVelocity( int v )
{
	cout << "1" << endl;
	torso.setVelocity(v);
	cout << "2" << endl;
	left_arm.setVelocity(v);
	cout << "3" << endl;
	right_arm.setVelocity(v);
	cout << "4" << endl;
	head.setVelocity(v);
}

bool iCubController::positionMove( std::vector<double> poss )
{ 
	if ( !isValid() ) {
		cout << "Error: Position not valid" << endl;
		return 0;
	}
	
	if ( poss.size() != getNumJoints() ) {
		cout << "Error: Goal vector not equal to num joints: " << poss.size() << " != " <<  getNumJoints() << endl;
		return 0;
	}
	
	std::vector<double> torsoCmd, leftCmd, rightCmd, headCmd;
	if ( !chop( poss, torsoCmd, headCmd, rightCmd, leftCmd ) ) { printf("chop failed\n"); return 0; }
	if ( !torso.positionMove(torsoCmd) ) { printf("torso move failed\n"); return 0; }
	if ( !left_arm.positionMove(leftCmd) ) { printf("left arm move failed\n"); return 0; }
	if ( !right_arm.positionMove(rightCmd) ) { printf("right arm move failed\n"); return 0; }
	
	return 1;
}

bool iCubController::setJointMask( std::vector<bool> vals )
{
	jointMask = vals;
	std::vector<bool> torsoMask, headMask, leftMask, rightMask;
	if ( !chop( vals,  torsoMask, headMask, rightMask, leftMask ) ) { return 0; }
	if ( !torso.setJointMask(torsoMask) ) { return 0; }
	if ( !left_arm.setJointMask(leftMask) ) { return 0; }
	if ( !right_arm.setJointMask(rightMask) ) { return 0; }
	
	return 1;
}

bool iCubController::checkMotionDone( bool* flag )
{
	*flag = 0;
	
	bool	r1 = false, r2 = false, r3 = false, r4 = false,
			f1 = false, f2 = false, f3 = false, f4 = false;
	
	r1 = torso.checkMotionDone( &f1 );
	r2 = left_arm.checkMotionDone( &f2 );
	r3 = right_arm.checkMotionDone( &f3 );
	r4 = head.checkMotionDone( &f4 );

	if ( !r1 || !r2 || !r3 || !r4)
	{ 
		printf("checkMotionDone() failed. was the connection interrupted?\n");
		return 0;
	}
	if ( f1 && f2 && f3 && f4 ) { *flag = 1; }
	
	return 1;
}

std::vector<double> iCubController::getRandomPose()
{
	std::vector<double> q = torso.getRandomPose(),
						qHead = right_arm.getRandomPose(),
						qLeft = left_arm.getRandomPose(),
						qRight = right_arm.getRandomPose();
	
	std::vector<double>::iterator i;
	for ( i = qHead.begin(); i != qHead.end(); ++i )
	{
		q.push_back(*i);
	}
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

//TODO This should pass by reference and return a bool
std::vector<double> iCubController::getCurrentPose()
{
	std::vector<double> q = torso.getCurrentPose(),
						qLeft = left_arm.getCurrentPose(),
						qRight = right_arm.getCurrentPose(),
						qHead = head.getCurrentPose();

	std::vector<double>::iterator i;
	for ( i = qHead.begin(); i != qHead.end(); ++i )
	{
		q.push_back(*i);
	}
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
