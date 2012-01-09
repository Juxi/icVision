/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef KINEMATICMODELEXCEPTION_H
#define KINEMATICMODELEXCEPTION_H

#include <QString>
#include <exception>

namespace KinematicModel { class KinematicModelException; }
	
/*! \brief A simple exception for the KinematicModel namespace
 */
class KinematicModel::KinematicModelException : public std::exception
{
	
public:
	
	KinematicModelException( const QString& msg = "" );	//!< Sets the error message
	~KinematicModelException () throw ();

	virtual const char* what() const throw();		//!< Returns the error message

private:
	
	QString errStr;									//!< The error message
};

#endif

/** @} */
