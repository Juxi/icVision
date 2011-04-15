#include "partBabbler.h"

PartBabbler::PartBabbler() : freq(0), t(0)
{
}
PartBabbler::~PartBabbler()
{
	partController.close();
}

void PartBabbler::crackBaby( qreal maxSpeed, bool hands )
{
	int i=0;
	if ( partController.isValid() )
	{
		for (i=0; i<partController.getNumJoints(); i++)
		{
			if ( i < 7 || hands )
			{
				if ( (qreal)rand()/RAND_MAX > 0.5 ) { maxSpeed = -maxSpeed; }
				if ( partController.velocityMove(i,maxSpeed*(qreal)rand()/RAND_MAX) ) { printf("."); }
				else { printf("x"); }
			}
		}
	}
}

void PartBabbler::doTheRobot( qreal maxSpeed, bool hands )
{
	if ( partController.isValid() )
	{ 
		if ( partController.randomPosMove( maxSpeed, hands ) ) { printf(" :-)\n"); }
		else { printf(" :-(\n"); }
	}
}