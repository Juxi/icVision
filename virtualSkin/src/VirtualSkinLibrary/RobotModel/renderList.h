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

class RobotModel::RenderList : public QObject
{
	Q_OBJECT
	
public:
	
	RenderList();
	virtual ~RenderList();
	
	virtual void render() = 0;

public slots:
	
	void callLists() { render(); }
	
signals:
	
	void appendedObject( RobotModel::DisplayList* list );
	void outdatedDisplayList( int idx );
	void changedState();
};

#endif

/** @} */
