#include "displaylist.h"

#include <qthread.h>

using namespace RobotModel;

//uint DisplayList::MAX_NUM_DISPLAYLISTS = 1;

GLfloat DisplayList::gray[4] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat DisplayList::transpGray[4] = { 0.9, 0.9, 0.9, 0.5 };
GLfloat DisplayList::red[4] =  { 0.3, 0.0, 0.0, 1.0 };
GLfloat DisplayList::transpRed[4] =  { 0.7, 0.0, 0.0, 0.5 };
GLfloat DisplayList::green[4] =  { 0.0, 0.3, 0.0, 1.0 };
GLfloat DisplayList::transpGreen[4] =  { 0.0, 0.7, 0.0, 0.5 };
GLfloat DisplayList::blue[4] =  { 0.0, 0.0, 0.3, 1.0 };
GLfloat DisplayList::transpBlue[4] =  { 0.0, 0.0, 0.7, 0.5 };

DisplayList::DisplayList() : index(0)
{
}

DisplayList::~DisplayList()
{
}

bool DisplayList::isColliding() const
{
	bool result = timeSinceLastCollision.elapsed() < collisionTimeout;
	//if (result)
	return result;
}

void DisplayList::render()
{
	//printf("   render - %p\n",QThread::currentThread());
	if ( glIsList(index) )
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, transpGray);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, transpGray);
		
		glPushMatrix();
			glMultMatrixd( T.constData() );
			glCallList( index );
		glPopMatrix();
	}
}
