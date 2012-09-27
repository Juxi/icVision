/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef MoBeEEXCEPTION_H
#define MoBeEEXCEPTION_H

#include <QString>
#include <exception>

namespace MoBeE { class MoBeEException; }
	
/*! \brief A simple exception for the RobotModel namespace
 */
class MoBeE::MoBeEException : public std::exception
{
	
public:
	
	MoBeEException( const QString& msg = "" );	//!< Sets the error message
	~MoBeEException () throw ();

	virtual const char* what() const throw();		//!< Returns the error message

private:
	
	QString errStr;									//!< The error message
};

#endif

/** @} */
