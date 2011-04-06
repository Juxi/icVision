/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "displaylist.h"
#include <QTime>

namespace RobotModel { class GLWidget; }

/*! \brief The QWidget responsible for rendering the scene with OpenGL
 *
 * This class, as well as Window have been almost entirely borrowed from the Qt HelloGL example. The only difference is that here
 * arbitrary OpenGL display lists can be rendered. This is done in conjunction with the class RenderList.
 *
 * The signals and slots between RenderList and this class work as follows:
 *	SIGNAL RenderList::appendedObject( RobotModel::DisplayList* )	--->	SLOT GLWidget::addDisplayList(DisplayList*)
 *  SIGNAL RenderList::outdatedDisplayList( int idx )				--->	SLOT GLWidget::removeDisplayList(int)
 *  SIGNAL RenderList::changedState()								--->	SLOT GLWidget::update()
 *  SIGNAL GLWidget::renderStuff()									--->	SLOT RenderList::callLists()
 *
 * NOTE:
 *	The signal slot mechanism ensures that display lists are created, destroyed and called by the same thread.
 *  This is necessary because OpenGL is not thread safe.
 */

class RobotModel::GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);	//!< Initializes some colors and the camera position/rotation
    ~GLWidget();					//!< Nothing to clean up

	QSize minimumSizeHint() const;	//!< see HelloGL example
    QSize sizeHint() const;			//!< see HelloGL example

public slots:
	
	void addDisplayList( RobotModel::DisplayList* displayList );	//!< Calls DisplayList.makeDisplayList();
	void removeDisplayList( int idx );								//!< Calls glDeleteLists(int,1);
	void update();													//!< Calls updateGL();
	
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
	
	//QVector<RobotModel::DisplayList*> displayListList;
};

#endif

/** @} */
