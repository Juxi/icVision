#include <QtGui>
#include <math.h>
#include <iostream>
#include "displaylist.h"
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
    qglClearColor(trolltechPurple.dark());

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);
    //glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition0[4] = { 0.5, 5.0, 7.0, 1.0 };
    //static GLfloat lightPosition1[4] = { 7.0, -5.0, 3.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
    //glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);

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

    // call the display lists
    QVector<DisplayList*>::iterator i;
    for ( i=listList.begin(); i!=listList.end(); ++i ) {
        (*i)->render();
    }
}

void GLWidget::drawCS()
{
    GLfloat red[] = {1,0,0,1};
    GLfloat green[] = {0,1,0,1};
    GLfloat blue[] = {0,0,1,1};

    glBegin(GL_LINES);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, red);
        glVertex3f(0,0,0); glVertex3f(1,0,0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, green);
        glVertex3f(0,0,0); glVertex3f(0,1,0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, blue);
        glVertex3f(0,0,0); glVertex3f(0,0,1);
    glEnd();
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

void GLWidget::createGLObject( DisplayList* object )
{
    object->makeDisplayList();

    // add this link to the list of links to be rendered
    listList.append( object );
    cout << "appended an object to GLlistList... size = " << listList.size() << endl;
}

void GLWidget::callUpdateGL() {
    //cout << "Calling updateGL() for " << objectList.size() << " objects." << endl;
    updateGL();
}
