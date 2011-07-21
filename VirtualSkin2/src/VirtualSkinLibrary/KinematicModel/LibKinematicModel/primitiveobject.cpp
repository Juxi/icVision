#include "primitiveobject.h"

using namespace KinematicModel;

PrimitiveObject::PrimitiveObject() : /*deathWish(false),*/ solidShape(NULL), solidObject(NULL), displayList(0), listPending(false)
{
	collidingColor[0] = 0.7; collidingColor[1] = 0.0; collidingColor[2] = 0.0; collidingColor[3] = 0.5;
		 freeColor[0] = 0.9;	  freeColor[1] = 0.9;	   freeColor[2] = 0.9;	    freeColor[3] = 0.5; 
			 black[0] = 0.0;		  black[0] = 0.0;		   black[0] = 0.0;			black[0] = 1.0;
}

PrimitiveObject::~PrimitiveObject()
{
	//QMutexLocker locker(&mutex);
	//printf("PrimitiveObject destructor called\n");
	if ( solidObject ) { DT_DestroyObject( solidObject ); }
	if ( solidShape ) { DT_DeleteShape( solidShape ); }
	//printf("deleted PrimtiveObject\n");
}

void PrimitiveObject::setListPending( bool b )
{ 
	listPending = b; 
	//if (listPending) printf(" set listPending\n");
	//else printf(" unset listPending\n");
}

void PrimitiveObject::display()
{
	//QMutexLocker locker(&mutex);
	
	makeDisplayList();
	setListPending( false );
}

void PrimitiveObject::updateSolid( const QMatrix4x4& M )
{
	if ( solidObject )
	{
		DT_SetMatrixd( solidObject, (M*T).data() );
	}
}

void PrimitiveObject::setCollidingColor( GLfloat color[4] )
{
	collidingColor[0] = color[0];
	collidingColor[1] = color[1];
	collidingColor[2] = color[2];
	collidingColor[3] = color[3];
}

void PrimitiveObject::setFreeColor( GLfloat color[4] )
{
	freeColor[0] = color[0];
	freeColor[1] = color[1];
	freeColor[2] = color[2];
	freeColor[3] = color[3];
}

void PrimitiveObject::render()
{
	//QMutexLocker locker(&mutex);
	
	if ( glIsList( displayList ) )
	{
		if ( isColliding() )
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, collidingColor);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, collidingColor);
		}
		else 
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, freeColor);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, freeColor);
		}
		
		glPushMatrix();
		glMultMatrixd( T.constData() );
		glCallList( displayList );
		glPopMatrix();
	}
}
