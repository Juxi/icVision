#include "demothreads.h"
#include "util.h"

#include <algorithm>
#include <string>
#include <cctype>

using namespace std;

MapThread::MapThread(KinematicModel::Model& model, KinematicModel::Robot& robot)
  : verbose(false), keepRunning(true), 
	d_pose_finder(model, robot),
	d_population_size(100), d_start_std(.1)
{
  init_standard_poses();
  
  //nullspace_function();
 //move_box_function();

  //hold_something_function();
  //		hand_left_function();
  //hand_right_function();
  //  hand_right_mark_function();
  
  hand_right_look_varun_function();
  //grasp_function();
  //around_object_function();

}

void MapThread::init_standard_poses() {
  std::cout << "init poses" << std::endl;
  double simulator_home_pose_arr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, -26., 20., 0, 50, 0, 0, 0, 10, 30, 0, 0, 0, 0, 0, 0, 0, -26, 20, 0, 50, 0, 0, 0, 10, 30, 0, 0, 0, 0, 0, 0, 0};
  d_simulator_home_pose = vector<double>(simulator_home_pose_arr, simulator_home_pose_arr + 41);

  double simulator_wide_pose_arr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, -45., 45., 75., 60, -65, 0, 0, 10, 60, 100, 0, 0, 0, 0, 0, 0, -45, 45, 75, 60, -65, 0, 0, 10, 60, 100, 0, 0, 0, 0, 0, 0};
  d_simulator_wide_pose = vector<double>(simulator_wide_pose_arr, simulator_wide_pose_arr + 41);
}

void MapThread::load_points(string filename) {
  poses_map_t poses_map = read_poses(filename);
  
  size_t n_poses = poses_map["WORKSPACE"].size();
  
  d_configuration_points = vector<vector<double> >(n_poses);
  
  for (size_t i(0); i < d_pose_finder.simulator().total_parts(); ++i) {
    string name("CFGSPACE_");
    name += d_pose_finder.simulator().part_name(i);
    transform(name.begin(), name.end(), name.begin(),  (int (*)(int))std::toupper);

    vector<vector<double> > &part_configuration(poses_map[name]);
    for (size_t n(0); n < part_configuration.size(); ++n)
      copy(part_configuration[n].begin(), part_configuration[n].end(), back_inserter(d_configuration_points[n]));
  }
//		Maybe conversion?
  
  for (size_t i(0); i < d_configuration_points[0].size(); ++i)
    cout << d_configuration_points[0][i] << " ";
  cout << endl;
  d_configuration_points = convert_to_normal(d_configuration_points);
  d_map_build_constraint->add_points(poses_map["WORKSPACE"], d_configuration_points);
  
  cout << "conf point, should be normalized:" << endl;
  for (size_t i(0); i < d_configuration_points[0].size(); ++i)
    cout << d_configuration_points[0][i] << " ";
  for (size_t i(0); i < d_configuration_points[0].size(); ++i)
    cout << d_configuration_points[1][i] << " ";
  for (size_t i(0); i < d_points->size(); ++i)
    d_pose_finder.simulator().add_point((*d_points)[i][0], (*d_points)[i][1], (*d_points)[i][2]);
  
  cout << "N Points: " << d_points->size() << endl;
  cout << "Dim Points: " << d_points->at(0).size() << endl;
  
}

void MapThread::run()
{
  //add bullshit
  //for (float z(-.1); z < .3; z += .03)
  //   d_pose_finder.simulator().add_ball(-.26, 0.06, z);
  //  for (float z(-.1); z < .3; z += .03)
  //   d_pose_finder.simulator().add_ball(-.26, 0.20, z);
    //(-0.307605, 0.134241, 0.1090077)

  //string load_file_name("maps/table_map_right_hand.save");
  
  //string load_file_name("maps/crappy_map.save");
  string load_file_name("map.save");
  string store_file_name("map.save");

  if (exists(load_file_name)) {
    cout << "loading Map " << load_file_name << endl;
    load_points(load_file_name);
  }
//  store_points(store_file_name);
  QTime time = QTime::currentTime();
  qsrand((uint)time.msec());

  bool filter(false);

  if (filter) {
    filter_collisions(d_pose_finder.simulator());
    store_points(store_file_name);
  }

  size_t n(0);
  bool test(false);
  if (test)
    while (true) {
      //				size_t n(qrand() % d_configuration_points.size());
      cout << "Testing!: "<< n << endl;
      //				n = 279;
      vector<double> random_pose = d_configuration_points[n];//d_pose_finder.simulator().home_pos();
	  
      for (size_t i(0); i < random_pose.size(); ++i)
		cout << random_pose[i] << " ";
      cout << endl;

      d_pose_finder.simulator().set_motors(random_pose);
      double n_collisions = d_pose_finder.simulator().computePose();
      KinematicModel::RobotObservation observation = d_pose_finder.simulator().robot().observe();
      
      cout << n_collisions << endl;
      usleep(2000000);
      cout << d_configuration_points.size() << endl;
      n = rand() % d_configuration_points.size();
    }
  else
    while (true) {
      //double start_std(.70);
      //size_t population_size(250);

	  std::vector<double> start_pose;
	  if (!d_configuration_points.size())
		start_pose = d_pose_finder.get_normal_homepos();
	  else
		start_pose = random_pose();
	  d_pose_finder.find_pose(start_pose, 0., 1.0e-6, d_start_std, d_population_size);
      add_best_pose();
      store_points(store_file_name);
    }
}


void MapThread::nullspace_function() {
  cout << "Building constraints" << endl;
  double height(.1);
  d_map_build_constraint = new MapBuildConstraint("head", 2, .02, .01);
  
  //d_pose_finder.pose_fitness_function().debug() = true;
  d_points = &(d_map_build_constraint->points());

  std::cout << "blaat" << std::endl;

  
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
  
  
  d_points = &(d_map_build_constraint->points());

  std::cout << "blaat " << d_simulator_wide_pose.size() << std::endl;

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
  d_points = &(d_map_build_constraint->points());


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
  d_points = &(d_map_build_constraint->points());


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

  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), .1);

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
  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 1);

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
  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), .5);

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
  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 3.);

  d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.10900)));

  d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

  //		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

  d_pose_finder.add_constraint(new PlaneConstraint("right_hand", 2, height), 2.);

  d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.))); //left
  //d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., 0., -1.))); //down

  d_pose_finder.add_constraint(d_map_build_constraint);
  d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

  //d_pose_finder.add_constraint(new PointingConstraint("head", Constraint::vector3(-0.20, 0.0, 0.000), 0.0, 0), .2);
  
  d_pose_finder.add_constraint(new PointingMarkerConstraint("head", "right_hand", 0.0, 0), .3);

}


void MapThread::grasp_function() {     
  float height(.0);
  cout << "Building constraints" << endl;

  d_map_build_constraint = new MapBuildConstraint("right_hand", 2, .04, 0.1);
  d_marker = "right_hand";
  d_points = &(d_map_build_constraint->points());

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
