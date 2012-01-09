#include "primitiveobject.h"

using namespace KinematicModel;

PrimitiveObject::PrimitiveObject() : /*deathWish(false),*/index(0), parentObject(NULL), solidShape(NULL), solidObject(NULL), displayList(0), listPending(false)
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

QString PrimitiveObject::getName() const
{ 
	if ( name != "" ) { return name; }
	else { return QString("primitive") + QString::number(index); }
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

void PrimitiveObject::setCollidingColor( QColor color )
{
	collidingColor[0] = color.redF();
	collidingColor[1] = color.greenF();
	collidingColor[2] = color.blueF();
	collidingColor[3] = color.alphaF();
}

void PrimitiveObject::setFreeColor( QColor color )
{
	freeColor[0] = color.redF();
	freeColor[1] = color.greenF();
	freeColor[2] = color.blueF();
	freeColor[3] = color.alphaF();
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
