#include "demothreads.h"
#include "util.h"

#include <algorithm>
#include <string>
#include <cctype>

using namespace std;

MapThread::MapThread(KinematicModel::Model& model, KinematicModel::Robot& robot)
  : verbose(false), keepRunning(true), d_pose_finder(model, robot)
{
  //nullspace_function();
  hold_something_function();
  //		hand_left_function();
  //		hand_right_function();
//  hand_right_mark_function();
  //hand_right_look_varun_function();
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
  string load_file_name("map.save");
  string store_file_name("map.save");
  if (exists(load_file_name)) {
    cout << "loading Map " << load_file_name << endl;
    load_points(load_file_name);
  }
//  store_points(store_file_name);

  QTime time = QTime::currentTime();
  qsrand((uint)time.msec());

  bool filter(true);

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
      print_vector<double>(observation.markerPosition("head"));

      cout << n_collisions << endl;
      usleep(500000);
      cout << d_configuration_points.size() << endl;
      n = rand() % d_configuration_points.size();
    }
  else
    while (true) {
      d_pose_finder.find_pose(2000000, 0., 1.0e-6, .30, 50);
      add_best_pose();
      KinematicModel::RobotObservation observation = d_pose_finder.simulator().robot().observe();
      print_vector<double>(observation.markerPosition("head"));
      store_points(store_file_name);
    }
}


void MapThread::nullspace_function() {
  cout << "Building constraints" << endl;
  d_marker = "head";
  d_map_build_constraint = new MapBuildConstraint("head", 2, .02, .1);
  
  d_points = &(d_map_build_constraint->points());
  
  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), .03);
  
  //d_pose_finder.add_constraint(new PointingMarkerConstraint("left_hand", "right_hand", .1, 1, -1), .1);
  //d_pose_finder.add_constraint(new PointingMarkerConstraint("right_hand", "left_hand", .1, 1), .1);
  
  d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.3076, -0.0542, 0.06900)), 3.);
  d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.3076, 0.0542, 0.06900)), 3.);

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
  cout << "Building constraints" << endl;
  d_map_build_constraint = new MapBuildConstraint("left_hand", 2, .04, 0.1);
  d_marker = "left_hand";
  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 2);

  d_pose_finder.add_constraint(new PointingMarkerConstraint("left_hand", "right_hand", .1, 1, -1), .1);
  d_pose_finder.add_constraint(new PointingMarkerConstraint("right_hand", "left_hand", .1, 1), .1);
  
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

  d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

  d_pose_finder.add_constraint(d_map_build_constraint);

  //		d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

}

void MapThread::hand_left_function() {
  cout << "Building constraints" << endl;
  d_map_build_constraint = new MapBuildConstraint("left_hand", 2, .02, 0.1);
  d_marker = "left_hand";

  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), .1);

  d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));

  d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.)));

  //		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

  d_pose_finder.add_constraint(new PlaneConstraint(d_marker, 2, .0), 5.);

  d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

  d_pose_finder.add_constraint(d_map_build_constraint);

  d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

}

void MapThread::hand_right_function() {
  cout << "Building constraints" << endl;
  d_map_build_constraint = new MapBuildConstraint("right_hand", 2, .02, 0.1);
  d_marker = "right_hand";
  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), .1);

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
  cout << "Building constraints" << endl;
  d_map_build_constraint = new MapBuildConstraint("right_hand", 2, .04, 0.1);
  d_marker = "right_hand";
  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 2.);

  d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.10900)));

  d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

  //		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

  d_pose_finder.add_constraint(new PlaneConstraint(d_marker, 2, .0), 2.);

  d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

  d_pose_finder.add_constraint(d_map_build_constraint);

  d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 0, Constraint::vector3(0., 0., 1.)));
  d_pose_finder.add_constraint(new PointingConstraint("head", Constraint::vector3(-0.20, 0.0, 0.000), 0.0, 0));
  //d_pose_finder.add_constraint(new PositionConstraint("head", Constraint::vector3(0.00658101, 0.0396648, 0.204938)));

  //d_pose_finder.add_constraint(new PointingMarkerConstraint("head", "right_hand", 0.0, 0));

}
