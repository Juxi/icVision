#include <QtGui>
#include "glwidget.h"
#include "displmatrix.h"

using namespace RobotModel;

GLfloat GLWidget::lightPosition[3][4] = {  { 0.0, -1.0, 1.0, 0.0 },
{ 1.0, 0.0, 1.0, 0.0 },
{ 0.0, 1.0, 0.0, 0.0 } };
GLfloat GLWidget::lightColor[3][4] = { { 1.0, 1.0, 1.0, 1.0 },
{ 0.5, 0.5, 0.5, 1.0 },
{ 0.1, 0.1, 0.1, 1.0 } };

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{	
    xRot = 0;
    yRot = 0;
    zRot = 0;
	aspect = 16.0/9.0;
	zoom = 0.4;   // lower numbers are more zoomed in
    trolltechPurple = QColor::fromRgbF(0.9, 0.9, 0.9, 1.0);
	timeSinceLastRender.start();
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    qglClearColor(trolltechPurple);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);		
	glDepthFunc(GL_LEQUAL);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_BLEND);
	
	glAlphaFunc(GL_GREATER,0.1);
	glEnable(GL_ALPHA_TEST);						

	glFrontFace(GL_CW);
    glShadeModel(GL_SMOOTH);
	
	
	glEnable(GL_LIGHTING);
	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition[0]);
	glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, lightColor[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor[0]);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition[1]);
	glLightfv(GL_LIGHT1, GL_AMBIENT_AND_DIFFUSE, lightColor[1]);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightColor[1]);
	glEnable(GL_LIGHT1);
	

	
	//printf("*** INITIALIZE GL EXITED ***\n");
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslated(0.0, 0.10, -10.0);
	
	QMatrix4x4 flip;
	flip(1,1) = -1;
	glMultMatrixd( flip.constData() );
	
	RobotModel::DisplMatrix rot;
	rot.setCartesianOrientation( QVector3D(-M_PI/2, 0, M_PI/2) );
	glMultMatrixd( rot.constData() );
	
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
	
	//drawCS();
	emit renderStuff();
	//printf("paintGL exits\n");
}

void GLWidget::addDisplayList( RobotModel::DisplayList* thisList )
{
	//displayListList.append(thisList);
	if( thisList ) { thisList->makeDisplayList(); }
}

void GLWidget::removeDisplayList( int idx )
{
	//printf("\n REMOVING DISPLAY LIST %d !!!", idx);
	if (glIsList(idx)) { 
		glDeleteLists(idx,1);
	}
}

void GLWidget::resizeGL(int width, int height)
{
	int x,y;
	if ( width/height > aspect )
	{
		y = height;
		x = aspect*height;
	}
	else
	{
		x = width;
		y = width/aspect;
	}
	
    glViewport((width - x) / 2, (height - y) / 2, x, y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-aspect*zoom, +aspect*zoom, +zoom, -zoom, 1.0, 20.0);
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
        setYRotation(yRot - 8 * dy);
        setZRotation(zRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot - 8 * dy);
        setZRotation(zRot + 8 * dx);
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

QSize GLWidget::minimumSizeHint() const
{
	int y = 50;
	int x = static_cast<int> (y*aspect);
    return QSize(x, y);
}

QSize GLWidget::sizeHint() const
{
	int y = 400;
	int x = static_cast<int> (y*aspect);
    return QSize(x, y);
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


void GLWidget::update(int i)
{ 
	if ( timeSinceLastRender.elapsed() > 5 )
	{
		//printf("UPDATE GL - %p\n",QThread::currentThread());
		QGLWidget::updateGL();
	}
	
	timeSinceLastRender.restart();
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