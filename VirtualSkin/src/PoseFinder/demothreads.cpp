#include "demothreads.h"
#include "util.h"
#include <fstream>

#include <algorithm>
#include <string>
#include <cctype>
#include <yarp/os/all.h>                                                                                             
#include "yarp_misc.h"

using namespace yarp::os;
using namespace std;

MapThread::MapThread(KinematicModel::Model& model, KinematicModel::Robot& robot, std::string configfile, std::string mapfile)
	: verbose(false), keepRunning(true), 
	d_pose_finder(model, robot),
	d_population_size(250), d_start_std(.1), 
	d_store_map_file_name(mapfile), d_load_map_file_name(mapfile) // loading and storing from and to the same file for now
{
	read_constraints(configfile);
	
	//init_standard_poses();

	//nullspace_function();
	//move_box_function();

	//hold_something_function();
	//		hand_left_function();
	//hand_right_function();
	//  hand_right_mark_function();

	//hand_right_look_varun_function();
	//grasp_function();
	//around_object_function();

}

void MapThread::init_standard_poses() {
	cout << "init poses" << endl;
	double simulator_home_pose_arr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, -26., 20., 0, 50, 0, 0, 0, 10, 30, 0, 0, 0, 0, 0, 0, 0, -26, 20, 0, 50, 0, 0, 0, 10, 30, 0, 0, 0, 0, 0, 0, 0};
	d_simulator_home_pose = vector<double>(simulator_home_pose_arr, simulator_home_pose_arr + 41);

	double simulator_wide_pose_arr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, -45., 45., 75., 60, -65, 0, 0, 10, 60, 100, 0, 0, 0, 0, 0, 0, -45, 45, 75, 60, -65, 0, 0, 10, 60, 100, 0, 0, 0, 0, 0, 0};
	d_simulator_wide_pose = vector<double>(simulator_wide_pose_arr, simulator_wide_pose_arr + 41);
}

void MapThread::load_points(string filename) {
	poses_map_t poses_map = read_poses(filename);
	int mapid = 2; //#TODO: read mapid from file
	size_t n_poses = poses_map["WORKSPACE"].size();

	d_poses_q = vector<vector<double> >(n_poses);

	for (size_t i(0); i < d_pose_finder.simulator().total_parts(); ++i) {
		string name("CFGSPACE_");
		name += d_pose_finder.simulator().part_name(i);
		transform(name.begin(), name.end(), name.begin(),  (int (*)(int))toupper);

		vector<vector<double> > &part_configuration(poses_map[name]);
		for (size_t n(0); n < part_configuration.size(); ++n)
			copy(part_configuration[n].begin(), part_configuration[n].end(), back_inserter(d_poses_q[n]));
	}
	//		Maybe conversion?

	for (size_t i(0); i < d_poses_q[0].size(); ++i)
		cout << d_poses_q[0][i] << " ";
	cout << endl;
	d_poses_q = convert_all_to_normal(d_poses_q);
	d_map_build_constraint->add_points(poses_map["WORKSPACE"], d_poses_q);

	cout << "conf point, should be normalized:" << endl;
	for (size_t i(0); i < d_poses_q[0].size(); ++i)
		cout << d_poses_q[0][i] << " ";
	if (d_poses_q.size() > 1)
		for (size_t i(0); i < d_poses_q[1].size(); ++i)
			cout << d_poses_q[1][i] << " ";
	for (size_t i(0); i < d_poses_x->size(); ++i)
		d_pose_finder.simulator().add_point((*d_poses_x)[i][0], (*d_poses_x)[i][1], (*d_poses_x)[i][2], mapid);

	cout << "N Points: " << d_poses_x->size() << endl;
	cout << "Dim Points: " << d_poses_x->at(0).size() << endl;

}

