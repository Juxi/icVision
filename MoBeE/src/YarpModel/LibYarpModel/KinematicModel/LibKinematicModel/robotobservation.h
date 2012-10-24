
#ifndef ROBOTOBSERVATION_H
#define ROBOTOBSERVATION_H


#include <QString>
#include <QMatrix4x4>
#include <cassert>
#include <sstream>
namespace KinematicModel {


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
    typedef QList< QPair<QVector3D, QVector3D> > Jacobian;
    
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
    
    inline Jacobian const& markerJacobian(unsigned int index) const	//!< return the marker configuration given an index
	{ return m_markerJacobian[index]; }
	
	inline QString markerName(unsigned int index) const				//!< return the part marker name given an index
	{ return m_markerName[index]; }

	inline QMatrix4x4 const &markerConfiguration(QString &name) {
		for (size_t i(0); i < m_markerName.size(); ++i) {
			if (m_markerName[i] == name)
				return m_markerConfiguration[i];
		}
		std::ostringstream oss;
		oss << "marker name [" << name.toStdString() << "] not found";
		throw KinematicModelException(oss.str().c_str());
	}

	inline std::vector<double> markerPosition(QString name) {
		QMatrix4x4 configuration(markerConfiguration(name));
		std::vector<double> position(3);
		position[0] = configuration.data()[12];
		position[1] = configuration.data()[13];
		position[2] = configuration.data()[14];
		return position;
	}

	double orientationMeasure(QString name, std::vector<double> &goal_orientation, std::vector<double> &mask_orientation) {
		assert(goal_orientation.size() == 9);
		QMatrix4x4 const &observation(markerConfiguration(name));

		double measure(0.0), norm1(0.0), norm2(0.0);
		int orientation_index(0);
		for (size_t i(0); i < goal_orientation.size(); ++i, ++orientation_index) {
			if (orientation_index % 4 == 3) ++orientation_index;
			double val1(observation.data()[orientation_index]);
			double val2(goal_orientation[i]);
			measure += val1 * val2 * mask_orientation[i];
			norm1 += val1 * val1 * mask_orientation[i];
			norm2 += val2 * val2 * mask_orientation[i];
		}
		return (measure / sqrt(norm1) / sqrt(norm2) + 1.0) / 2.0;
	}
    
	friend class Robot;

protected:
	QVector<QString> m_markerName;								//!< list of part marker names
	QVector<QMatrix4x4> m_markerConfiguration;					//!< list of part marker configurations
    
    QVector< Jacobian > m_markerJacobian;
};


}
#endif
