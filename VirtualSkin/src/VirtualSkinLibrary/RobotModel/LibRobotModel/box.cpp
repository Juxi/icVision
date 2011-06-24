#include <qgl.h>

#include <SOLID.h>
#include "box.h"
#include "robotmodelexception.h"

#include <QThread>

using namespace RobotModel;

GLfloat Box::black[4] =  { 0.0, 0.0, 0.0, 1.0 };

Box::Box( const QVector3D& size, bool lines ) : x(size.x()), y(size.y()), z(size.z()), withLines(lines)
{
	// check validity of parameters
	if ( x <= 0 || y <= 0 || z <= 0 ) { throw RobotModelException("Box must have non-zero dimensions height, width and depth."); }

    shape = DT_NewBox(x,y,z);
	solidObject = DT_CreateObject( static_cast<PrimitiveObject*>(this), shape);
}
Box::~Box()
{
}
void Box::makeDisplayList( )
{
	//printf("     make - %p\n",QThread::currentThread());
	
    double X=x/2, Y=y/2, Z=z/2;  //, csLen=(x+y+z)/3;

    // get a unique display list index and define the list
    index = glGenLists(1);  // (this number determines the max number of physical objects representable in OpenGL)

    glNewList( displayListIdx(), GL_COMPILE );  // a cylinder expressed in a centroidal y-principal CS
        glBegin( GL_QUADS );
            glNormal3d( 1, 0, 0 );
				glVertex3d( X,  Y,  Z );
				glVertex3d( X, -Y,  Z );
				glVertex3d( X, -Y, -Z );
				glVertex3d( X,  Y, -Z );
            glNormal3d( -1, 0, 0 );
				glVertex3d( -X, -Y,  Z );
				glVertex3d( -X, -Y, -Z );
				glVertex3d( -X,  Y, -Z );
				glVertex3d( -X,  Y,  Z );
            glNormal3d( 0, 1, 0 );
				glVertex3d(  X,  Y, -Z );
				glVertex3d( -X,  Y, -Z );
				glVertex3d( -X,  Y,  Z );
				glVertex3d(  X,  Y,  Z );
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
	
		if ( withLines )
		{
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, black);
			glBegin(GL_LINE_STRIP);
				glVertex3d( X,  Y,  Z );
				glVertex3d( X, -Y,  Z );
				glVertex3d( X, -Y, -Z );
				glVertex3d( X,  Y, -Z );
				glVertex3d( X,  Y,  Z );
			glEnd();
			glBegin(GL_LINE_STRIP);
				glVertex3d( -X, -Y,  Z );
				glVertex3d( -X, -Y, -Z );
				glVertex3d( -X,  Y, -Z );
				glVertex3d( -X,  Y,  Z );
				glVertex3d( -X, -Y,  Z );
			glEnd();
			glBegin(GL_LINES);
				glVertex3d(  X,  Y,  Z );
				glVertex3d( -X,  Y,  Z );
				glVertex3d(  X, -Y,  Z );
				glVertex3d( -X, -Y,  Z );
				glVertex3d(  X, -Y, -Z );
				glVertex3d( -X, -Y, -Z );
				glVertex3d(  X,  Y, -Z );
				glVertex3d( -X,  Y, -Z );
			glEnd();
		}
    glEndList();
}
