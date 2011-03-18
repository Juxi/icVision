#ifndef DISPLMATRIX_H
#define DISPLMATRIX_H

#include <QMatrix4x4>
#include <math.h>

namespace RobotModel {

class DisplMatrix : public QMatrix4x4
{
public:
    DisplMatrix();
	DisplMatrix( const QMatrix4x4& m );
	
	void setTranslation( const QVector3D& trans );
	void translate( const QVector3D& trans );
	
	void setCartesianRotation( const QVector3D& rot );
	void cartesianRotate( const QVector3D& rot );
	
	void setAxisAngleRotation( const QVector3D& axis, qreal angle );
	void axisAngleRotate( const QVector3D& axis, qreal angle );
	
	void specialRotate( const QVector3D& axis, qreal angle = 0 );
	
	void operator =( const QMatrix4x4& m ) { QMatrix4x4::operator=(m); }
};
	
}

#endif
