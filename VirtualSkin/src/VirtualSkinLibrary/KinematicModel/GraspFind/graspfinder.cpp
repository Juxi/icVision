#include "graspfinder.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <fstream>

using namespace std;

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

void write_poses(vector<vector<double> > &vectors, string filename) {
	ofstream out_file(filename.c_str());
	out_file << "CFGSPACE" << endl;
	for (size_t i(0); i < vectors.size(); ++i) {
		vector<double> &vec(vectors[i]);
		for (size_t n(0); n < vec.size(); ++n)
			out_file << vec[n] << " ";
		out_file << endl;
	}
}

class Pose : public Function
{
public:
	GraspFinder *d_grasp_finder;
	public:

	Pose(GraspFinder *grasp_finder) : d_grasp_finder(grasp_finder)
	{
	}

	double pos_error(vector<double> &values, vector<double> &goal_pos) const {
		double distance(0.0);
		for (size_t i(0); i < values.size(); ++i)
			distance += pow(values[i] - goal_pos[i], 2.0);
		return sqrt(distance);
	}

	double rand_val() const {
		return (((double)(qrand() % 10000000)) / 10000000.);
	}

	vector<double> diff(vector<double> &left, vector<double> &right) const {
		vector<double> diff(left);
		for (size_t i(0); i < diff.size(); ++i)
			diff[i] -= right[i];
		return diff;
	}

	double norm_in_product(vector<double> &left, vector<double> &right) const {
		double total(0), total_left(0), total_right(0);
		for (size_t i(0); i < left.size(); ++i) {
			total += left[i] * right[i];
			total_left += left[i] * left[i];
			total_right += right[i] * right[i];
		}
		return total / sqrt(total_left) / sqrt(total_right);
	}

	double close_measure(vector<double> &values, double alpha) const {
		vector<double> distances;
		for (size_t i(0); i < d_grasp_finder->d_points.size(); ++i) {
			distances.push_back(pos_error(values, d_grasp_finder->d_points[i]));
		}

		sort(distances.begin(), distances.end());
		int n_distances = distances.size() >= 3 ? 3 : distances.size();

		double total_dist = accumulate(distances.begin(), distances.end(), 0.0);
		double measure(0.0);
		for (size_t i(0); i < n_distances; ++i)  {
			measure += fabs(distances[i] - alpha);// * (distances[i] - alpha); //or use abs?
		}

		return measure + exp(-total_dist);
	}

	double grasp_measure(vector<double> &object_pos, double distance) const {
		KinematicModel::RobotObservation observation = d_grasp_finder->theRobot.observe();

		vector<double> position_thumb = observation.markerPosition(QString("right_thumb"));
		vector<double> position_index = observation.markerPosition(QString("right_index"));

		//fixed distance from object
		double thumb_distance = pos_error(object_pos, position_thumb);
		double index_distance = pos_error(object_pos, position_index);

		double thumb_distance_measure = fabs(thumb_distance - distance);
		double index_distance_measure = fabs(index_distance - distance);

		vector<double> thumb_diff(diff(object_pos, position_thumb));
		vector<double> index_diff(diff(object_pos, position_index));

		vector<double> goal_orientation(9);
		vector<double> mask_orientation(9);
		copy(thumb_diff.begin(), thumb_diff.end(), goal_orientation.begin() + 6);
		fill(mask_orientation.begin() + 6, mask_orientation.begin() + 9, 1);

		double thumb_to_object = observation.orientationMeasure("right_thumb", goal_orientation, mask_orientation);

		fill(goal_orientation.begin(), goal_orientation.end(), 0);
		copy(index_diff.begin(), index_diff.end(), goal_orientation.begin() + 3);
		fill(mask_orientation.begin(), mask_orientation.end(), 0);
		fill(mask_orientation.begin() + 3, mask_orientation.begin() + 6, 1);

		double index_to_object = observation.orientationMeasure("right_index", goal_orientation, mask_orientation);

		double opposite_measure = norm_in_product(thumb_diff, index_diff);

		//return index_distance_measure + index_to_object;
//			cout << index_distance_measure << endl;
		return 10. * thumb_distance_measure + 10. * index_distance_measure + thumb_to_object + index_to_object + opposite_measure;
	}