void MapThread::run()
{
	//add bullshit
	//for (float z(-.1); z < .3; z += .03)
	//   d_pose_finder.simulator().add_ball(-.26, 0.06, z);
	//  for (float z(-.1); z < .3; z += .03)
	//   d_pose_finder.simulator().add_ball(-.26, 0.20, z);
	//(-0.307605, 0.134241, 0.1090077)

	std::ifstream in_file(d_load_map_file_name.c_str());
	if (in_file) {
		in_file.close();
		cout << "loading Map " << d_load_map_file_name << endl;
		load_points(d_load_map_file_name);
	}
	//  store_points(store_file_name);


	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	bool filter(false);

	if (filter) {
		filter_collisions(d_pose_finder.simulator());
		store_points(d_store_map_file_name);
	}

	// wait until the thread comes up, then stop it
	while (!d_pose_finder.simulator().model().isRunning())
		msleep(20);

	d_pose_finder.simulator().model().stop();
	size_t n(0);
	bool test(false);
	if (test)
		while (keepRunning) {
			//				size_t n(qrand() % d_poses_q.size());
			cout << "Testing!: "<< n << endl;
			//				n = 279;
			vector<double> random_pose = d_poses_q[n];//d_pose_finder.simulator().home_pos();

			for (size_t i(0); i < random_pose.size(); ++i)
				cout << random_pose[i] << " ";
			cout << endl;

			d_pose_finder.simulator().set_motors(random_pose);
			double n_collisions = d_pose_finder.simulator().computePose();
			KinematicModel::RobotObservation observation = d_pose_finder.simulator().robot().observe();

			cout << n_collisions << endl;
			usleep(2000000);
			cout << d_poses_q.size() << endl;
			n = rand() % d_poses_q.size();
		}
	else
		while (keepRunning) {
			//double start_std(.70);
			//size_t population_size(250);

			vector<double> start_pose;
			if (!d_poses_q.size())
				start_pose = d_pose_finder.get_normal_homepos();
			else {
				//start_pose = random_pose();
				start_pose = outside_pose(d_map_build_constraint->alpha());
			}

			// use large standard deviation on first pose
			double my_start_std = d_start_std;
			if (d_poses_q.size() > 0) {
				my_start_std = 	d_start_std*0.25;
			}

			double minfitness = 0.0001;
			//d_pose_finder.find_pose(start_pose, minfitness, 1.0e-6, my_start_std, d_population_size);
			d_pose_finder.find_pose(start_pose, minfitness, 1.0e-4, my_start_std, d_population_size);
			add_best_pose(minfitness);
			store_points(d_store_map_file_name);
		}
}


void MapThread::add_best_pose(double minfitness) {
	std::vector<double> best_point = d_pose_finder.best_point();

	//assumed last pose was best pose?
	d_pose_finder.simulator().set_motors(best_point);
	double n_collisions = d_pose_finder.simulator().computePose();

	//
	KinematicModel::RobotObservation observation = d_pose_finder.simulator().robot().observe();
	std::vector<double> position = observation.markerPosition(QString(d_map_build_constraint->marker().c_str()));
	if (n_collisions > 0) {
		cout << "Skipping point with collisions" << endl;
		return;
	}

	// we might need a couple of bad points in the beginning, to get the search started, so don't throw out bad points yet
	//if (d_pose_finder.d_pose_fitness_function.eval(best_point) > minfitness) {
	//	cout << "Skipping point sub-threshold fitness";
	//	return;
	//}

	int mapid = 2; //#TODO: read mapid
	d_poses_q.push_back(best_point);
	d_map_build_constraint->add_point(position, best_point);
	for (size_t i(0); i < position.size(); ++i)
		std::cout << position[i] << " ";
	std::cout << std::endl;
	d_pose_finder.simulator().add_point(position[0], position[1], position[2], mapid);
}

