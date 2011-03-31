/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef RENDERLIST_H
#define RENDERLIST_H

#include <QObject>

namespace RobotModel
{
	class DisplayList;
	class RenderList;
}

/*! \brief An abstract class that provides the signal/slot interface between GLWidget and the (physical) objects to be rendered therein
 *
 * This class should be inherited by any class that contain a number of physical objects to be rendered. See Robot and World as examples. See
 * GLWidget for an explanation of how to connect the signals and slots.
 */
class RobotModel::RenderList : public QObject
{
	Q_OBJECT
	
public:
	
	RenderList();				//!< Nothing to do here
	virtual ~RenderList();		//!< Nothing to do here
	
	virtual void render() = 0;	//!< This should call display lists with glCallList( int )

public slots:
	
	void callLists() { render(); }	//!< Causes the display lists to be called
	
signals:
	
	void appendedObject( RobotModel::DisplayList* list );	//!< Indicates that a new physical object has been created, and that its corresponding display list should be created as well if necessary
	void outdatedDisplayList( int idx );					//!< Indicates that an object has been deleted and that its corresponding display list should be destroyed if it exists
	void changedState();									//!< Indicates that one or more objects (from the sub-class) have moved, and that the scene should be re-rendered as soon as is convenient
};

#endif

/** @} */
