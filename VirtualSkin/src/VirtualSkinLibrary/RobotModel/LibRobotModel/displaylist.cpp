#include "displaylist.h"

#include <qthread.h>

using namespace RobotModel;

//uint DisplayList::MAX_NUM_DISPLAYLISTS = 1;
GLfloat DisplayList::red[4] =  { 0.3, 0.0, 0.0, 1.0 };
GLfloat DisplayList::gray[4] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat DisplayList::transpRed[4] =  { 0.7, 0.0, 0.0, 0.7 };
GLfloat DisplayList::transpGray[4] = { 0.9, 0.9, 0.9, 0.7 };

DisplayList::DisplayList() : index(0), semiTransparent(true)
{
}

DisplayList::~DisplayList()
{
}

bool DisplayList::isColliding() const
{
	return timeSinceLastCollision.elapsed() < collisionTimeout;
}

void DisplayList::render()
{
	//printf("   render - %p\n",QThread::currentThread());
	if ( glIsList(index) )
	{
		if ( semiTransparent )
		{
			if ( isColliding() )
			{
				glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, transpRed);
				glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, transpRed);
			}
			else
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, transpGray);
				glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, transpGray);
			}
		}
		else
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
		}
		
		glPushMatrix();
			glMultMatrixd( T.constData() );
			glCallList( index );
		glPopMatrix();
	}
}
