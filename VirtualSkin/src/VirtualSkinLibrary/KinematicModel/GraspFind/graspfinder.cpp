#include "graspfinder.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <fstream>

#include "constraints.h"
#include "posefitnessfunction.h"

using namespace std;

vector<double> GraspFinder::get_best_point() {
	return d_best_point;
}

void GraspFinder::find_pose(unsigned int maxevals, double fitness_threshold, double std, int population_size) {
	try
	{
		// Experiment setup:
		// (*) dimension of sphere function
		// (*) number of function evaluations
		// (*) number of trials
		unsigned int dim = d_total_motors;
		//unsigned int maxevals = 30000;
		bool useImportanceMixing = false;
		bool useBaseline = false;

		PoseFitnessFunction f(this);

		Matrix pt = Matrix::zeros(dim);
		for (size_t i(0); i < d_home_pos.size(); ++i)
		  pt[i] = d_home_pos[i];

		Matrix sigma = Matrix::ones(dim);
		sigma *= std;

		//sigma(0) = 1e3;
		int population = population_size;//leave 0 to use default

		NES nes(f, useImportanceMixing, useBaseline);
		nes.init(pt, sigma, population);
		do
		{
			nes.iterate();
			printf("fitness: %g\n", nes.bestFitness());
			vector<double> best_point(nes.bestPoint().get_data());
			//for (size_t i(0); i < best_point.size(); ++i)
			//	cout << best_point[i] << " ";
			//cout << endl;
//			msleep(3);
		}
		while (nes.bestFitness() > fitness_threshold && nes.evaluations() < maxevals);

		d_best_point = (nes.bestPoint().get_data());
		vector<double> best_point = get_best_point();

		KinematicModel::RobotObservation observation = theRobot.observe();
		vector<double> position = observation.markerPosition(QString("right_hand"));
		d_configuration_points.push_back(best_point);
		d_points.push_back(position);
		d_last_position = position;
		add_point(position[0], position[1], position[2]);
		cout << "n points: " << d_points.size() << endl;
		for (size_t i(0); i < position.size(); ++i)
			cout << position[i] << " ";
		cout << endl;
//		if (d_configuration_points.size() > 1) {
//			write_poses(d_configuration_points, "poses.save");
//		}
	}
	catch (const char* exception)
	{
		printf("\n\nEXCEPTION: %s\n\n", exception);
	}
}
