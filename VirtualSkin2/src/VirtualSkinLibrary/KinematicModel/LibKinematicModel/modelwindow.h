/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef MODELWINDOW_H
#define MODELWINDOW_H

#include <QWidget>
#include <QGLWidget>

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

namespace KinematicModel
{
	class ModelWindow;
}

/*! \brief This QWidget is the window for the optional GUI used to visualize the Robot. (see GLWidget)
 */
class KinematicModel::ModelWindow : public QWidget
{
	Q_OBJECT

public:
	
	ModelWindow();			//!< Borrowed from the Qt HelloGL example.
	QGLWidget *glWidget;	//!< Customized to render arbitrary OpenGL display lists

};

#endif

/** @} */
