/*
 *  displmatrix.cpp
 *  robotModel
 *
 *  Created by Kail Frank on 9/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <qmath.h>
#include <transformable.h>

using namespace KinematicModel;

void Transformable::setPosition( const QVector3D& trans )
{
	T(0,3) = trans.x();
	T(1,3) = trans.y();
	T(2,3) = trans.z();
	//updateChildren();
}

void Transformable::translate( const QVector3D& trans )
{
	QMatrix4x4 translation;
	
	translation(0,3) = trans.x();
	translation(1,3) = trans.y();
	translation(2,3) = trans.z();
	
	T = translation * (T);
	//updateChildren();
}

void Transformable::setCartesianOrientation( const QVector3D& angle )
{
	// rotates about z then y then x
	//QVector3D v = angle*M_PI/180;
	const qreal sG = sin( angle.x() ),  cG = cos( angle.x() ),
				sB = sin( angle.y() ),  cB = cos( angle.y() ),
				sA = sin( angle.z() ),  cA = cos( angle.z() );

	T(0,0) = cA*cB;				T(0,1) = -sA*cB;				T(0,2) =  sB;
	T(1,0) = sA*cG + cA*sB*sG;	T(1,1) =  cA*cG - sA*sB*sG;		T(1,2) =  -cB*sG;
	T(2,0) = sA*sG - cA*sB*cG;	T(2,1) =  cA*sG + sA*sB*cG;		T(2,2) =   cB*cG;
	//updateChildren();
}

void Transformable::cartesianRotate( const QVector3D& angle )
{
	// rotates about z then y then x
	//QVector3D v = angle*M_PI/180;
	const qreal sG = sin( angle.x() ),  cG = cos( angle.x() ),
				sB = sin( angle.y() ),  cB = cos( angle.y() ),
				sA = sin( angle.z() ),  cA = cos( angle.z() );
	
	QMatrix4x4 rot;
	
	rot(0,0) = cA*cB;				rot(0,1) = -sA*cB;				rot(0,2) =  sB;
	rot(1,0) = sA*cG + cA*sB*sG;	rot(1,1) =  cA*cG - sA*sB*sG;	rot(1,2) =  -cB*sG;
	rot(2,0) = sA*sG - cA*sB*cG;	rot(2,1) =  cA*sG + sA*sB*cG;	rot(2,2) =   cB*cG;
	
	T = rot * (T);
	//updateChildren();
}

void Transformable::setAxisAngleOrientation( const QVector3D& anAxis, qreal angle )
{
	// rotates 'angle' about 'axis'
	const qreal Cos=cos( angle ),
				Sin=sin( angle ),
				Vers=1-Cos;
	
	QVector3D axis = anAxis.normalized();
	
	T(0,0) = axis.x()*axis.x()*Vers + Cos;
	T(0,1) = axis.x()*axis.y()*Vers - axis.z()*Sin;
	T(0,2) = axis.x()*axis.z()*Vers + axis.y()*Sin;
	
	T(1,0) = axis.x()*axis.y()*Vers + axis.z()*Sin;
	T(1,1) = axis.y()*axis.y()*Vers + Cos;
	T(1,2) = axis.y()*axis.z()*Vers - axis.x()*Sin;
	
	T(2,0) = axis.x()*axis.z()*Vers - axis.y()*Sin;
	T(2,1) = axis.y()*axis.z()*Vers + axis.x()*Sin;
	T(2,2) = axis.z()*axis.z()*Vers + Cos;
	//updateChildren();
}

void Transformable::axisAngleRotate( const QVector3D& anAxis, qreal angle )
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
	
	T = rot * (T);
	//updateChildren();
}

void Transformable::setSpecialEulerOrientation( const QVector3D& axis, qreal angle )
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
	//updateChildren();
}