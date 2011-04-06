#include "displaylist.h"
#include "constants.h"

#include <qthread.h>

using namespace RobotModel;

//uint DisplayList::MAX_NUM_DISPLAYLISTS = 1;
GLfloat DisplayList::red[4] =  { 1.0, 0.0, 0.0, 1.0 };
GLfloat DisplayList::gray[4] = { 0.3, 0.3, 0.3, 1.0 };

DisplayList::DisplayList() : index(0)
{
}

DisplayList::~DisplayList()
{
}

bool DisplayList::isColliding() const
{
	return timeSinceLastCollision.elapsed() < 20 * YARP_PERIOD_ms;
}

void DisplayList::render()
{
	//printf("   render - %p\n",QThread::currentThread());
	if ( glIsList(index) )
	{
		if ( isColliding() )
		{
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, red);
		}
		else
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gray);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, gray);
		}
		
		glPushMatrix();
			glMultMatrixd( T.constData() );
			glCallList( index );
		glPopMatrix();
	}
}
