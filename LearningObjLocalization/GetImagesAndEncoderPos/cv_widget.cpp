/*
 *  cv_widget.cpp
 *  A Qt Widget to display and render also CV Mat images and others
 *
 *  Created by Juxi Leitner on 3/15/11.
 *  Copyright: Juxi Leitner <juxi.leitner@gmail.com>, 2011
 *  based on the Aquila implementation = (C) 2011 Martin Peniak(www.martinpeniak.com)    
 */

#include "cv_widget.h"


/**
*@brief     constructor
*/
CVWidget::CVWidget() : QGLWidget(QGLFormat(QGL::SampleBuffers))
{
    setMinimumSize(320,240);
	setMaximumSize(320,240);
}

/**
*@brief     initialisation of OpenGL properties
*/
void CVWidget::initializeGL()
{
    glClearColor(0.0f,0.0f,0.0f,1.0f);
}

/**
*@brief     painting
*/
void CVWidget::paintGL()
{
    glClear (GL_COLOR_BUFFER_BIT);
    glClearColor (0.0,0.0,0.0,1.0);
    if (!qframe.isNull())
    {
        //you can use glDrawPixels directly by using
        //glDrawPixels(qframe.width(),qframe.height(), GL_RGBA, GL_UNSIGNED_BYTE, qframe.bits());
        //or do 2D texture mapping instead
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0,qframe.width(),qframe.height(),0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D( GL_TEXTURE_2D, 0, 4, qframe.width(), qframe.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, qframe.constBits() );
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(0,qframe.height());
        glTexCoord2f(0,1); glVertex2f(0,0);
        glTexCoord2f(1,1); glVertex2f(qframe.width(),0);
        glTexCoord2f(1,0); glVertex2f(qframe.width(),qframe.height());
        glEnd();
        glDisable(GL_TEXTURE_2D);
        //some example of alpha blending
        //glEnable(GL_DEPTH_TEST);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glColor4f(0.0f,1.0f,1.0f, 0.9f);
        glFlush();
    }
}

/**
*@brief     resizes viewport
*/
void CVWidget::resizeGL(int w, int h)
{
    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode (GL_MODELVIEW);
}

/**
*@brief     updates qframe with new data that will be rendered
*/
void CVWidget::sendImage(cv::Mat* img, bool rgbSwapped) {
    if(rgbSwapped) qframe = QImage((const unsigned char*)(img->data), img->cols, img->rows, img->step, QImage::Format_RGB888).rgbSwapped();
    else qframe = QImage((const unsigned char*)(img->data), img->cols, img->rows, img->step, QImage::Format_RGB888);
    QImage tmp = qframe;
    qframe = QGLWidget::convertToGLFormat(tmp);

	this->setFixedHeight(240);
	this->setFixedWidth(320);
//	this->setFixedHeight(img->rows);
//	this->setFixedWidth (img->cols);

    this->update();
}

void CVWidget::sendImage(QImage &qi) {
    qframe = QGLWidget::convertToGLFormat(qi);
    this->update();
}

void CVWidget::sendImage(IplImage *img) {
	cv::Mat m(img);
	sendImage(&m, false);
}

void CVWidget::sendImage(const char *file) {
	QImage tmp(file);
    qframe = QGLWidget::convertToGLFormat(tmp);
    this->update();
}

const QImage* CVWidget::getImage() {
	return &qframe;
}