	double right_hand_measure() const {
		string marker_name("right_hand");

		KinematicModel::RobotObservation observation = d_grasp_finder->theRobot.observe();
		static bool object = false;

		vector<double> goal_pos(3);
		goal_pos[0] = -0.237605;// + rand_val() * .03;
		goal_pos[1] = 0.234241;// + rand_val() * .03;
		goal_pos[2] = 0.1390077;
		vector<double> position = observation.markerPosition(QString(marker_name.c_str()));
		double position_distance = Pose::pos_error(position, goal_pos);

		double height_error = fabs(goal_pos[2] - position[2]);// * (goal_pos[2] - position[2]);

		if (!object) {
			d_grasp_finder->add_point(goal_pos[0], goal_pos[1], goal_pos[2]);
			object = true;
		}

		vector<double> goal_orientation(9);
		vector<double> mask_orientation(9);
		goal_orientation[4] = -1;
		mask_orientation[3] = 1;
		mask_orientation[4] = 1;
		mask_orientation[5] = 1;
		double orientation_goodness = observation.orientationMeasure(QString(marker_name.c_str()), goal_orientation, mask_orientation);

		double alpha(.1);
		double nn_distance = close_measure(position, alpha);
		//return position_distance + orientation_goodness;
		//return nn_distance + orientation_goodness;
		return grasp_measure(goal_pos, .05);// + orientation_goodness;
	}

	double left_hand_measure() const {
		string marker_name("left_hand");

		KinematicModel::RobotObservation observation = d_grasp_finder->theRobot.observe();
		static bool object = false;

		//cout << "val: " << rand_val() << endl;
		vector<double> goal_pos(3);
		goal_pos[0] = -0.227605;//; + rand_val() * .03;
		goal_pos[1] = -0.234241;//;+ rand_val() * .03;
		goal_pos[2] = 0.0390077 + .1;
		vector<double> position = observation.markerPosition(QString(marker_name.c_str()));
		double position_distance = Pose::pos_error(position, goal_pos);

		if (!object) {
			//d_grasp_finder->add_ball(goal_pos[0], goal_pos[1], goal_pos[2]);
			object = true;
		}

		vector<double> goal_orientation(9);
		vector<double> mask_orientation(9);
		goal_orientation[4] = -1;
		mask_orientation[3] = 1;
		mask_orientation[4] = 1;
		mask_orientation[5] = 1;
		double orientation_goodness = observation.orientationMeasure(QString(marker_name.c_str()), goal_orientation, mask_orientation);
		//if (position_distance < 1.0e-4)
		//  cout << position_distance << " " << orientation_goodness << endl;
		return position_distance + orientation_goodness;
	}

	double eval(const Matrix& point) const
	{
		vector<double> values(point.get_data());
//			for (size_t i(0); i < values.size(); ++i)
//				cout << values[i] << " ";
//			cout << endl;
		d_grasp_finder->set_motors(values);
		double collisions = d_grasp_finder->theModel.computePose();
		double collision_value = .1 * pow(3. -(double)collision_value, 2.0);

		double home_penalty = d_grasp_finder->distance_to_home(values);
		double right_hand_val = right_hand_measure();
		double left_hand_val = left_hand_measure();
		//cout <<  "pos: " << home_penalty << " " << left_hand_val << " " << right_hand_val << " |" << endl;
		//cout << collisions << endl;
		return 10. * home_penalty + right_hand_val + left_hand_val + collisions;
	}
};

void GraspFinder::do_test() {
	try
	{
		// Experiment setup:
		// (*) dimension of sphere function
		// (*) number of function evaluations
		// (*) number of trials
		unsigned int dim = d_total_motors;
		//unsigned int maxevals = 30000;
		unsigned int maxevals = 300000;
		double fitness_threshold = -1.99;
		bool useImportanceMixing = false;
		bool useBaseline = false;

		unsigned int m;

		rngSeed(123 * m + 45);
		Pose f(this);

		Matrix pt = Matrix::zeros(dim);
		for (size_t i(0); i < d_home_pos.size(); ++i)
		  pt[i] = d_home_pos[i];

		Matrix sigma = Matrix::ones(dim);
		sigma *= .43;

		//sigma(0) = 1e3;
		int population = 250;//leave 0 to use default

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

		vector<double> best_point(nes.bestPoint().get_data());
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
		if (d_configuration_points.size() > 1) {
			write_poses(d_configuration_points, "poses.save");
		}
	}
	catch (const char* exception)
	{
		printf("\n\nEXCEPTION: %s\n\n", exception);
	}
}
