
#ifndef ROBOTOBSERVATION_H
#define ROBOTOBSERVATION_H


#include <QString>
#include <QMatrix4x4>


namespace RobotModel {


// forward declaration needed for friendship declaration
class Robot;


//!
//! \brief A RobotObservation object contains information on all "markers" placed on the robot.
//!
//! \par
//! The xml specification of the robot model allows for the definition of
//! markers attached to links. This class collects the resulting information
//! per pose in a single object.
//!
//! \par
//! The RobotObservation object does not hold references to the corresponding
//! marker objects; instead, it holds pairs of names and configurations.
//! The configuration is a 4x4 rigid body transformation matrix, encoding the
//! configuration in global "world" coordinates. This design keeps the
//! information in the RobotObservation object constant (and consistent),
//! even if the model itself undergoes further movements over time.
//!
class RobotObservation
{
public:
	RobotObservation() { }											//!< default constructor
	RobotObservation(RobotObservation const& rhs)					//!< copy constructor
	{ *this = rhs; }
	RobotObservation& operator = (RobotObservation const& rhs)		//!< assignment operator
	{
		m_markerName = rhs.m_markerName;
		m_markerConfiguration = rhs.m_markerConfiguration;
		return *this;												//!< windows isn't too smart 
	}

	inline unsigned int getNumMarkers() const						//!< return the number of part markers
	{ return m_markerConfiguration.size(); }
	inline QMatrix4x4 const& markerConfiguration(unsigned int index) const	//!< return the marker configuration given an index
	{ return m_markerConfiguration[index]; }
	inline QString markerName(unsigned int index) const				//!< return the part marker name given an index
	{ return m_markerName[index]; }

	friend class Robot;

protected:
	QVector<QString> m_markerName;								//!< list of part marker names
	QVector<QMatrix4x4> m_markerConfiguration;					//!< list of part marker configurations
};


}
#endif