void MapThread::read_constraints(string filename) {
	cout << "reading constraints" << endl;
	ConstraintFactory constraint_factory;

	Property file_config;  
	file_config.fromConfigFile(filename.c_str());
	cout << file_config.isNull() << endl;

	vector<string> constraints;

	Bottle &settings = file_config.findGroup("settings");
	cout << settings.isNull() << endl;
	Bottle &constraints_b = settings.findGroup("constraints");
	cout << constraints_b.isNull() << endl;

	//read the constraint list
	for (size_t i(1); i < constraints_b.size(); ++i)
		constraints.push_back(constraints_b.get(i).asString().c_str());

	//read the configurations for the constraints in constraint list
	for (size_t i(0); i < constraints.size(); ++i) {
		cout << "constraint name: " << constraints[i] << endl;
		pair<Constraint*, double> constraint_weight = constraint_factory.constraint_from_bottle(file_config.findGroup(constraints[i].c_str()), file_config);
		constraint_weight.first->post_hook(d_pose_finder.simulator());
		d_pose_finder.add_constraint(constraint_weight.first, constraint_weight.second);
	}

	//add map constraint
	string map_marker = find_and_check<string>(settings, "map_marker");
	size_t n_neighbours = find_and_check<int>(settings, "n_neighbours");
	double distance = find_and_check<double>(settings, "distance");
	double push_factor = find_and_check<double>(settings, "push_factor");


	d_map_build_constraint = new MapBuildConstraint(map_marker, n_neighbours, distance, push_factor);
	d_poses_x = &(d_map_build_constraint->points());
	d_pose_finder.add_constraint(d_map_build_constraint);

	cout << "done" << endl;
}


void MapThread::nullspace_function() {
	cout << "Building constraints" << endl;
	double height(.1);
	//d_map_build_constraint = new MapBuildConstraint("head", 2, .02, .01);
	d_map_build_constraint = new MapBuildConstraint("head", 2, .02, .01);

	//d_pose_finder.pose_fitness_function().debug() = true;
	d_poses_x = &(d_map_build_constraint->points());


	//d_pose_finder.set_start_search_pos(d_simulator_wide_pose);
	d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().real_to_normal_motors(d_simulator_wide_pose)), 3);


	//d_pose_finder.add_constraint(new PointingMarkerConstraint("left_hand", "right_hand", .1, 1, -1), .1);
	//d_pose_finder.add_constraint(new PointingMarkerConstraint("right_hand", "left_hand", .1, 1), .1);

	//d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.3076, -0.0542, 0.06900)), 3.);
	//d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.3076, 0.0542, 0.06900)), 3.);


	d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2476, -0.1, height)), 1.);
	d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2476, 0.1, height)), 1.);

	//pointing straight
	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.)));
	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//keeping straight
	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.)));
	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	d_pose_finder.add_constraint(d_map_build_constraint);
}

void MapThread::move_box_function() {
	cout << "Building constraints" << endl;
	d_map_build_constraint = new MapBuildConstraint("right_hand", 2, .02, .1);


	d_poses_x = &(d_map_build_constraint->points());

	//d_pose_finder.set_start_search_pos(d_simulator_wide_pose);  
	d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().real_to_normal_motors(d_simulator_wide_pose)), 1);


	//d_pose_finder.add_constraint(new PointingMarkerConstraint("left_hand", "right_hand", .1, 1, -1), .1);
	//d_pose_finder.add_constraint(new PointingMarkerConstraint("right_hand", "left_hand", .1, 1), .1);

	//d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.3076, -0.0542, 0.06900)), 3.);
	//d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.3076, 0.0542, 0.06900)), 3.);

	//d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2476, -0.05, 0.06900)), 1.);
	//d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2476, 0.05, 0.06900)), 1.);

	d_pose_finder.add_constraint(new PlaneConstraint("left_hand", 1,  -0.05), 1.);
	d_pose_finder.add_constraint(new PlaneConstraint("right_hand", 1,  0.05), 1.);

	d_pose_finder.add_constraint(new PointingMarkerConstraint("left_hand", "right_hand", 0, 1, -1), .4);
	d_pose_finder.add_constraint(new PointingMarkerConstraint("right_hand", "left_hand", 0, 1), .4);

	//pointing straight
	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.)));
	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//keeping straight
	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.)));
	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	d_pose_finder.add_constraint(d_map_build_constraint);
}

