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

DisplMatrix::DisplMatrix()
{
}
DisplMatrix::DisplMatrix( const QMatrix4x4& m) : QMatrix4x4(m)
{
}
void DisplMatrix::setTranslation( const QVector3D& trans )
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
void DisplMatrix::specialRotate( const QVector3D& axis, qreal angle )
{	
	QVector3D xAxis = QVector3D(1,0,0);
	QVector3D yAxis = QVector3D(0,1,0);

	QVector3D xzProj = QVector3D(axis.x(),0,axis.y()).normalized();
	
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
void DisplMatrix::setCartesianRotation( const QVector3D& angle )
{
	// rotates about z then y then x
	QVector3D v = angle*M_PI/180;
	const qreal sG = sin( v.x() ),  cG = cos( v.x() ),
				sB = sin( v.y() ),  cB = cos( v.y() ),
				sA = sin( v.z() ),  cA = cos( v.z() );
	
	operator()(0,0) = cA*cB;			operator()(0,1) = -sA*cB;				operator()(0,2) =  sB;
	operator()(1,0) = sA*cG + cA*sB*sG;	operator()(1,1) =  cA*cG + sA*sB*sG;	operator()(1,2) =  -cB*sG;
	operator()(2,0) = sA*sG - cA*sB*cG;	operator()(2,1) =  cA*sG + sA*sB*cG;	operator()(2,2) =   cB*cG;
}
void DisplMatrix::cartesianRotate( const QVector3D& angle )
{
	// rotates about z then y then x
	QVector3D v = angle*M_PI/180;
	const qreal sG = sin( v.x() ),  cG = cos( v.x() ),
				sB = sin( v.y() ),  cB = cos( v.y() ),
				sA = sin( v.z() ),  cA = cos( v.z() );
	
	QMatrix4x4 rot;
	
	rot(0,0) = cA*cB;				rot(0,1) = -sA*cB;				rot(0,2) =  sB;
	rot(1,0) = sA*cG + cA*sB*sG;	rot(1,1) =  cA*cG + sA*sB*sG;	rot(1,2) =  -cB*sG;
	rot(2,0) = sA*sG - cA*sB*cG;	rot(2,1) =  cA*sG + sA*sB*cG;	rot(2,2) =   cB*cG;
	
	*this = rot * (*this);
}
void DisplMatrix::setAxisAngleRotation( const QVector3D& anAxis, qreal angle )
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