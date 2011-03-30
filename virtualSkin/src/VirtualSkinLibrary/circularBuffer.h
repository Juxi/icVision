/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QVector>

namespace VirtualSkin {

/**
 * A simple circular buffer of QVectors
 *
 *	In VirtualSkin, this is used to store the history of poses for a RobotModel::BodyPart. The history is written to the buffer by StateObserver.
 *	The outer QVector contains states of the RobotModel::BodyPart, and the inner QVector (each state) contains a tuple of motor encoder positions. 
 */
class CircularBuffer
{

public:
    CircularBuffer();		//!< Nothing special to do here
	~CircularBuffer();		//!< Nothing special to do here

	void setBufferSize( int bufferLength );	//!< Clear and then resize the buffer
	void init( const QVector<qreal> v);		//!< Fill the buffer with the QVector v
	void put( const QVector<qreal> v);		//!< Write the QVector v to the buffer and advance the 'current' position
	void next();							//!< Advances the 'current' position in the buffer
	
	QVector<qreal>& getOldest();			//!< Returns a reference to the oldest value in the buffer
	QVector<qreal>& getCurrent();			//!< Returns a reference to the current value

private:

	QVector< QVector<qreal> >::iterator i;	//!< Represents the 'current' position in the buffer
	QVector< QVector<qreal> > buffer;		//!< The buffer itself
};
}
#endif
/** @} */