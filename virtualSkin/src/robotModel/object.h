#ifndef OBJECT_H
#define OBJECT_H

#include <QMatrix4x4>
#include "displaylist.h"

namespace RobotModel {

class PrimitiveObject;

class CompositeObject : public QVector<PrimitiveObject*>, public DisplayList
{
	
public:
	
    CompositeObject( const QString& aName = "unnamedObject" );
    virtual ~CompositeObject();
	
	void setName( const QString& aName ) { objectName = aName; }
	const QString& getName() { return objectName; }
	
	PrimitiveObject* newSphere( double r, const QVector3D& pos = QVector3D(0,0,0) );
	PrimitiveObject* newCylinder( double r, double h, const QVector3D& pos = QVector3D(0,0,0) );
	PrimitiveObject* newBox( const QVector3D& size, const QVector3D& pos = QVector3D(0,0,0) );
	
	virtual void append( PrimitiveObject* primitiveObject ); 
	PrimitiveObject* getPrimitiveByName( const QString& objectName ) const;
	
	virtual bool remove( PrimitiveObject* primitive );
	
    void setT( const QMatrix4x4& txfr );	
	void setCartesianRotation( const QVector3D& rot );
	void cartesianRotate( const QVector3D& rot );
	void setAxisAngleRotation( const QVector3D& axis, qreal angle );
	void axisAngleRotate( const QVector3D& axis, qreal angle );
	void specialRotate( const QVector3D& axis, qreal angle = 0 );
	void setTranslation( const QVector3D& trans );
	void translate( const QVector3D& trans );
	
	void makeDisplayList();
    virtual void render();
	
	void doNotCheckCollision( const CompositeObject* a ) const;
	void doNotCheckCollision( PrimitiveObject* a ) const;
	
    virtual void notColliding();
	virtual void update();

protected:
	
	QString objectName;
	int numSpheres,
		numCylinders,
		numBoxes;
	
};
	
}

#endif
