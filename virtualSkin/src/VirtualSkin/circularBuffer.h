#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QMutex>
#include <QVector>

namespace VirtualSkin {

class CircularBuffer
{

public:
    CircularBuffer();
	~CircularBuffer();

	void setBufferSize( int bufferLength );
	void init( const QVector<qreal> ); // fills the buffer with only the current position
	void put( const QVector<qreal> );
	void next();
	
	QVector<qreal>& getOldest();
	QVector<qreal>& getCurrent();

private:

	QVector< QVector<qreal> >::iterator i;
	QVector< QVector<qreal> > buffer;
	//QMutex mutex;
};
}
#endif
