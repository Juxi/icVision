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
	
/*void PartBabbler::oscillate()
{
	int i=0;
	qreal v;
	if ( partController.isValid() )
	{
		for (i=0; i<partController.getNumJoints(); i++)
		{
			if ( i < 7 )
			{
				v=100*sin(t);
				if ( partController.velocityMove(i,100*sin(t)) ) { printf("%f ",v); }
				else { printf("x "); }
			}
		}
		printf("\n");
		t += 0.01;
		if ( t >= 2*M_PI ) { t = 0; }
	}
	
}

void BabblePart::crackBabySafe( int maxSpeed )
{
	int i=0;
	qreal x, min, max;
	for (i=0; i<numJoints; i++)
	{
		lim->getLimits(i, &min, &max);
		enc->getEncoder(i, &x);
		qreal normPos = (x-min)/(max-min);
		
		qreal probFwd = (1-normPos)*rand()/RAND_MAX;
		qreal probBack = -normPos*rand()/RAND_MAX;
		qreal move = maxSpeed*(probFwd+probBack);
		
		vel->velocityMove(i,move);
	}
}
*/
