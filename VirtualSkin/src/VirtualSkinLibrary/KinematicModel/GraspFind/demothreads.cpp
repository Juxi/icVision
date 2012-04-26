#include "demothreads.h"

using namespace std;

MapThread::MapThread(KinematicModel::Model& model, KinematicModel::Robot& robot)
  : verbose(false), keepRunning(true), d_pose_finder(model, robot)
{
  //nullspace_function();
  hold_something_function();
  //		hand_left_function();
  //		hand_right_function();
}


void MapThread::run()
{
  string load_file_name("map.save");
  string store_file_name("map.save");
  if (exists(load_file_name)) {
    std::cout << "loading Map " << load_file_name << std::endl;
    load_points(load_file_name);
  }
  store_points(store_file_name);

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
      std::cout << "Testing!: "<< n << std::endl;
      //				n = 279;
      std::vector<double> random_pose = d_configuration_points[n];//d_pose_finder.simulator().home_pos();
      for (size_t i(0); i < random_pose.size(); ++i)
	std::cout << random_pose[i] << " ";
      std::cout << std::endl;
      d_pose_finder.simulator().set_motors(random_pose);
      double n_collisions = d_pose_finder.simulator().computePose();
      std::cout << n_collisions << std::endl;
      usleep(500000);
      n = (n + 1) % d_configuration_points.size();
    }
  else
    while (true) {
      d_pose_finder.find_pose(2000000, 0., 1.0e-6, .20, 100);
      add_best_pose();
      store_points(store_file_name);
    }
}


void MapThread::nullspace_function() {
  std::cout << "Building constraints" << std::endl;
  d_marker = "head";
  d_map_build_constraint = new MapBuildConstraint("head", 2, .02, .01);
  d_points = &(d_map_build_constraint->points());
  
  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), .1);
  
  //		d_pose_finder.add_constraint(new PointingMarkerConstraint("left_hand", "right_hand", .1, 1, -1), .1);
  //		d_pose_finder.add_constraint(new PointingMarkerConstraint("right_hand", "left_hand", .1, 1), .1);
  
  d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2176, -0.1642, 0.16900)), 3.);
  d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2176, 0.1642, 0.16900)), 3.);

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
  std::cout << "Building constraints" << std::endl;
  d_map_build_constraint = new MapBuildConstraint("left_hand", 2, .02, 0.01);
  d_marker = "left_hand";
  d_points = &(d_map_build_constraint->points());

  d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 1);

  d_pose_finder.add_constraint(new PointingMarkerConstraint("left_hand", "right_hand", .1, 1, -1), .1);
  d_pose_finder.add_constraint(new PointingMarkerConstraint("right_hand", "left_hand", .1, 1), .1);
  
  d_pose_finder.add_constraint(new AveragePositionConstraint("right_hand", "left_hand", Constraint::vector3(-0.2376, 0.0, 0.03900)), .2);
  //		d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));

  //		d_pose_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.)));

  //		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

  //		d_pose_finder.add_constraint(new PlaneConstraint(d_marker, 2, .0), 5.);

  d_pose_finder.add_constraint(new CollisionConstraint(), 1.);

  d_pose_finder.add_constraint(d_map_build_constraint);

  //		d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 0, Constraint::vector3(0., 0., 1.)));

}

void MapThread::hand_left_function() {
  std::cout << "Building constraints" << std::endl;
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
  std::cout << "Building constraints" << std::endl;
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

  //d_pose_finder.add_constraint(new PointingMarkerConstraint("head", "right_hand", 0.0, 0));

}
