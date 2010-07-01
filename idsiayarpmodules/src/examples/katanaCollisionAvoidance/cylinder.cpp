#include "cylinder.h"
#include "displmatrix.h"
#include <qgl.h>
#include <iostream>

using namespace std;

Cylinder::Cylinder( qreal radius,
                    qreal height,
                    const QVector3D& pos,
                    const QVector3D& cylAxis,
                    int lod ) : radius(radius), height(height), lod(lod)
{

    // make a cylinder
    dtCreateObject( this, dtCylinder( radius, height ) );
    dtLoadIdentity();

    // solid aligns the cylinder with the y-axis
    QVector3D  axis = QVector3D::crossProduct( QVector3D(0,1,0), cylAxis ).normalized();
    double     angle = acos( QVector3D::dotProduct( QVector3D(0,1,0), cylAxis.normalized() ) );

    // we want it somewhere else (still in the LinkCS)
    //placeLocal( cylPos, axis, angle );
    setL( DisplMatrix(axis,pos,angle,pos.length()) );
}
Cylinder::~Cylinder()
{
}

void Cylinder::makeDisplayList() {

    int    numSegments = 2*(lod+3);
    double dt = 2*M_PI/numSegments;

    QVector3D yAxis = QVector3D(0,1,0),
              xAxis = QVector3D(1,0,0);

    QVector4D p0 = QVector4D( radius, -height/2, 0, 1 ),
              q0 = QVector4D( radius,  height/2, 0, 1 ),
              n0 = QVector4D( radius, 0, 0, 1 ),
              p[numSegments+1],
              q[numSegments+1],
              pn[numSegments+1],
              qn[numSegments+1];

    QMatrix4x4 M,N;

    // get a unique display list index and define the list
    setDisplayListIdx(glGenLists(10000));  // (this number determines the max number of physical objects representable in OpenGL)

    glNewList( displayListIdx(), GL_COMPILE );  // a cylinder expressed in a centroidal y-principal CS
        glBegin( GL_TRIANGLE_STRIP ); // body of cylinder
            for ( int t=0; t<=numSegments; t++ ) {
                M = DisplMatrix( yAxis, yAxis, t*dt, 0 );
                p[t] = M*p0;
                pn[t] = M*n0;
                pn[t].normalize();

                glNormal3d( pn[t].x(), pn[t].y(), pn[t].z() );
                glVertex3d( p[t].x(),  p[t].y(),  p[t].z() );

                N = DisplMatrix( yAxis, yAxis, (t+0.5)*dt, 0 );
                q[t] = N*q0;
                qn[t] = N*n0;
                qn[t].normalize();

                glNormal3d( qn[t].x(), qn[t].y(), qn[t].z() );
                glVertex3d( q[t].x(),  q[t].y(),  q[t].z() );
            }
        glEnd();

        // bottom cap
        glBegin( GL_TRIANGLE_FAN );
            glNormal3d( 0, -1, 0 );
            glVertex3d( 0, -height/2, 0 );
            for ( int t=0; t<=numSegments; t++ ) {
                glNormal3d( 0, -1, 0 );
                glVertex3d( p[t].x(),  p[t].y(),  p[t].z() );
            }
        glEnd();

        // top cap
        glBegin( GL_TRIANGLE_FAN );
            glNormal3d( 0, 1, 0 );
            glVertex3d( 0, height/2, 0 );
            for ( int t=0; t<=numSegments; t++ ) {
                glNormal3d( 0, 1, 0 );
                glVertex3d( q[t].x(),  q[t].y(),  q[t].z() );
            }
        glEnd();

    glEndList();
}
