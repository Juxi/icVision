/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef ROBOTMODELEXCEPTION_H
#define ROBOTMODELEXCEPTION_H

#include <QString>

namespace RobotModel { class RobotModelException; }
	
class RobotModel::RobotModelException
{
	
public:
	
	RobotModelException( const QString& msg = "" );
	~RobotModelException();
	
	const char* what();

private:
	
	QString errStr;
};

#endif

/** @} */
