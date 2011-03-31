/*
 *  displmatrix.cpp
 *  robotModel
 *
 *  Created by Kail Frank on 9/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <qmath.h>

#include "displmatrix.h"

using namespace RobotModel;

void DisplMatrix::setPosition( const QVector3D& trans )
{
	operator()(0,3) = trans.x();
	operator()(1,3) = trans.y();
	operator()(2,3) = trans.z();
}
void DisplMatrix::translate( const QVector3D& trans )
{
	QMatrix4x4 translation;
	
	translation(0,3) = trans.x();
	translation(1,3) = trans.y();
	translation(2,3) = trans.z();
	
	*this = translation * (*this);
}
void DisplMatrix::setSpecialEulerOrientation( const QVector3D& axis, qreal angle )
{	
	QVector3D xAxis = QVector3D(1,0,0);
	QVector3D yAxis = QVector3D(0,1,0);
	
	setCartesianOrientation( QVector3D(0,0,0) );

	QVector3D xzProj = QVector3D(axis.x(),0,axis.z()).normalized();
	
	if ( !qFuzzyIsNull(xzProj.length()) )
	{
		qreal rotY = acos( QVector3D::dotProduct(xAxis,xzProj) );
		axisAngleRotate(yAxis,rotY);
	}
	
	QVector3D commonNorm = QVector3D::crossProduct( yAxis, axis );
	qreal rotCommonNorm = acos( QVector3D::dotProduct(yAxis,axis.normalized()) );
	axisAngleRotate(commonNorm, rotCommonNorm);
	
	axisAngleRotate(axis, angle);
}
/*void DisplMatrix::specialEulerRotate( const QVector3D& axis, qreal angle )
{	
	QMatrix4x4 rot;
	rot.setToIdentity();
	rot(0,0) = operator()(0,0);	rot(0,1) = operator()(0,1); rot(0,2) = operator()(0,2);
	rot(1,0) = operator()(1,0);	rot(1,1) = operator()(1,1); rot(1,2) = operator()(1,2);
	rot(2,0) = operator()(2,0);	rot(2,1) = operator()(2,1); rot(2,2) = operator()(2,2);
	
	// principal directions of the current transformation
	QVector3D xAxis = (rot*QVector3D(1,0,0)).normalized();	
	QVector3D yAxis = (rot*QVector3D(0,1,0)).normalized();
	QVector3D zAxis = (rot*QVector3D(0,0,1)).normalized();
	
	// projection of 'axis' onto the xz plane of the current transformation CS
	QVector3D xzProj = QVector3D( QVector3D::dotProduct(xAxis,axis), 0, QVector3D::dotProduct(zAxis,axis) ).normalized();
	
	if ( !qFuzzyIsNull(xzProj.length()) )
	{
		qreal rotY = acos( QVector3D::dotProduct(QVector3D(1,0,0),xzProj) );
		axisAngleRotate(yAxis,rotY);
	}
	
	QVector3D commonNorm = QVector3D::crossProduct( yAxis, axis );
	qreal rotCommonNorm = acos( QVector3D::dotProduct(yAxis,axis.normalized()) );
	axisAngleRotate(commonNorm, rotCommonNorm);
	
	axisAngleRotate(axis, angle);
}*/
void DisplMatrix::setCartesianOrientation( const QVector3D& angle )
{
	// rotates about z then y then x
	//QVector3D v = angle*M_PI/180;
	const qreal sG = sin( angle.x() ),  cG = cos( angle.x() ),
				sB = sin( angle.y() ),  cB = cos( angle.y() ),
				sA = sin( angle.z() ),  cA = cos( angle.z() );
	
	operator()(0,0) = cA*cB;			operator()(0,1) = -sA*cB;				operator()(0,2) =  sB;
	operator()(1,0) = sA*cG + cA*sB*sG;	operator()(1,1) =  cA*cG + sA*sB*sG;	operator()(1,2) =  -cB*sG;
	operator()(2,0) = sA*sG - cA*sB*cG;	operator()(2,1) =  cA*sG + sA*sB*cG;	operator()(2,2) =   cB*cG;
}
void DisplMatrix::cartesianRotate( const QVector3D& angle )
{
	// rotates about z then y then x
	//QVector3D v = angle*M_PI/180;
	const qreal sG = sin( angle.x() ),  cG = cos( angle.x() ),
				sB = sin( angle.y() ),  cB = cos( angle.y() ),
				sA = sin( angle.z() ),  cA = cos( angle.z() );
	
	QMatrix4x4 rot;
	
	rot(0,0) = cA*cB;				rot(0,1) = -sA*cB;				rot(0,2) =  sB;
	rot(1,0) = sA*cG + cA*sB*sG;	rot(1,1) =  cA*cG + sA*sB*sG;	rot(1,2) =  -cB*sG;
	rot(2,0) = sA*sG - cA*sB*cG;	rot(2,1) =  cA*sG + sA*sB*cG;	rot(2,2) =   cB*cG;
	
	*this = rot * (*this);
}
void DisplMatrix::setAxisAngleOrientation( const QVector3D& anAxis, qreal angle )
{
	// rotates 'angle' about 'axis'
	const qreal Cos=cos( angle ),
				Sin=sin( angle ),
				Vers=1-Cos;
	
	QVector3D axis = anAxis.normalized();
	
	operator()(0,0) = axis.x()*axis.x()*Vers + Cos;
	operator()(0,1) = axis.x()*axis.y()*Vers - axis.z()*Sin;
	operator()(0,2) = axis.x()*axis.z()*Vers + axis.y()*Sin;
	
	operator()(1,0) = axis.x()*axis.y()*Vers + axis.z()*Sin;
	operator()(1,1) = axis.y()*axis.y()*Vers + Cos;
	operator()(1,2) = axis.y()*axis.z()*Vers - axis.x()*Sin;
	
	operator()(2,0) = axis.x()*axis.z()*Vers - axis.y()*Sin;
	operator()(2,1) = axis.y()*axis.z()*Vers + axis.x()*Sin;
	operator()(2,2) = axis.z()*axis.z()*Vers + Cos;
}
void DisplMatrix::axisAngleRotate( const QVector3D& anAxis, qreal angle )
{
	// rotates 'angle' about 'axis'
	const qreal Cos=cos( angle ),
	Sin=sin( angle ),
	Vers=1-Cos;
	
	QVector3D axis = anAxis.normalized();
	
	QMatrix4x4 rot;
	
	rot(0,0) = axis.x()*axis.x()*Vers + Cos;
	rot(0,1) = axis.x()*axis.y()*Vers - axis.z()*Sin;
	rot(0,2) = axis.x()*axis.z()*Vers + axis.y()*Sin;
	
	rot(1,0) = axis.x()*axis.y()*Vers + axis.z()*Sin;
	rot(1,1) = axis.y()*axis.y()*Vers + Cos;
	rot(1,2) = axis.y()*axis.z()*Vers - axis.x()*Sin;
	
	rot(2,0) = axis.x()*axis.z()*Vers - axis.y()*Sin;
	rot(2,1) = axis.y()*axis.z()*Vers + axis.x()*Sin;
	rot(2,2) = axis.z()*axis.z()*Vers + Cos;
	
	*this = rot * (*this);
}