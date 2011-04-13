
#ifndef CONTROLTHREAD_H_
#define CONTROLTHREAD_H_

#include <QThread>
#include "fwdkin.h"


class ControlThread : public QThread
{
	Q_OBJECT

public:
	ControlThread(FwdKin& fwdkin, const double* coeff);

private:
	void run();

	FwdKin& m_kinematics;
	const double* m_coeff;
	unsigned int m_time;

	double m_j[101][7];			// joint angles
};


#endif
