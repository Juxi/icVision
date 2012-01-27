#include "graspfinder.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include <iostream>

void GraspFinder::stop()
{
	printf("ObjectSoup thread is shutting down ");
	keepRunning = false;
	while ( isRunning() )
	{
		printf(".");
		msleep(1000);
	}
	printf("\n");
}

void GraspFinder::run()
{
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
	
	size_t counter(0);
	while ( keepRunning )
	{
		//std::vector<double> motor_values(d_total_motors);

		//for (size_t i(0); i < motor_values.size(); ++i)
		//	motor_values[i] = 1.0 * qrand() / RAND_MAX;

		//set_motors(motor_values);
		//int collisions = theModel.computePose();
		//std::cout << counter << ", collisions: " << collisions << std::endl;

		do_test();
		msleep(1);
		++counter;
	}
	

	//printf("ObjectSoup::run() returned\n");
}

//def get_hand_orientation_measure(self):
//    goal_orientation = np.array([0., 0., -1., 0., 0.])
//    orientation = self.get_hand_orientation()
//    return np.dot(goal_orientation, orientation) / norm(goal_orientation) / norm(orientation)
//
//def get_hand_orientation(self):
//	observations = self.get_observations()
//
//	if not observations:
//		return None
//	return np.array(observations['right_hand'][0:3] + [observations['right_hand'][6]] + [observations['right_hand'][10]])

void GraspFinder::do_test() {

	class Pose : public Function
	{
	public:
		GraspFinder *d_grasp_finder;
		public:

		Pose(GraspFinder *grasp_finder) : d_grasp_finder(grasp_finder)
		{
		}

		double pos_error(std::vector<double> &values, std::vector<double> goal_pos) const {
			double distance(0.0);
			for (size_t i(0); i < 3; ++i)
				distance += pow(values[i] - goal_pos[i], 2.0);
			return sqrt(distance);
		}

		double rand_val() const {
			return (((double)(qrand() % 10000000)) / 10000000.);
		}

		double right_hand_measure() const {
			std::string marker_name("right_hand");

			KinematicModel::RobotObservation observation = d_grasp_finder->theRobot.observe();

			std::vector<double> goal_pos(3);
			goal_pos[0] = -0.227605;// + rand_val() * .03;
			goal_pos[1] = 0.234241;// + rand_val() * .03;
			goal_pos[2] = 0.0390077 + .0;
			std::vector<double> position = observation.markerPosition(QString(marker_name.c_str()));
			double position_distance = Pose::pos_error(position, goal_pos);

			std::vector<double> goal_orientation(9);
			std::vector<double> mask_orientation(9);
			goal_orientation[5] = -1;
			mask_orientation[3] = 1;
			mask_orientation[4] = 1;
			mask_orientation[5] = 1;
			double orientation_goodness = observation.orientationMeasure(QString(marker_name.c_str()), goal_orientation, mask_orientation);

			return position_distance + orientation_goodness;
		}

		double left_hand_measure() const {
			std::string marker_name("left_hand");

			KinematicModel::RobotObservation observation = d_grasp_finder->theRobot.observe();

			//std::cout << "val: " << rand_val() << std::endl;
			std::vector<double> goal_pos(3);
			goal_pos[0] = -0.227605;//; + rand_val() * .03;
			goal_pos[1] = -0.234241;//;+ rand_val() * .03;
			goal_pos[2] = 0.0390077 + .0;
			std::vector<double> position = observation.markerPosition(QString(marker_name.c_str()));
			double position_distance = Pose::pos_error(position, goal_pos);
			std::cout << "left position error: " << position_distance << std::endl;

			std::vector<double> goal_orientation(9);
			std::vector<double> mask_orientation(9);
			goal_orientation[5] = 1;
			mask_orientation[3] = 1;
			mask_orientation[4] = 1;
			mask_orientation[5] = 1;
			double orientation_goodness = observation.orientationMeasure(QString(marker_name.c_str()), goal_orientation, mask_orientation);

			return position_distance + orientation_goodness;
		}

		double eval(const Matrix& point) const
		{

			std::vector<double> values(point.get_data());
			d_grasp_finder->set_motors(values);
			double collisions = d_grasp_finder->theModel.computePose();
			double collision_value = .1 * pow(3. -(double)collision_value, 2.0);

			double home_penalty = d_grasp_finder->distance_to_home(values);
			double right_hand_val = right_hand_measure();
			double left_hand_val = left_hand_measure();
			//std::cout <<  "pos: " << home_penalty << " " << left_hand_val << " " << right_hand_val << " |" << endl;

			return 1. * home_penalty + right_hand_val + left_hand_val;
		}
	};

	try
	{
		// Experiment setup:
		// (*) dimension of sphere function
		// (*) number of function evaluations
		// (*) number of trials
		unsigned int dim = d_total_motors;
		unsigned int maxevals = 200000;
		unsigned int trials = 10000;

		bool useImportanceMixing = false;
		bool useBaseline = false;

		unsigned int m;
		double mean = 0.0;
		for (m=0; m<trials; m++)
		{
			rngSeed(123 * m + 45);
			Pose f(this);
			Matrix pt = rngGauss(dim);
			pt *= .2;

			Matrix add = Matrix::ones(dim);
			add *= .0;
			pt += add;

			Matrix sigma = Matrix::ones(dim);
			sigma *= .4;

			//sigma(0) = 1e3;
			int population = 0;//leave 0 to use default

			NES nes(f, useImportanceMixing, useBaseline);
			nes.init(pt, sigma, population);
			do
			{
				nes.iterate();
				//printf("fitness: %g\n", nes.bestFitness());
				std::vector<double> best_point(nes.bestPoint().get_data());
				//for (size_t i(0); i < best_point.size(); ++i)
				//	std::cout << best_point[i] << " ";
				//std::cout << std::endl;
				
			}
			while (nes.evaluations() < maxevals);
			mean -= log10(nes.bestFitness());
			//printf("   trial: %d/%d     fitness: %g\n", m+1, trials, nes.bestFitness());
		}
		mean /= (double)trials;
		printf("xNES mean success (-log10(fitness): %g\n", mean);
	}
	catch (const char* exception)
	{
		printf("\n\nEXCEPTION: %s\n\n", exception);
	}
}