void MapThread::hold_something_function() {
	d_population_size = 200;
	d_start_std = .3;

	cout << "Building constraints" << endl;
	d_map_build_constraint = new MapBuildConstraint("left_hand", 2, .04, 0.1);
	d_marker = "left_hand";
	d_poses_x = &(d_map_build_constraint->points());


	//d_pose_finder.set_start_search_pos(d_simulator_home_pose);


	double pose_mask_arr[] = {1, 1, 1, 
		.2, .2, .2, 0, 0, 0,
		1, 1, 1, 1, 1, 1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, 
		1, 1, 1, 1, 1, 1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1};
	vector<double> pose_mask(pose_mask_arr, pose_mask_arr + 41);


	d_pose_finder.add_constraint(new HomePoseConstraint(d_simulator_wide_pose), 10);

	d_pose_finder.add_constraint(new PointingMarkerConstraint("left_hand", "right_hand", .2, 1, -1), .1);
	d_pose_finder.add_constraint(new PointingMarkerConstraint("right_hand", "left_hand", .2, 1), .1);

	//pointing straight
	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.)));
	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//keeping straight
	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.)));
	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

	//d_pose_finder.add_constraint(new AveragePositionConstraint("right_hand", "left_hand", Constraint::vector3(-0.2376, 0.0, 0.03900)), .2);
	//		d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));

	//		d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

	//		d_pose_finder.add_constraint(new PlaneConstraint(d_marker, 2, .0), 5.);

	//d_pose_finder.add_constraint(new PointingMarkerConstraint("head", "right_hand", 0.0, 0));

	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	d_pose_finder.add_constraint(d_map_build_constraint);

	//		d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

}

void MapThread::around_object_function() {
	//d_population_size = 200;
	//d_start_std = .3;

	cout << "Building constraints" << endl;
	d_map_build_constraint = new MapBuildConstraint("right_handpalm", 2, .04, 0.1);
	d_poses_x = &(d_map_build_constraint->points());


	//d_pose_finder.set_start_search_pos(d_simulator_home_pose);


	double pose_mask_arr[] = {1, 1, 1, 
		.2, .2, .2, 0, 0, 0,
		1, 1, 1, 1, 1, 1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, 
		1, 1, 1, 1, 1, 1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1};
	vector<double> pose_mask(pose_mask_arr, pose_mask_arr + 41);


	double goal_pos_arr[] = {-.3, .2, .0};
	vector<double> goal_pos(goal_pos_arr, goal_pos_arr + 3);

	//d_pose_finder.add_constraint(new HomePoseConstraint(d_simulator_wide_pose), 10);
	d_pose_finder.add_constraint(new PointingConstraint("right_handpalm", goal_pos, .05, 1, -1));

	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	d_pose_finder.add_constraint(d_map_build_constraint, 4);
}

void MapThread::hand_left_function() {
	cout << "Building constraints" << endl;
	d_map_build_constraint = new MapBuildConstraint("left_hand", 2, .02, 0.1);
	d_marker = "left_hand";

	d_poses_x = &(d_map_build_constraint->points());

	d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().home_pos()), .1);

	d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));

	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., 1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

	d_pose_finder.add_constraint(new PlaneConstraint(d_marker, 2, .0), 5.);

	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	d_pose_finder.add_constraint(d_map_build_constraint);

	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., -1., 0.)));
}

void MapThread::hand_right_function() {
	cout << "Building constraints" << endl;
	d_map_build_constraint = new MapBuildConstraint("right_hand", 2, .02, 0.1);
	d_marker = "right_hand";
	d_poses_x = &(d_map_build_constraint->points());

	d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().home_pos()), 1);

	d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.10900)));

	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

	d_pose_finder.add_constraint(new PlaneConstraint(d_marker, 2, .0), 5.);

	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	d_pose_finder.add_constraint(d_map_build_constraint);

	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.)));
	//d_pose_finder.add_constraint(new PointingConstraint("head", Constraint::vector3(-0.2376, 0.1, 0.10900), 0.0, 0));
	//d_pose_finder.add_constraint(new PointingMarkerConstraint("head", "right_hand", 0.0, 0));
}

