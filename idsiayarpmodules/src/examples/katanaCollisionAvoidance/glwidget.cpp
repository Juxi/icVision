#include <math.h>
#include <iostream>
#include "glwidget.h"

using namespace std;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;

    trolltechGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    trolltechPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::initializeGL()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    qglClearColor(trolltechPurple.dark());
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);  // hides occluded faces (not always so good)
    //glColor3f(0.0,1.0,0.0);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -10.0);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

    // draw the WorldCS
    drawCS();

    // set default look of the robot
    GLfloat gray[] = {0.5,0.5,0.5,1.0};
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, gray);

    // call the display lists
    QVector<Link*>::iterator i;
    QVector<PhysObject*>::iterator j;

    for ( i=linkList.begin(); i!=linkList.end(); ++i ) {
        QVector<PhysObject*> list = (*i)->getPhysObjectList();
        for ( j=list.begin(); j!=list.end(); ++j ) {
            // if there is a collision change the color here
            glPushMatrix();
                glMultMatrixd( (*j)->txfrToWorld().data() );
                glCallList( (*j)->displayList() );
            glPopMatrix();
        }
    }
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot - 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot - 8 * dy);
        setZRotation(zRot - 8 * dx);
    }
    lastPos = event->pos();
}

void GLWidget::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}

void GLWidget::createGLObject( Link* link )
{
    // write the display list(s) for this link
    QVector<PhysObject*>::iterator j;
    QVector<PhysObject*> list = link->getPhysObjectList();
    for ( j=list.begin(); j!=list.end(); ++j ) {
        (*j)->makeDisplayList();
    }

    // add this link to the list of links to be rendered
    linkList.append( link );
}

void GLWidget::callUpdateGL() {
    //cout << "Calling updateGL();" << endl;
    updateGL();
}

void GLWidget::drawCS()
{
    GLfloat red[] = {1,0,0,1};
    GLfloat green[] = {0,1,0,1};
    GLfloat blue[] = {0,0,1,1};

    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, red);
    glBegin(GL_LINES);
        glVertex3f(0,0,0); glVertex3f(1,0,0);
    glEnd();
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, green);
    glBegin(GL_LINES);
        glVertex3f(0,0,0); glVertex3f(0,1,0);
    glEnd();
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, blue);
    glBegin(GL_LINES);
        glVertex3f(0,0,0); glVertex3f(0,0,1);
    glEnd();
}
