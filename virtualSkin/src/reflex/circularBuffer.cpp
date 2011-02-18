#include "circularBuffer.h"

CircularBuffer::CircularBuffer()
{
}
CircularBuffer::~CircularBuffer()
{
}
void CircularBuffer::setBufferSize( int len )
{
	//mutex.lock();
		buffer.resize(len);
		i=buffer.begin();
	//mutex.unlock();
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
	//mutex.lock();
		if ( i + 1 == buffer.end() )
		{
			i = buffer.begin();
		}
		else ++i;
	//mutex.unlock();
}
void CircularBuffer::init( const QVector<qreal> v )
{
	//mutex.lock();
		for ( i=buffer.begin(); i!=buffer.end(); ++i )
		{
			*i = v;
		}
	//mutex.unlock();
	i=buffer.begin();
}
QVector<qreal>& CircularBuffer::getCurrent()
{
	//mutex.lock();
		if ( i == buffer.begin() )
		{
			return *(buffer.end()-1);
		}
		else
		{
			return *(i-1);
		}
	//mutex.unlock();
}
QVector<qreal>& CircularBuffer::getOldest()
{
	return *i;
}