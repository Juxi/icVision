#include "circularBuffer.h"
#include "constants.h"
using namespace VirtualSkin;

CircularBuffer::CircularBuffer() : period(static_cast<qreal>(YARP_PERIOD_ms)), empty(true)
{
}
CircularBuffer::~CircularBuffer()
{
}

QVector< QVector<qreal> >::iterator CircularBuffer::next()
{
	if ( i + 1 == buffer.end() ) { return buffer.begin(); }
	else { return i + 1; }
}

QVector< QVector<qreal> >::iterator CircularBuffer::prev()
{
	if ( i == buffer.begin() ) { return buffer.end() - 1; }
	else { return i - 1; }
}

void CircularBuffer::setBufferSize( int len )
{
	buffer.clear();
	buffer.resize(len);
	i=buffer.begin();
}
void CircularBuffer::put( const QVector<qreal> v )
{
	if (empty) { init(v); }
	else /*if ( time.elapsed() > 10 )*/ {
		*i = v;
		//QVector<qreal>::iterator j;
		//for ( j=(*i).begin(); j!=(*i).end(); ++j ) {
		//	printf("%f ",*j);
		//} printf("\n");
		i = next();
		period = (period + static_cast<qreal>(time.restart()))/2;
	}
}

void CircularBuffer::init( const QVector<qreal> v )
{
	for ( i=buffer.begin(); i!=buffer.end(); ++i ) { *i = v; }
	i=buffer.begin();
	period = static_cast<qreal>(YARP_PERIOD_ms);
	empty = false;
}

QVector<qreal>& CircularBuffer::getCurrent()
{
	return *prev();
}

QVector<qreal>& CircularBuffer::getOldest()
{
	return *i;
}

QVector< QVector<qreal> > CircularBuffer::getHistory()
{
	int count = 0;
	QVector< QVector<qreal> > history;
	QVector< QVector<qreal> >::iterator j = i;
	
	while ( count < buffer.size() )
	{
		if ( j == buffer.begin() ) { j = buffer.end() - 1; }
		else { --j; }
		history.append(*j);
		count++;
	}
	
	return history;
}