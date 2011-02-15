#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "displaylist.h"
#include <QTime>

/*! \brief The QWidget responsible for rendering the scene with OpenGL (should be a member of Window)
 *
 * This class, as well as Window have been almost entirely borrowed from the QT HelloGL example. The only difference is that here
 * arbitrary OpenGL display lists can be rendered. This is intended to be done in conjunction with the class RenderList.
 *
 * To use this class:
 *	Simply construct a Window with a member GLWidget. Then use the signals and slots as follows:
 *	Call/activate the slot addDisplayList(DisplayList*) whenever a new Object is constructed that should be rendered.
 *	Call/activate the slot removeDisplayList(int) whenever such an object is deleted.
 *	Call/activate the slot update() whenever the scene should be redrawn, and the signal renderStuff() will be emitted. Clearly renderStuff()
 *	should be connected to some class that can call one or more display lists. The interface to such a class should look like that of RenderList.
 *
 * NOTE:
 *	This all may seem a bit convaluded, but the signal slot mechanism described above is necessary to ensure that display lists are created, destroyed
 *	and called by the same thread. This is necessary because OpenGL does not seem to be thread safe.
 */

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);	//!< Just initializes some colors and the camera rotation
    ~GLWidget();					//!< Nothing to clean up

	QSize minimumSizeHint() const;	//!< see HelloGL example
    QSize sizeHint() const;			//!< see HelloGL example

public slots:
	
	void addDisplayList( RobotModel::DisplayList* displayList );	//!< Calls DisplayList.makeDisplayList();
	void removeDisplayList( int idx );					//!< Calls glDeleteLists(int,1);
	void update();										//!< Calls updateGL();
	
    void setXRotation(int angle);	//!< see HelloGL example
    void setYRotation(int angle);	//!< see HelloGL example
    void setZRotation(int angle);	//!< see HelloGL example
	
signals:
	
	void renderStuff();					//!< Connect to RenderList.callLists() or anything that calls DisplayList.render()
    void xRotationChanged(int angle);	//!< see HelloGL example
    void yRotationChanged(int angle);	//!< see HelloGL example
    void zRotationChanged(int angle);	//!< see HelloGL example

protected:
	
    void initializeGL();						//!< see HelloGL example
    void paintGL();								//!< see HelloGL example
    void resizeGL(int width, int height);		//!< see HelloGL example
    void mousePressEvent(QMouseEvent *event);	//!< see HelloGL example
    void mouseMoveEvent(QMouseEvent *event);	//!< see HelloGL example

private:
	
	static void drawCS();				//!< Draws a right handed coordinate system at the origin of the OpenGL's 3D cartesian space. (mostly for troubleshooting) 
    void normalizeAngle(int *angle);	//!< see HelloGL example

    int xRot;				//!< see HelloGL example
    int yRot;				//!< see HelloGL example
    int zRot;				//!< see HelloGL example
    QPoint lastPos;			//!< see HelloGL example
    QColor trolltechPurple;	//!< see HelloGL example
	
	QTime timeSinceLastRender;
};

#endif
