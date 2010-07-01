#include "box.h"
#include "displmatrix.h"
#include <qgl.h>
#include <iostream>

using namespace std;

Box::Box( qreal x,
          qreal y,
          qreal z,
          const QVector3D& pos,
          const QVector3D& boxAxis,
          qreal angle ) : x(x), y(y), z(z)
{
    // make a box
    dtCreateObject( this, dtBox( x, y, z ) );
    dtLoadIdentity();

    // solid aligns the box with the y-axis
    QVector3D  rotAxis = QVector3D::crossProduct( QVector3D(0,1,0), boxAxis ).normalized();
    double rotAngle = acos( QVector3D::dotProduct( QVector3D(0,1,0), boxAxis.normalized() ) );



    QMatrix4x4 M = DisplMatrix(QVector3D(0,1,0),QVector3D(0,0,0),angle,0),
               N = DisplMatrix(rotAxis,pos,rotAngle,pos.length());

    // we want it somewhere else (still in the LinkCS)
    setL( N*M );
    //placeLocal(QMatrix4x4());
}
Box::~Box()
{
}
void Box::makeDisplayList() {
    double X=x/2, Y=y/2, Z=z/2, csLen=(x+y+z)/3;

    // get a unique display list index and define the list
    setDisplayListIdx(glGenLists(10000));  // (this number determines the max number of physical objects representable in OpenGL)

    glNewList( displayListIdx(), GL_COMPILE );  // a cylinder expressed in a centroidal y-principal CS
        glBegin( GL_QUADS );
            glNormal3d( 1, 0, 0 );
            glVertex3d( X,  Y,  Z );
            glVertex3d( X, -Y,  Z );
            glVertex3d( X, -Y, -Z );
            glVertex3d( X,  Y, -Z );
            glNormal3d( -1, 0, 0 );
            glVertex3d( -X,  Y,  Z );
            glVertex3d( -X,  Y, -Z );
            glVertex3d( -X, -Y, -Z );
            glVertex3d( -X, -Y,  Z );
            glNormal3d( 0, 1, 0 );
            glVertex3d(  X,  Y,  Z );
            glVertex3d( -X,  Y,  Z );
            glVertex3d( -X,  Y, -Z );
            glVertex3d(  X,  Y, -Z );
            glNormal3d( 0, -1, 0 );
            glVertex3d(  X, -Y,  Z );
            glVertex3d(  X, -Y, -Z );
            glVertex3d( -X, -Y, -Z );
            glVertex3d( -X, -Y,  Z );
            glNormal3d( 0, 0, 1 );
            glVertex3d(  X,  Y, Z );
            glVertex3d( -X,  Y, Z );
            glVertex3d( -X, -Y, Z );
            glVertex3d(  X, -Y, Z );
            glNormal3d( 0, 0, -1 );
            glVertex3d(  X,  Y, -Z );
            glVertex3d(  X, -Y, -Z );
            glVertex3d( -X, -Y, -Z );
            glVertex3d( -X,  Y, -Z );
        glEnd();

        /*GLfloat red[] = {1,0,0,1};
        GLfloat green[] = {0,1,0,1};
        GLfloat blue[] = {0,0,1,1};
        glBegin(GL_LINES);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, red);
            glVertex3f(0,0,0); glVertex3f(csLen,0,0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, green);
            glVertex3f(0,0,0); glVertex3f(0,csLen,0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, blue);
            glVertex3f(0,0,0); glVertex3f(0,0,csLen);
        glEnd();*/

    glEndList();
}
