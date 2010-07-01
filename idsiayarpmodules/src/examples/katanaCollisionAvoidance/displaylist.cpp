#include "displaylist.h"
#include <iostream>

using namespace std;

DisplayList::DisplayList() : index(0), colliding(0)
{
     red[0] = 1.0;  red[1] = 0.0;  red[2] = 0.0;  red[3] = 1.0;
    gray[0] = 0.5; gray[1] = 0.5; gray[2] = 0.5; gray[3] = 1.0;
}
DisplayList::~DisplayList()
{
}

void DisplayList::render()
{
    //cout << "rendering display list " << index << endl;
    if ( isColliding() ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, red);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gray);
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, gray);
    }
    glPushMatrix();
        glMultMatrixd( getT().constData() );
        glCallList( index );
    glPopMatrix();
    unSetColliding();
}
