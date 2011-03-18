#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include "displmatrix.h"
#include <qgl.h>
#include <QTime>


namespace RobotModel {

class DisplayList
{
public:
    DisplayList();
    virtual ~DisplayList();

    virtual void makeDisplayList() = 0;
	virtual void render();
	
	//int	getIdx() { return index; }
	const DisplMatrix& getT() { return T; }

    void setDisplayListIdx( int idx ) { index = idx; }
    int displayListIdx() { return index; }

    void setColliding() { timeSinceLastCollision.restart(); }
    bool isColliding() const;
    //void unSetColliding() { colliding = false; }
	
	virtual void setCartesianRotation( const QVector3D& rot ) { T.setCartesianRotation(rot); }
	virtual void cartesianRotate( const QVector3D& rot ) { T.cartesianRotate(rot); }
	virtual void setAxisAngleRotation( const QVector3D& axis, qreal angle ) { T.setAxisAngleRotation(axis, angle); }
	virtual void axisAngleRotate( const QVector3D& axis, qreal angle ) { T.axisAngleRotate(axis, angle); }
	virtual void specialRotate( const QVector3D& axis, qreal angle = 0 ) { T.specialRotate(axis, angle); }
	virtual void setTranslation( const QVector3D& trans ) { T.setTranslation(trans); }
	virtual void translate( const QVector3D& trans ) { T.translate(trans); }
	
protected:
	DisplMatrix T;
	
private:
    int  index;
    //bool colliding;
	QTime timeSinceLastCollision;
    
    static GLfloat red[4];
    static GLfloat gray[4];
};
	
}

#endif
