/*
 *  gl_widget.h
 *  A Qt Widget to display QT code, able to render also CV Mat images and others
 *
 *  Created by Juxi Leitner on 3/15/11.
 *  Copyright: Juxi Leitner <juxi.leitner@gmail.com>, 2011
 *  based on the Aquila implementation = (C) 2011 Martin Peniak(www.martinpeniak.com)    
 */

#ifndef CV_WIDGET_H
#define CV_WIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <opencv/cv.h>
#include <yarp/sig/all.h>

/**
*@brief    CVWidget
*/
class CVWidget : public QGLWidget {
public:
    CVWidget();
	
    //methods
    void sendImage(cv::Mat *img, bool rgbSwapped);
    void sendImage(IplImage *img);
	void sendImage(QImage &qi);	
	void sendImage(const char *);		
	
	const QImage* getImage();

private:
    //variables
//    QImage orig_img;	// the orig image
    QImage qframe;	// the one displayed

protected:
    //methods
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
};

#endif // CV_WIDGET_H
