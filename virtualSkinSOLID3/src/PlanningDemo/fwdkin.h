
#ifndef FwdKin_H
#define FwdKin_H


#include <model.h>

using namespace RobotModel;


class FwdKin : public QObject
{
	Q_OBJECT

public:
	FwdKin(bool visualize = false);
	virtual ~FwdKin();

	Model model;
	Robot& robot;
	World& world;

	void pose(const double* jointAngle, bool& collision, double* handpos, bool wait = false);

public slots:
	void obtainObservation(RobotObservation obs);

protected:

	bool m_hasObs;
	RobotObservation m_obs;
};


#endif