void MapThread::hand_right_mark_function() {
	cout << "Building constraints" << endl;
	d_map_build_constraint = new MapBuildConstraint("right_hand", 3, .04, 0.1);
	d_marker = "right_hand";
	d_poses_x = &(d_map_build_constraint->points());

	d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().home_pos()), .5);

	d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.10900)));

	//  d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

	d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.10900)), .01);

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

	//  d_pose_finder.add_constraint(new PlaneConstraint(d_marker, 2, .0), 5.);

	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	d_pose_finder.add_constraint(d_map_build_constraint);

	//  d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.)));
	//d_pose_finder.add_constraint(new PointingConstraint("head", Constraint::vector3(-0.2376, 0.1, 0.10900), 0.0, 0));
	//d_pose_finder.add_constraint(new PointingMarkerConstraint("head", "right_hand", 0.0, 0));
}

void MapThread::hand_right_look_varun_function() {     
	float height(.1);
	cout << "Building constraints" << endl;
	d_map_build_constraint = new MapBuildConstraint("right_hand", 2, .04, 0.1);
	d_poses_x = &(d_map_build_constraint->points());

	d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().home_pos()), .3);

	d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.10900)));

	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

	//d_pose_finder.add_constraint(new PlaneConstraint("right_hand", 2, height), 2.);

	d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.))); //left
	//d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., 0., -1.))); //down

	d_pose_finder.add_constraint(d_map_build_constraint);
	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	//d_pose_finder.add_constraint(new PointingConstraint("head", Constraint::vector3(-0.20, 0.0, 0.000), 0.0, 0), .2);

	//d_pose_finder.add_constraint(new PointingMarkerConstraint("head", "right_hand", 0.0, 0), .3);
}

void MapThread::grasp_function() {     
	float height(.0);
	cout << "Building constraints" << endl;

	d_map_build_constraint = new MapBuildConstraint("right_hand", 2, .04, 0.1);
	d_marker = "right_hand";
	d_poses_x = &(d_map_build_constraint->points());

	d_pose_finder.add_constraint(new HomePoseConstraint(d_simulator_wide_pose), 7.);

	d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.10900)));

	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

	double point_factor(10);
	d_pose_finder.add_constraint(new GraspConstraint("right_thumb", "right_index", 2, 1, .01, Constraint::vector3(-0.307605, 0.134241, 0.1090077), point_factor), .04);

	d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

	d_pose_finder.add_constraint(d_map_build_constraint);

	//d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.)));

	d_pose_finder.add_constraint(new PointingMarkerConstraint("head", "right_index", 0.0, 0));
}

std::vector<double> MapThread::random_pose() {
	return d_poses_q[rand() % d_poses_q.size()];
}

std::vector<double> MapThread::outside_pose(double threshold) {
	vector<vector<double > > &poses = *d_poses_x; // we want to augment the range in workspace, not in configuration space
	//vector<vector<double > > &poses = d_poses_q;
	int nposes = d_poses_x->size();
	vector<int > neighbors(nposes, 0); // number of neighbors within area

	double diff;
	for (int i(0); i< nposes; i++) {
		vector<double > distances(nposes, 0.0); // not really necessary to put this in a vector, but maybe use it later
		for (int j(0); j< nposes; j++) {
			// compute the distance between poses[i] and poses[j]
			for (int k(0); k< poses[i].size(); k++) {
				diff = poses[i][k]-poses[j][k];
				distances[j] = distances[j]+diff*diff;
			}
			distances[j] = sqrt(distances[j]);
			// count the number of point within threshold distance of poses_x[i];
			if (distances[j] <= threshold)
				neighbors[i]++;
		}
	}

	// randomly pick a pose among the poses with the smallest number of neighbors within threshold:
	int minvalue = *std::min_element(neighbors.begin(), neighbors.end());
	vector<int > min_neigbor_indices;
	for (int j(0); j< nposes; j++) {
		if (neighbors[j] <= minvalue)
			min_neigbor_indices.push_back(j);
	}
	int min_neighbor_index = min_neigbor_indices[rand() % min_neigbor_indices.size()];

	// return the pose at randomly selected minimum neighbor index
	cout << "choosing point from " << min_neigbor_indices.size() << " / " << nposes << " poses with " << minvalue << " / " << *std::max_element(neighbors.begin(), neighbors.end()) << " neigbors within " << threshold << " workspace units" << endl;
	cout << "starting point (workspace): " << poses[min_neighbor_index][0] << " " << poses[min_neighbor_index][1] << " " << poses[min_neighbor_index][2] << endl;
	return d_poses_q[min_neighbor_index];
}

