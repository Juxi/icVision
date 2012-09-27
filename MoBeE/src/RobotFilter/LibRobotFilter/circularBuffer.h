/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QMutex>
#include <QVector>
#include <QTime>

namespace MoBeE
{
	class CircularBuffer;
}

/** \brief A simple circular buffer of QVectors
 *
 *	In MoBeE, this is used to store the history of poses for a RobotModel::BodyPart. The history is written to the buffer by StateObserver.
 *	The outer QVector contains states of the RobotModel::BodyPart, and the inner QVector (each state) contains a tuple of motor encoder positions. 
 */
class MoBeE::CircularBuffer
{

public:
    CircularBuffer();		//!< Nothing special to do here
	~CircularBuffer();		//!< Nothing special to do here

	void setBufferSize( int bufferLength );	//!< Clear and then resize the buffer
	
	struct Item
	{
		int label;	//! 0-colliding, 1-safe, 2-waypoint
		QVector<qreal> value;
	};
	
	QVector< Item >::iterator next();		//!< Advances the 'current' position in the buffer
	QVector< Item >::iterator prev();		//!< Moves the 'current' position in the buffer backward
	
	void init( Item );				//!< Fill the buffer with the QVector v
	void put( Item );		//!< Write the QVector v to the buffer and advance the 'current' position
	
	/*void labelLastEntry( int anInt )
	{
		prev()->label = anInt;
		printf("labeled last entry: %d\n",  prev()->label );
	}*/
	void init( int anInt )
	{
		Item item = *prev();
		item.label = anInt;
		init(item);
	}
	
	Item& getOldest();			//!< Returns a reference to the oldest value in the buffer
	Item& getCurrent();			//!< Returns a reference to the current value
	QVector< Item > getHistory();	//!< Return the current contents of the buffer in reverse chronological order
	qreal getPeriod() { return period; }    //!< Returns the average time between writes to the buffer
											/**< This is recalculated each time we traverse the circular buffer, so it may be wacky if you use
												 a very short buffer, and it may take a long time to refresh if you use a really long buffer */

private:
	
	QMutex mutex;
	QVector< Item >::iterator i;	//!< Represents the 'current' position in the buffer
	QVector< Item > buffer;		//!< The buffer itself
	QTime time;
	qreal period;
	bool empty;
	
};
#endif
/** @} */