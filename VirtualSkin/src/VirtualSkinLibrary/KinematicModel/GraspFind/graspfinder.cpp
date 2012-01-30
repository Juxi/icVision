#include "graspfinder.h"

#include <iostream>
#include <algorithm>

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
	while (true) {
		do_test();
	}
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

		double pos_error(std::vector<double> &values, std::vector<double> &goal_pos) const {
			double distance(0.0);
			for (size_t i(0); i < values.size(); ++i)
				distance += pow(values[i] - goal_pos[i], 2.0);
			return sqrt(distance);
		}

		double rand_val() const {
			return (((double)(qrand() % 10000000)) / 10000000.);
		}

		double close_measure(std::vector<double> &values, double alpha) const {
			std::vector<double> distances;
			for (size_t i(0); i < d_grasp_finder->d_points.size(); ++i) {
				distances.push_back(pos_error(values, d_grasp_finder->d_points[i]));
			}

			std::sort(distances.begin(), distances.end());
			int n_distances = distances.size() >= 2 ? 2 : distances.size();

			double measure(0.0);
			for (size_t i(0); i < n_distances; ++i)  {
				measure += fabs(distances[i] - alpha);// * (distances[i] - alpha); //or use abs?
			}

			return measure;
		}

		double right_hand_measure() const {
			std::string marker_name("right_hand");

			KinematicModel::RobotObservation observation = d_grasp_finder->theRobot.observe();
			static bool object = false;

			std::vector<double> goal_pos(3);
			goal_pos[0] = -0.327605;// + rand_val() * .03;
			goal_pos[1] = 0.234241;// + rand_val() * .03;
			goal_pos[2] = 0.0390077;
			std::vector<double> position = observation.markerPosition(QString(marker_name.c_str()));
			double position_distance = Pose::pos_error(position, goal_pos);

			double height_error = fabs(goal_pos[2] - position[2]);// * (goal_pos[2] - position[2]);

			if (!object) {
				//d_grasp_finder->add_ball(goal_pos[0], goal_pos[1], goal_pos[2]);
				object = true;
			}

			std::vector<double> goal_orientation(9);
			std::vector<double> mask_orientation(9);
			goal_orientation[4] = -1;
			mask_orientation[3] = 1;
			mask_orientation[4] = 1;
			mask_orientation[5] = 1;
			double orientation_goodness = observation.orientationMeasure(QString(marker_name.c_str()), goal_orientation, mask_orientation);

			double alpha(.1);
			double nn_distance = close_measure(position, alpha);
			//return position_distance + orientation_goodness;
			return height_error + nn_distance + orientation_goodness;
		}

		double left_hand_measure() const {
			std::string marker_name("left_hand");

			KinematicModel::RobotObservation observation = d_grasp_finder->theRobot.observe();
			static bool object = false;

			//std::cout << "val: " << rand_val() << std::endl;
			std::vector<double> goal_pos(3);
			goal_pos[0] = -0.227605;//; + rand_val() * .03;
			goal_pos[1] = -0.234241;//;+ rand_val() * .03;
			goal_pos[2] = 0.0390077 + .1;
			std::vector<double> position = observation.markerPosition(QString(marker_name.c_str()));
			double position_distance = Pose::pos_error(position, goal_pos);

			if (!object) {
				//d_grasp_finder->add_ball(goal_pos[0], goal_pos[1], goal_pos[2]);
				object = true;
			}

			std::vector<double> goal_orientation(9);
			std::vector<double> mask_orientation(9);
			goal_orientation[4] = -1;
			mask_orientation[3] = 1;
			mask_orientation[4] = 1;
			mask_orientation[5] = 1;
			double orientation_goodness = observation.orientationMeasure(QString(marker_name.c_str()), goal_orientation, mask_orientation);
			//if (position_distance < 1.0e-4)
			//  std::cout << position_distance << " " << orientation_goodness << std::endl;
			return position_distance + orientation_goodness;
		}

		double eval(const Matrix& point) const
		{
			std::vector<double> values(point.get_data());
//			for (size_t i(0); i < values.size(); ++i)
//				std::cout << values[i] << " ";
//			std::cout << std::endl;
			d_grasp_finder->set_motors(values);
			double collisions = d_grasp_finder->theModel.computePose();
			double collision_value = .1 * pow(3. -(double)collision_value, 2.0);

			double home_penalty = d_grasp_finder->distance_to_home(values);
			double right_hand_val = right_hand_measure();
			double left_hand_val = left_hand_measure();
			//std::cout <<  "pos: " << home_penalty << " " << left_hand_val << " " << right_hand_val << " |" << endl;

			return home_penalty + right_hand_val + left_hand_val + collisions;
		}
	};

	try
	{
		// Experiment setup:
		// (*) dimension of sphere function
		// (*) number of function evaluations
		// (*) number of trials
		unsigned int dim = d_total_motors;
		unsigned int maxevals = 30000;
		double fitness_threshold = -1.99;
		bool useImportanceMixing = false;
		bool useBaseline = false;

		unsigned int m;

			rngSeed(123 * m + 45);
			Pose f(this);
			Matrix pt = rngGauss(dim);
			pt *= .1;

			Matrix add = Matrix::ones(dim);
			add *= .0;
			pt += add;
			for (size_t i(0); i < d_home_pos.size(); ++i)
			  pt[i] += d_home_pos[i];

			Matrix sigma = Matrix::ones(dim);
			sigma *= .03;

			//sigma(0) = 1e3;
			int population = 50;//leave 0 to use default

			NES nes(f, useImportanceMixing, useBaseline);
			nes.init(pt, sigma, population);
			do
			{
				nes.iterate();
				printf("fitness: %g\n", nes.bestFitness());
				std::vector<double> best_point(nes.bestPoint().get_data());
				//for (size_t i(0); i < best_point.size(); ++i)
				//	std::cout << best_point[i] << " ";
				//std::cout << std::endl;
				msleep(3);
			}
			while (nes.bestFitness() > fitness_threshold && nes.evaluations() < maxevals);

			std::vector<double> best_point(nes.bestPoint().get_data());
			KinematicModel::RobotObservation observation = theRobot.observe();
			std::vector<double> position = observation.markerPosition(QString("right_hand"));
			d_configuration_points.push_back(best_point);
			d_points.push_back(position);
			add_ball(position[0], position[1], position[2]);
			std::cout << "n points: " << d_points.size() << std::endl;
			for (size_t i(0); i < position.size(); ++i)
				std::cout << position[i] << " ";
			std::cout << std::endl;
	}
	catch (const char* exception)
	{
		printf("\n\nEXCEPTION: %s\n\n", exception);
	}
}
