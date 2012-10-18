
#ifndef MARKER_H
#define MARKER_H

#include "sphere.h"
namespace KinematicModel {


class CompositeObject;
class Motor;


class Marker
{
public:
	Marker(KinTreeNode* object, QString name) : m_object(object),
													tracerObject(NULL),
													m_name(name),
													d(0)
	{
	}

	inline KinTreeNode* node()
	{ return m_object; }
	inline const CompositeObject* tracer() const
	{ return tracerObject; }

	inline QString name() const
	{ return m_name; }
	
	inline CompositeObject* getTracerObject()
	{ return tracerObject; }
	
	inline void createTracer( DT_ResponseClass c, int num, double r, QColor qc )
	{
		d = 2*r;
		tracerObject = new CompositeObject(c,c); 
		tracerObject->persistent = true;
		tracerObject->setFreeColor(qc);
		
		int i;
		for (i=0; i<num; i++)
		{
			PrimitiveObject* p = new Sphere(r);
			tracerObject->appendPrimitive(p);
			if ( i > 0 ) qc.setAlphaF( 1 - (double)i/(double)num);
			p->setFreeColor(qc);
		}
		//printf("created %d balls\n",i);
	}
	
	inline void updateTracer()
	{
		const QVector<PrimitiveObject*>& primitives = tracerObject->data();
		
		qreal* newT = m_object->getT().data();
		qreal* oldT = (*primitives.begin())->getT().data();
		
		//printf("\n");
		//printf("a: %f, %f, %f\n",newT[12],newT[13],newT[14]);
		//printf("b: %f, %f, %f\n",oldT[12],oldT[13],oldT[14]);
		
		qreal	a = newT[12]-oldT[12],
				b = newT[13]-oldT[13],
				c = newT[14]-oldT[14];
		
		double dx = sqrt( a*a + b*b + c*c );
		
		if ( dx > d )
		{
			QVector<PrimitiveObject*>::const_iterator i;
			
			for ( i=primitives.end()-1; i!=primitives.begin(); --i )
				(*i)->setT( (*(i-1))->getT() );
			(*primitives.begin())->setT( m_object->getT() );
		}
	}

protected:
	KinTreeNode *m_object;
    CompositeObject *tracerObject;
	QString m_name;
	qreal d;
};


}
#endif
