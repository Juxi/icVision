/*******************************************************************
 ***              Copyright (C) 2010 Gregor Kaufmann             ***
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef CALLOBSERVER_H_
#define CALLOBSERVER_H_

#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include "responseObserver.h"

/** \brief Contains classes relevant to the virtual skin static library.
 *
 * The CMake project VirtualSkin in YOUR_GIT_REPOSITORY/virtualSkin/src/VirtualSkinLibrary can be built independantly to produce the static library libVirtualSkin.a. The project requires:
 *		- RobotModel (libRobotModel.a built from the source in ./RobotModel)
 *			- Qt (4.x)
 *			- OpenGL
 *			- FreeSOLID
 *		- YarpFilter (libYarpFilter.a built from source in ./YarpFilter)
 *			-YARP
 *			-ACE
 */
namespace VirtualSkin
{	
	class RobotFilter;
	class CallObserver;
}

/** \brief An IObserver implemented to monitor RPC calls
 */
class VirtualSkin::CallObserver : public yarp::os::IObserver {

public:
	CallObserver(RobotFilter* r, const int b);			//!< Nothing special to do here
														/**< \param r The RobotFilter to which this Observer belongs
															 \param b The index of the RobotModel::BodyPart to which this Observer applies */
	virtual ~CallObserver();							//!< Nothing special to do here

	virtual void onDataObserved(yarp::os::Bottle &b);	//!< The handler function (called whenever an RPC call passes through the filter)

	void setResponseObsever(ResponseObserver *o);		//!< Sets the Observer for RPC responses

private:
	RobotFilter			*robotFilter;		//!< Indicates the RobotFilter to which this Observer belongs
	const int			bodyPart;			//!< Indicates the RobotModel::BodyPart to which this Observer applies
	ResponseObserver	*responseObserver;	//!< An IObserver implemented to monitor RPC responses
	
};
#endif
/** @} */