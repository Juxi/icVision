#include "circularBuffer.h"

using namespace VirtualSkin;

CircularBuffer::CircularBuffer() : period(0)
{
}
CircularBuffer::~CircularBuffer()
{
}
void CircularBuffer::setBufferSize( int len )
{
	buffer.clear();
	buffer.resize(len);
	i=buffer.begin();
}
void CircularBuffer::put( const QVector<qreal> v )
{
	if (i)
	{
		*i = v;
		next();
		if ( i == buffer.end()-1 )
		{ 
			period = static_cast<qreal>(time.restart()) / static_cast<qreal>(buffer.size());
		}
	}
}
void CircularBuffer::next()
{
	if ( i + 1 == buffer.end() )
	{
		i = buffer.begin();
	}
	else ++i;
}
void CircularBuffer::init( const QVector<qreal> v )
{
	for ( i=buffer.begin(); i!=buffer.end(); ++i )
	{
		*i = v;
	}
	i=buffer.begin();
}
QVector<qreal>& CircularBuffer::getCurrent()
{
	if ( i == buffer.begin() )
	{
		return *(buffer.end()-1);
	}
	else
	{
		return *(i-1);
	}
}
QVector<qreal>& CircularBuffer::getOldest()
{
	return *i;
}
QVector< QVector<qreal> > CircularBuffer::getHistory()
{
	int count = 0;
	QVector< QVector<qreal> > history;
	QVector< QVector<qreal> >::iterator j = i-1;
	while ( count < buffer.size() )
	{
		history.append(*j);
		if ( j == buffer.begin() ) 
		{
			j = buffer.end()-1;
		}
		else --j;
		count++;
	}
	return history;
}