#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

#include <QMutex>
#include <QMatrix4x4>
#include "solid.h"
#include "displaylist.h"

class CompositeObject;

class PrimitiveObject : public DisplayList
{
	
public:
	
    PrimitiveObject();
    virtual ~PrimitiveObject();
	
	void setName( const QString& aName ) { name = aName; }
	const QString& getName() { return name; }

    void setParent( CompositeObject* object );
	CompositeObject* getParent() { return parent; }
	
    void setL( const QMatrix4x4& txfr );
    void update( const QMatrix4x4& txfr );
	
	void setCartesianRotation( const QVector3D& rot );
	void cartesianRotate( const QVector3D& rot );
	void setAxisAngleRotation( const QVector3D& axis, qreal angle );
	void axisAngleRotate( const QVector3D& axis, qreal angle );
	void specialRotate( const QVector3D& axis, qreal angle = 0 );
	void setTranslation( const QVector3D& trans );
	void translate( const QVector3D& trans );

protected:
	
	QString		name;
    CompositeObject*     parent;
	DtShapeRef	shape;
    DisplMatrix L;      // transformation from the PrimitiveObjectCS to the ObjectCS
	QMutex mutex;

    void doNotCollideWith( CompositeObject* object );
};

#endif // PHYSOBJECT_H
