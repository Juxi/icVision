#include "cylinder.h"
#include "displmatrix.h"
#include <qgl.h>
#include <iostream>

using namespace std;

Cylinder::Cylinder() : radius(0), height(0) {}
Cylinder::Cylinder( const qreal& radius,
                    const qreal& height,
                    const QVector3D& cylPos,
                    const QVector3D& cylAxis ) : radius(radius), height(height)
{

    // make a cylinder
    dtCreateObject( &solidObject, dtCylinder( radius, height ) );
    dtLoadIdentity();

//cout << "---------------------------------------------------" << endl;
//cout << "Making SOLID cylinder: r=" << radius << " h=" << height << endl;
//cout << "---------------------------------------------------" << endl;

    // define the rotation axis and angle between the CylinderCS and LinkCS
    QVector3D  yAxis = QVector3D(0,1,0),
               rotationAxis = QVector3D::crossProduct( yAxis, cylAxis ).normalized();
    double rotationAngle = acos( QVector3D::dotProduct( yAxis, cylAxis.normalized() ) );

//cout << "Rotation Axis: [" << rotationAxis.x() << " " << rotationAxis.y() << " " << rotationAxis.z() << "]" << endl;
//cout << "Rotation Angle: " << rotationAngle << endl;
//cout << "Translation Axis: [" << cylPos.x() << " " << cylPos.y() << " " << cylPos.z() << "]" << endl;
//cout << "Translation Distance: " << cylPos.length() << endl;

    // define the transformation matrix ObjectCS to LinkCS
    txfrToLinkCS = DisplMatrix( rotationAxis, cylPos, rotationAngle, cylPos.length() );

//const qreal* t = txfrToLinkCS.constData();
//cout << "objToLink:        " << t[0] << " " << t[4] << " " << t[8]  << " " << t[12] << endl
//     << "                  " << t[1] << " " << t[5] << " " << t[9]  << " " << t[13] << endl
//     << "                  " << t[2] << " " << t[6] << " " << t[10] << " " << t[14] << endl
//     << "                  " << t[3] << " " << t[7] << " " << t[11] << " " << t[15] << endl;

    // define the transformation matrix ObjectCS to WorldCS
    txfrToWorldCS.setToIdentity();

//t = txfrToWorldCS.constData();
//cout << "objToWorld:        " << t[0] << " " << t[4] << " " << t[8]  << " " << t[12] << endl
//     << "                   " << t[1] << " " << t[5] << " " << t[9]  << " " << t[13] << endl
//     << "                   " << t[2] << " " << t[6] << " " << t[10] << " " << t[14] << endl
//     << "                   " << t[3] << " " << t[7] << " " << t[11] << " " << t[15] << endl;
}
Cylinder::~Cylinder(){}

void Cylinder::makeDisplayList() {

    int    lod = 12;
    double dt = 2*M_PI/lod;

    QVector3D yAxis = QVector3D(0,1,0),
              xAxis = QVector3D(1,0,0);

    QVector4D p0 = QVector4D( radius, -height/2, 0, 1 ),
              q0 = QVector4D( radius,  height/2, 0, 1 ),
              n0 = QVector4D( radius, 0, 0, 1 ),
              p,q,pn,qn;

    QMatrix4x4 M,N;

    // get a unique display list index and define the list
    displayListIdx = glGenLists(10000);  // (this number determines the max number of physical objects representable in OpenGL)
    glNewList( displayListIdx, GL_COMPILE );
        glBegin( GL_TRIANGLE_STRIP );
            // calculate the coordinates of the cylinder in the centroidal y-principal CS
            for ( int t=0; t<=lod; t++ ) {

                M = DisplMatrix( yAxis, yAxis, t*dt, 0 );
                p = M*p0;
                pn = M*n0;
                pn.normalize();

                N = DisplMatrix( yAxis, yAxis, (t+0.5)*dt, 0 );
                q = N*q0;
                qn = N*n0;
                qn.normalize();

                glVertex3d( p.x(),  p.y(),  p.z() );
                glNormal3d( pn.x(), pn.y(), pn.z() );

                glVertex3d( q.x(),  q.y(),  q.z() );
                glNormal3d( qn.x(), qn.y(), qn.z() );

            }
        glEnd();
    glEndList();
}