void MapThread::store_points(std::string filename) {
	poses_map_t store_map;

	std::vector<std::vector<double> > real_poses = convert_all_to_real(d_poses_q);

	for (size_t i(0); i < real_poses.size(); ++i) {
		std::map<std::string, std::vector<double> > a_motor_map(d_pose_finder.simulator().to_motor_named_map(real_poses[i]));
		std::string name("CFGSPACE_");
		std::map<std::string, std::vector<double> >::iterator it(a_motor_map.begin());
		for (; it != a_motor_map.end(); ++it) {
			std::string sub_name(name + it->first);
			std::transform(sub_name.begin(), sub_name.end(), sub_name.begin(), toupper);
			if (!store_map.count(sub_name))
				store_map[sub_name] = std::vector<std::vector<double> >();
			store_map[sub_name].push_back(it->second);
		}
	}
	store_map["WORKSPACE"] = *d_poses_x;
	if (d_poses_x->size() > 0)
		write_poses(store_map, filename);
}

std::vector<std::vector<double> > MapThread::convert_all_to_real(std::vector<std::vector<double> > &in) {
	std::vector<std::vector<double> >  out;
	for (size_t i(0); i < in.size(); ++i)
		out.push_back(d_pose_finder.simulator().normal_to_real_motors(in[i]));
	return out;
}

std::vector<std::vector<double> > MapThread::convert_all_to_normal(std::vector<std::vector<double> > &in) {
	std::vector<std::vector<double> >  out;
	for (size_t i(0); i < in.size(); ++i) {
		out.push_back(d_pose_finder.simulator().real_to_normal_motors(in[i]));
	}
	return out;
}

void MapThread::stop() {
	printf("PoseFinder thread is shutting down ");
	keepRunning = false;
	while ( isRunning() ) {
		printf(".");
		msleep(1000);
	}
	printf("\n");
}

void MapThread::filter_collisions(Simulator &simulator) {
	std::vector<std::vector<double> > filtered_configurations;
	std::vector<std::vector<double> > filtered_points;

	for (size_t i(0); i < d_poses_q.size(); ++i) {
		std::vector<double> pose = d_poses_q[i];//d_pose_finder.simulator().home_pos();
		simulator.set_motors(pose);
		double n_collisions = simulator.computePose();
		if (n_collisions) {
			std::cout << "FILTARR" << std::endl;
			continue;
		}
		filtered_configurations.push_back(pose);
		filtered_points.push_back((*d_poses_x)[i]);
	}

	d_poses_q = filtered_configurations;
	*d_poses_x = filtered_points;
}



OnlineThread::OnlineThread(KinematicModel::Model& model, KinematicModel::Robot& robot) : 
	verbose(false), 
	keepRunning(true), 
	d_pose_finder(model, robot)
{
	PositionConstraint *position_constraint = new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900));

	d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().home_pos()), 1.);

	d_pose_finder.add_constraint(position_constraint);

	OrientationConstraint *orientation_constraint = new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.));
	d_pose_finder.add_constraint(orientation_constraint);

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

	d_pose_finder.add_constraint(new PlaneConstraint("left_hand", 2, .0), 10.);

	d_pose_finder.add_constraint(new CollisionConstraint());

	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., 0., 1.)));

	slider_window.show();
	slider_window.add_slider("Front", &(position_constraint->goal()[0]), -.3, .3);
	slider_window.add_slider("Right", &(position_constraint->goal()[1]), -.3, .3);
	slider_window.add_slider("Up", &(position_constraint->goal()[2]), -.3, .3);

	slider_window.add_slider("Pointing Front", &(orientation_constraint->element(0)), -1, 1);
	slider_window.add_slider("Pointing Right", &(orientation_constraint->element(1)), -1, 1);
	slider_window.add_slider("Pointing Up", &(orientation_constraint->element(2)), -1, 1);


	connect(&slider_window, SIGNAL(something_changed()), this, SLOT(reset_variance()));
}

