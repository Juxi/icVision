#include "sphere.h"

using namespace KinematicModel;

float Sphere::X = 0.525731112119133606f;
float Sphere::Z = 0.850650808352039932f;
GLfloat Sphere::vdata[12][3] = {	{-X, 0, Z}, { X, 0,  Z}, {-X,  0, -Z}, { X,  0, -Z},
									{ 0, Z, X}, { 0, Z, -X}, { 0, -Z,  X}, { 0, -Z, -X},
									{ Z, X, 0}, {-Z, X,  0}, { Z, -X,  0}, {-Z, -X,  0}	};
GLint Sphere::tindices[20][3] = {	{0,4,1},  {0,9,4},  {9,5,4},  {4,5,8},  {4,8,1},
									{8,10,1}, {8,3,10}, {5,3,8},  {5,2,3},  {2,7,3},
									{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
									{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5},  {7,2,11}	};

Sphere::Sphere( qreal r, int d ) throw(KinematicModelException) : radius(r), lod(d)
{
	// check validity of parameters
	if ( radius <= 0 ) { throw KinematicModelException("Sphere radius must be greater than 0."); }
	geomType = SPHERE;
	solidShape = DT_NewSphere(radius);
	solidObject = DT_CreateObject(this, solidShape);
}
Sphere::~Sphere()
{	
}

GL_DisplayList Sphere::makeDisplayList()
{

    displayList = glGenLists(1);			// get a unique display list index and define the list
    glNewList( displayList, GL_COMPILE );  // a cylinder expressed in a centroidal y-principal CS

        for ( int i=0; i<20; i++ )
		{
            subdivide(&vdata[tindices[i][0]][0],
                      &vdata[tindices[i][1]][0],
                      &vdata[tindices[i][2]][0],
                      lod);
        }
    glEndList();
	
	//listPending = false;
	return displayList;
}

void Sphere::subdivide( float* v1, float* v2, float* v3, int depth )
{
    float v12[3], v23[3], v31[3];
    int i;

    if (depth == 0) {
            drawTriangle(v1, v2, v3);
            return;
    }
    for (i = 0; i < 3; i++) {
            v12[i] = v1[i]+v2[i];
            v23[i] = v2[i]+v3[i];
            v31[i] = v3[i]+v1[i];
    }
    normalize(v12);
    normalize(v23);
    normalize(v31);
    subdivide(v1, v12, v31, depth - 1);
    subdivide(v2, v23, v12, depth - 1);
    subdivide(v3, v31, v23, depth - 1);
    subdivide(v12, v23, v31, depth - 1);
}

void Sphere::normalize( float v[3] )
{
    float d = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    if (d != 0) {
        v[0] = v[0]/d;
        v[1] = v[1]/d;
        v[2] = v[2]/d;
    }
}

void Sphere::drawTriangle( float* v1, float* v2, float* v3 )
{
    glBegin(GL_TRIANGLES);
	glNormal3fv(v1); glVertex3f(radius*v1[0],radius*v1[1],radius*v1[2]);
	glNormal3fv(v2); glVertex3f(radius*v2[0],radius*v2[1],radius*v2[2]);
	glNormal3fv(v3); glVertex3f(radius*v3[0],radius*v3[1],radius*v3[2]);
    glEnd();
}

