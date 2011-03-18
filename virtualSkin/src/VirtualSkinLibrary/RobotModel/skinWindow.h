#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QGLWidget>
#include "world.h"

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

namespace RobotModel {
	
	class DisplayList;
	class Robot;

/*! \brief This QWidget is the window for the optional GUI used to visualize the Robot. (see GLWidget)
 *
 * The code is mostly borrowed from the Qt HelloGL example.
 */

class SkinWindow : public QWidget
{
    Q_OBJECT

public:
    SkinWindow();
	
	QGLWidget *glWidget;

};
}
#endif