void OnlineThread::stop() {
	printf("PoseFinder thread is shutting down ");
	keepRunning = false;
	while ( isRunning() )
	{
		printf(".");
		msleep(1000);
	}
	printf("\n");
}

void OnlineThread::run() {

	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	while (keepRunning) {
		if (slider_window.changed()) {
			d_pose_finder.find_pose(d_pose_finder.get_normal_homepos(), 0., 1.0e-6, .10, 50);
			slider_window.changed(false);
			std::cout << "Found pose" << std::endl;
		}
		else
			sleep(1);
	}
}


/*
PathThread::PathThread(KinematicModel::Model& model, KinematicModel::Robot& robot)
	: verbose(false), keepRunning(true),
	d_pose_finder(model, robot)
{
	if (false) {
		for (float z(-.2); z < .2; z += .03)
			d_pose_finder.simulator().add_ball(-.2, .25, z);
		//			for (float z(-.2); z < .2; z += .03)
		//				d_pose_finder.simulator().add_ball(-.2, .3, z);
	}
	float x(-0.2076), y(0.2942), z(0.13900);

	d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 10.);

	d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));

	d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

	d_pose_finder.simulator().add_point(x, y, z, mapid);

	d_pose_finder.add_constraint(new GraspConstraint("right_thumb", "right_index", 2, 1, .04, Constraint::vector3(x, y, z)));

	d_pose_finder.add_constraint(new CollisionConstraint(), 2.);
	//
	//		d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 1.);
	//
	//		d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));
	//
	//		d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));
	//
	//	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));
	//
	//		d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));
	//
	//		d_pose_finder.add_constraint(new CollisionConstraint());
	//
	d_pose_finder.set_filter(new StoreFilter("position"));
}

void PathThread::stop()
{
	printf("PoseFinder thread is shutting down ");
	keepRunning = false;
	while ( isRunning() )
	{
		printf(".");
		msleep(1000);
	}
	printf("\n");
}

void PathThread::run()
{
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
	d_pose_finder.find_pose(10000, 0.,.1, 100);
	//		d_pose_finder.find_pose(10000, 0., .04, 50);

	StoreFilter &store_filter = reinterpret_cast<StoreFilter&>(d_pose_finder.get_filter());
	std::cout << "n_points: " << store_filter.d_configurations.size() << std::endl;

	std::vector<std::vector<double> > path;
	try {
		path = extract_path(store_filter.configurations(), store_filter.fitnesses(), store_filter.collisions(), d_pose_finder.simulator().home_pos(), d_pose_finder.best_point());
	}
	catch (char const *message) {
		std::cout << message << std::endl;
	}

	while (true) {
		for (size_t i(0); i < path.size(); ++i) {
			std::cout << "step " << i << "/" << path.size() << std::endl;
			if (i)
				std::cout << "distance: " << Constraint::pos_error(path[i-1], path[i]) << std::endl;
			d_pose_finder.simulator().set_motors(path[i]);
			d_pose_finder.simulator().computePose();

			std::vector<double> position = d_pose_finder.simulator().robot().observe().markerPosition(QString("right_hand"));
			std::cout << "pos" << std::endl;
			for (size_t i(0); i < position.size(); ++i)
				std::cout << position[i] << " ";
			std::cout << std::endl;
			msleep(300);
		}
	}
}*/


void StoreFilter::operator()(const std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation) {
	//		if (n_collisions)
	//			return;
	d_configurations.push_back(values);
	d_fitnesses.push_back(fitness);
	d_collisions.push_back(n_collisions);
	std::vector<double> position = observation.markerPosition(QString("right_hand"));
	for (size_t i(0); i < position.size(); ++i)
		d_outfile << position[i] << ", ";
	d_outfile << std::endl;
}


