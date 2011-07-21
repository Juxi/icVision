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
#include <QTime>

namespace VirtualSkin
{
	class CircularBuffer;
}

/** \brief A simple circular buffer of QVectors
 *
 *	In VirtualSkin, this is used to store the history of poses for a RobotModel::BodyPart. The history is written to the buffer by StateObserver.
 *	The outer QVector contains states of the RobotModel::BodyPart, and the inner QVector (each state) contains a tuple of motor encoder positions. 
 */
class VirtualSkin::CircularBuffer
{

public:
    CircularBuffer();		//!< Nothing special to do here
	~CircularBuffer();		//!< Nothing special to do here

	void setBufferSize( int bufferLength );	//!< Clear and then resize the buffer
	
	QVector< QVector<qreal> >::iterator next();		//!< Advances the 'current' position in the buffer
	QVector< QVector<qreal> >::iterator prev();		//!< Moves the 'current' position in the buffer backward
	void init( const QVector<qreal> v);				//!< Fill the buffer with the QVector v
	
	void put( const QVector<qreal> v);		//!< Write the QVector v to the buffer and advance the 'current' position
	
	QVector<qreal>& getOldest();			//!< Returns a reference to the oldest value in the buffer
	QVector<qreal>& getCurrent();			//!< Returns a reference to the current value
	QVector< QVector<qreal> > getHistory();	//!< Return the current contents of the buffer in reverse chronological order
	qreal getPeriod() { return period; }    //!< Returns the average time between writes to the buffer
											/**< This is recalculated each time we traverse the circular buffer, so it may be wacky if you use
												 a very short buffer, and it may take a long time to refresh if you use a really long buffer */

private:

	QVector< QVector<qreal> >::iterator i;	//!< Represents the 'current' position in the buffer
	QVector< QVector<qreal> > buffer;		//!< The buffer itself
	QTime time;
	qreal period;
	bool empty;
	
};
#endif
/** @} */