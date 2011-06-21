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
#include <exception>

namespace RobotModel { class RobotModelException; }
	
/*! \brief A simple exception for the RobotModel namespace
 */
class RobotModel::RobotModelException : public std::exception
{
	
public:
	
	RobotModelException( const QString& msg = "" );	//!< Sets the error message
	~RobotModelException () throw ();

	virtual const char* what() const throw();		//!< Returns the error message

private:
	
	QString errStr;									//!< The error message
};

#endif

/** @} */
