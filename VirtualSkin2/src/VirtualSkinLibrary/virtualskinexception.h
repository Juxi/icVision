/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef VIRTUALSKINEXCEPTION_H
#define VIRTUALSKINEXCEPTION_H

#include <QString>
#include <exception>

namespace VirtualSkin { class VirtualSkinException; }
	
/*! \brief A simple exception for the RobotModel namespace
 */
class VirtualSkin::VirtualSkinException : public std::exception
{
	
public:
	
	VirtualSkinException( const QString& msg = "" );	//!< Sets the error message
	~VirtualSkinException () throw ();

	virtual const char* what() const throw();		//!< Returns the error message

private:
	
	QString errStr;									//!< The error message
};

#endif

/** @} */
