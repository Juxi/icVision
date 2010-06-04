#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "link.h"

/*! \brief The QWidget responsible for holding the rendered OpenGL scene (see GLObject and GLObjectList)
 *
 * This code is almost entirely borrowed from the Qt HelloGL example. We have added several slots, through which
 * the Robot class can add to and update the GLObjects in the world (see GLObject and GLObjectList), as well as one
 * slot that causes the scene to be painted, allowing a timer thread to paint the scene repeatedly according to some
 * predetermined frame rate (see Timer)
 */

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0); // add number
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    //GLObjectList glObjectList;       // KAIL - objects to be rendered
    QVector<Link*> linkList;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    /*** KAIL ***/
    void createGLObject( Link* link );
    void callUpdateGL();                                                                               // call updateGL()

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void normalizeAngle(int *angle);
    void drawCS();

    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    QColor trolltechGreen;
    QColor trolltechPurple;

};

#endif
