#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

class GLWidget;

/*! \brief This QWidget is the window for the optional GUI used to visualize the Robot. (see GLWidget)
 *
 * The code is almost entirely borrowed from the Qt HelloGL example.
 */

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

    //! Returns a pointer to the GLWidget used to render the Robot.
    /*! Useful for connecting signals and slots in the scope of main() */
    GLWidget* getGlWidget();

private:
    QSlider *createSlider();

    GLWidget *glWidget;
    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;
};

#endif
