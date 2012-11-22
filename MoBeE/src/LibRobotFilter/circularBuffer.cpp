#include "circularBuffer.h"
#include "constants.h"

using namespace MoBeE;

CircularBuffer::CircularBuffer() : period(static_cast<qreal>(YARP_PERIOD_ms)), empty(true)
{
}
CircularBuffer::~CircularBuffer()
{
}

void CircularBuffer::setBufferSize( int len )
{
	//printf("called set buffer size\n");
	mutex.lock();
		//printf("clear\n");
		buffer.clear();
		//printf("resize\n");
		buffer.resize(len);
		//printf("set i\n");
		i=buffer.begin();
	mutex.unlock();
}

void CircularBuffer::put( Item v )
{
	//printf("called put\n");
	if (empty)
	{ 
		init( v ); 
		return;
	}

	mutex.lock();
		*i = v;
		i = next();
		period = (period + static_cast<qreal>(time.restart()))/2;
	mutex.unlock();
}

void CircularBuffer::init( Item v )
{
	//printf("called circular buffer init\n");
	mutex.lock();
		for ( i=buffer.begin(); i!=buffer.end(); ++i ) { *i = v; }
		i=buffer.begin();
		period = static_cast<qreal>(YARP_PERIOD_ms);
		empty = false;
	mutex.unlock();
	//printf("done init-ing\n");
}

QVector< CircularBuffer::Item > CircularBuffer::getHistory()
{
	int count = 0;
	QVector< Item > history;
	
	mutex.lock();
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