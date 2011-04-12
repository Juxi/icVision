#include "circularBuffer.h"

using namespace VirtualSkin;

CircularBuffer::CircularBuffer()
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