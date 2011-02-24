#include "iCubBabbler.h"
#include <qthread.h>

ICubBabbler::ICubBabbler() //: runThread(true)
{
}
ICubBabbler::~ICubBabbler()
{
}

bool ICubBabbler::configure( const char* _robotName )
{
	robotName = _robotName;
	return configure();
}

bool ICubBabbler::configure()
{
	if ( !torso.open( robotName.toAscii().data(), "torso" )) { return 0; }
	if ( !head.open( robotName.toAscii().data(), "head" )) { return 0; }
	if ( !leftArm.open( robotName.toAscii().data(), "left_arm" )) { return 0; }
	if ( !rightArm.open( robotName.toAscii().data(), "right_arm" )) { return 0; }

	return 1;
}

void ICubBabbler::crackBaby( qreal period, qreal velocity, bool hands )
{
	while (true) //TODO: put this in a thread that we can stop gracefully
	{
		torso.crackBaby( velocity, hands );
		head.crackBaby( velocity, hands );
		leftArm.crackBaby( velocity, hands );
		rightArm.crackBaby( velocity, hands );
		printf("\n");
		usleep(period);
	}
}

void ICubBabbler::doTheRobot( qreal period, qreal velocity, bool hands )
{
	//bool headDone, torsoDone, lArmDone, rArmDone;
	
	while (true) //TODO: put this in a thread that we can stop gracefully
	{
		printf("SENDING POSITION MOVE:\n");
		torso.doTheRobot( velocity, hands);
		head.doTheRobot( velocity, hands);
		leftArm.doTheRobot( velocity, hands);
		rightArm.doTheRobot( velocity, hands);
		printf("\n");
		
		//headDone = false,
		//torsoDone = false,
		//lArmDone = false,
		//rArmDone = false;
		
		//while ( !headDone || !torsoDone || !lArmDone || !rArmDone )
		//{
		//	head.checkMotionDone(&headDone);
		//	torso.checkMotionDone(&torsoDone);
		//	leftArm.checkMotionDone(&lArmDone);
		//	rightArm.checkMotionDone(&rArmDone);
		//}
		
		usleep(period);
	}
}

/*void ICubBabbler::oscillate()
{
	while (true)
	{
		torso.oscillate();
		head.oscillate();
		leftArm.oscillate();
		rightArm.oscillate();
		usleep(1000);
	}
}


void ICubBabbler::crackBabySafe( int period, int magnitude )
{
	while (true) //TODO: put this in a thread that we can stop
	{
		torso.crackBabySafe( magnitude );
		head.crackBabySafe( magnitude );
		leftArm.crackBabySafe( magnitude );
		rightArm.crackBabySafe( magnitude );
		
		usleep(1000*period);
	}
}
*/