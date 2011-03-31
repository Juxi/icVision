#include <qgl.h>
#include <qmath.h>

#include <SOLID/solid.h>
#include "cylinder.h"
#include "displmatrix.h"
#include "robotmodelexception.h"

using namespace RobotModel;

Cylinder::Cylinder( qreal aRadius, qreal aHeight, int lod ) : radius(aRadius), height(aHeight), lod(lod)
{
	// check validity of parameters
	if ( radius <= 0 ) { throw RobotModelException("Cylinder radius must be greater than 0."); }
	if ( height <= 0 ) { throw RobotModelException("Cylinder height must be greater than 0."); }
	
    // make a cylinder
	shape = dtCylinder( radius, height );
    dtCreateObject( static_cast<PrimitiveObject*>(this), shape );
    dtLoadIdentity();
}
Cylinder::~Cylinder()
{
	dtDeleteObject( this );
}

void Cylinder::makeDisplayList()
{
    int    numSegments = 2*(lod+3);
    double dt = 2*M_PI/numSegments;

    QVector3D yAxis = QVector3D(0,1,0),
              xAxis = QVector3D(1,0,0),
              pN,qN;

    QVector4D p0 = QVector4D( radius, -height/2, 0, 1 ),
              q0 = QVector4D( radius,  height/2, 0, 1 ),
			  n0 = QVector4D( radius, 0, 0, 1 );
	
	QVector4D *p,*q,*pn,*qn;
	
	p = new QVector4D[numSegments+1];
	q = new QVector4D[numSegments+1];
	pn = new QVector4D[numSegments+1];
	qn = new QVector4D[numSegments+1];

    DisplMatrix M,N;

    // get a unique display list index and define the list
    setDisplayListIdx(glGenLists(1));
	//printf("created robot-cylinder %d \n", glGenLists(1) );
	
    glNewList( displayListIdx(), GL_COMPILE );  // a cylinder expressed in a centroidal y-principal CS
	
        glBegin( GL_TRIANGLE_STRIP ); // body of cylinder
            for ( int t=0; t<=numSegments; t++ )
			{

				M.setAxisAngleOrientation(yAxis, t*dt);
                p[t] = M*p0;
                pn[t] = M*n0;
                pN = pn[t].toVector3D();
                pN.normalize();

				N.setAxisAngleOrientation(yAxis, (t+0.5)*dt);
                q[t] = N*q0;
                qn[t] = N*n0;
                qN = qn[t].toVector3D();
                qN.normalize();

                glNormal3d( pN.x(), pN.y(), pN.z() );
                glVertex3d( p[t].x(),  p[t].y(),  p[t].z() );
                glNormal3d( qN.x(), qN.y(), qN.z() );
                glVertex3d( q[t].x(),  q[t].y(),  q[t].z() );
            }
        glEnd();

        // bottom cap
        glBegin( GL_TRIANGLE_FAN );
            glNormal3d( 0, -1, 0 );
            glVertex3d( 0, -height/2, 0 );
            for ( int t=0; t<=numSegments; t++ )
			{
                glVertex3d( p[t].x(),  p[t].y(),  p[t].z() );
            }
        glEnd();

        // top cap
        glBegin( GL_TRIANGLE_FAN );
            glNormal3d( 0, 1, 0 );
            glVertex3d( 0, height/2, 0 );
            for ( int t=0; t<=numSegments; t++ )
			{
                glVertex3d( q[t].x(),  q[t].y(),  q[t].z() );
            }
        glEnd();
	
    glEndList();
	
	delete[] p;
	delete[] q;
	delete[] pn;
	delete[] qn;
}
