#include "primitiveobject.h"

using namespace KinematicModel;

PrimitiveObject::PrimitiveObject() : /*deathWish(false),*/index(0), parentObject(NULL), solidShape(NULL), solidObject(NULL), displayList(0), listPending(false)
{
	collidingColor[0]  = 0.7; collidingColor[1]  = 0.0; collidingColor[2] = 0.0; collidingColor[3] = 0.3;
	constraintColor[0] = 0.7; constraintColor[1] = 0.2; constraintColor[2] = 0.0; constraintColor[3] = 0.3;
		 freeColor[0] = 0.3;	  freeColor[1] = 0.3;	   freeColor[2] = 0.3;	    freeColor[3] = 1.0; 
			 black[0] = 0.0;		  black[0] = 0.0;		   black[0] = 0.0;			black[0] = 1.0;

	// create collision timers
	QPair<CollisionType, QTime*> p;
	p = QPair<CollisionType, QTime*>(COLLISION, new QTime());
	collisionTimers.append(p);
	p = QPair<CollisionType, QTime*>(CONSTRAINT, new QTime());
	collisionTimers.append(p);
	p = QPair<CollisionType, QTime*>(FIELD, new QTime());
	collisionTimers.append(p);
}

PrimitiveObject::~PrimitiveObject()
{
	//QMutexLocker locker(&mutex);
	//printf("PrimitiveObject destructor called\n");
	if ( solidObject ) { DT_DestroyObject( solidObject ); }
	if ( solidShape ) { DT_DeleteShape( solidShape ); }
	//printf("deleted PrimitiveObject\n");

	QVector<QPair<CollisionType, QTime*> >::iterator i;
	for (i=collisionTimers.begin(); i!=collisionTimers.end(); ++i) {
		delete (*i).second;
	}
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
    
    printf("\n\n\nFREE COLOR: %f %f %f %f\n\n\n", freeColor[0], freeColor[1], freeColor[2], freeColor[3]);
}

void PrimitiveObject::setConstraintColor( QColor color )
{
	constraintColor[0] = color.redF();
	constraintColor[1] = color.greenF();
	constraintColor[2] = color.blueF();
	constraintColor[3] = color.alphaF();
}

bool PrimitiveObject::setColliding(CollisionType t) { 
	QVector<QPair<CollisionType, QTime*> >::iterator i;
	for (i=collisionTimers.begin(); i!=collisionTimers.end(); ++i) {
		if ((*i).first == t) {
			(*i).second->restart();
			return true;
		}
	}
	return false;
}

CollisionType PrimitiveObject::isColliding() { 
	QVector<QPair<CollisionType, QTime*> >::iterator i;
	for (i=collisionTimers.begin(); i!=collisionTimers.end(); ++i) {
		if ((*i).second->elapsed() < COLLISION_TIMEOUT) {
			return (*i).first;
		}
	}
	return FREE;
}


void PrimitiveObject::render()
{
	//QMutexLocker locker(&mutex);
	
	if ( glIsList( displayList ) )
	{
		switch (isColliding()) {
		case COLLISION:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, collidingColor);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, collidingColor);
			break;
		case CONSTRAINT:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, constraintColor);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, constraintColor);
			break;
		case FIELD:
		default:
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, freeColor);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, freeColor);
			break;
		}
		
		glPushMatrix();
		glMultMatrixd( T.constData() );
		glCallList( displayList );
		glPopMatrix();
	}
}
