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
	
/*! \brief A simple exception for the RobotModel namespace
 */
class RobotModel::RobotModelException
{
	
public:
	
	RobotModelException( const QString& msg = "" );	//!< Sets the error message
	~RobotModelException();							//!< Nothing special
	
	const char* what();								//!< Returns the error message

private:
	
	QString errStr;									//!< The error message
};

#endif

/** @} */
