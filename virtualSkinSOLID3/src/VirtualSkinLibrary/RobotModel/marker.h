
#ifndef MARKER_H
#define MARKER_H


namespace RobotModel {


class CompositeObject;
class Motor;


class Marker
{
public:
	Marker(CompositeObject* object, QString name)
	: m_object(object),
	m_name(name)
	{ }

	inline CompositeObject* object()
	{ return m_object; }
	inline const CompositeObject* object() const
	{ return m_object; }

	inline QString name() const
	{ return m_name; }

protected:
	CompositeObject* m_object;
	QString m_name;
};


}
#endif
