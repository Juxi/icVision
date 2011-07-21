#include "objectsoup.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

void ObjectSoup::stop()
{
	printf("\n*** ObjectSoup thread is shutting down ***\n\n");
	keepRunning = false;
	while ( isRunning() )
	{
		printf(".");
		msleep(1);
	}
	printf("\n\n");
}

void ObjectSoup::run()
{
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
	
	
	// fill the world with objects
	KinematicModel::CompositeObject* obj[num];
	for ( int i =0; i<num; ++i )
	{
		// make a new object
		if (verbose) printf( "Composite Object: %d\n", i );	
		obj[i] = makeARandomObjectLikeAMothaFucka();
	}
	
	/**/
	int j;
	time.start();
	while ( keepRunning )
	{
		j = qrand() % (num+1);
		if (verbose) printf( "replacing object: %d\n", j );
		obj[j]->kill();
		obj[j] = makeARandomObjectLikeAMothaFucka();
		msleep(20);
	}
	
	printf("ObjectSoup::run() returned\n");
}

KinematicModel::CompositeObject* ObjectSoup::makeARandomObjectLikeAMothaFucka()
{
	KinematicModel::CompositeObject* obj = new KinematicModel::CompositeObject( theModel.OBSTACLE() );
	
	// choose a random color for the object
	GLfloat collidingColor[4] = { (float)qrand()/RAND_MAX, (float)qrand()/RAND_MAX, (float)qrand()/RAND_MAX, 0.5 };
	GLfloat freeColor[4];
	freeColor[0] = collidingColor[0];
	freeColor[1] = collidingColor[1];
	freeColor[2] = collidingColor[2];
	freeColor[3] = 1.0;
	
	// choose a random number of primitives to append to the object 1 to 5 inclusive
	int numPrimitives =  (qrand() % 6 + 1);
	if (verbose) printf( " Num Primitives: %d\n", numPrimitives);
	KinematicModel::PrimitiveObject* primitive;
	for ( int j=0; j<numPrimitives; ++j )
	{
		// choose a random geometry type for each primitive
		KinematicModel::GeomType geomType =  (KinematicModel::GeomType)( qrand() % ((int)KinematicModel::BOX+1) );
		if (verbose) printf( "  Geometry Type: %d\n", (int)geomType );
		switch (geomType)
		{
			case KinematicModel::SPHERE:	primitive = new KinematicModel::Sphere( (float)qrand()/RAND_MAX/10.0+0.01 );
				break;
			case KinematicModel::CYLINDER:	primitive = new KinematicModel::Cylinder( (float)qrand()/RAND_MAX/10+.01,
																					  (float)2*qrand()/RAND_MAX/10+.01 );
				break;
			case KinematicModel::BOX:		primitive = new KinematicModel::Box( QVector3D((float)qrand()/RAND_MAX/10+.01,
																						   (float)qrand()/RAND_MAX/10+.01,
																						   (float)qrand()/RAND_MAX/10+.01) );
				break;
			default:
				break;
		}
		primitive->setCollidingColor(collidingColor);
		primitive->setFreeColor(freeColor);
		primitive->translate(randomTranslation(0.1));
		primitive->cartesianRotate(randomRotation());
		obj->append(primitive);
	}
	obj->translate(randomTranslation(1.0));
	primitive->cartesianRotate(randomRotation());
	theModel.appendObject(obj);
	return obj;
}

QVector3D ObjectSoup::randomTranslation( float n )
{
	float m = n/2;
	return QVector3D( m*qrand()/RAND_MAX-m*qrand()/RAND_MAX,
					  m*qrand()/RAND_MAX-m*qrand()/RAND_MAX,
					  m*qrand()/RAND_MAX-m*qrand()/RAND_MAX );
}

QVector3D ObjectSoup::randomRotation()
{
	return QVector3D( M_PI*qrand()/RAND_MAX, M_PI*qrand()/RAND_MAX, M_PI*qrand()/RAND_MAX );
}
