#include "circularBuffer.h"
#include "constants.h"
using namespace VirtualSkin;

CircularBuffer::CircularBuffer() : period(static_cast<qreal>(YARP_PERIOD_ms)), empty(true)
{
}
CircularBuffer::~CircularBuffer()
{
}

void CircularBuffer::setBufferSize( int len )
{
	mutex.lock();
		buffer.clear();
		buffer.resize(len);
		i=buffer.begin();
	mutex.lock();
}

void CircularBuffer::put( Item v )
{
	mutex.lock();
		if (empty) { init( v ); }
		else
		{
			*i = v;
			i = next();
			period = (period + static_cast<qreal>(time.restart()))/2;
		}
	mutex.unlock();
}

void CircularBuffer::init( Item v )
{
	mutex.lock();
		for ( i=buffer.begin(); i!=buffer.end(); ++i ) { *i = v; }
		i=buffer.begin();
		period = static_cast<qreal>(YARP_PERIOD_ms);
		empty = false;
	mutex.unlock();
}

QVector< CircularBuffer::Item > CircularBuffer::getHistory()
{
	mutex.lock();
		int count = 0;
		QVector< Item > history;
		QVector< Item >::iterator j = i;
		
		while ( count < buffer.size() )
		{
			if ( j == buffer.begin() ) { j = buffer.end() - 1; }
			else { --j; }
			
			history.append(*j);
			count++;
		}
	mutex.unlock();
	
	return history;
}

CircularBuffer::Item& CircularBuffer::getCurrent()
{
	return *prev();
}

CircularBuffer::Item& CircularBuffer::getOldest()
{
	return *i;
}

QVector< CircularBuffer::Item >::iterator CircularBuffer::next()
{
	if ( i + 1 == buffer.end() ) { return buffer.begin(); }
	else { return i + 1; }
}

QVector< CircularBuffer::Item >::iterator CircularBuffer::prev()
{
	if ( i == buffer.begin() ) { return buffer.end() - 1; }
	else { return i - 1; }
}