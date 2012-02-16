#ifndef __MAP_CREATOR_H__
#define __MAP_CREATOR_H__

#include "simulator.h"

class MapCreator {
	Simulator &d_simulator;

public:
	MapCreator(Simulator &simulator) : d_simulator(simulator){}

	void update_points(vector<double> best_point) {
		KinematicModel::RobotObservation observation = d_simulator.theRobot.observe();
		vector<double> position = observation.markerPosition(QString("right_hand"));
		d_configuration_points.push_back(best_point);
		d_points.push_back(position);
		d_last_position = position;
		add_point(position[0], position[1], position[2]);
		cout << "n points: " << d_points.size() << endl;
		for (size_t i(0); i < position.size(); ++i)
			cout << position[i] << " ";
		cout << endl;
	}
};

#endif
