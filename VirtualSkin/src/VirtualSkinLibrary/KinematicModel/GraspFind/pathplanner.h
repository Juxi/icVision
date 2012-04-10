#ifndef _PATH_PLANNER_H_
#define _PATH_PLANNER_H_

#include <vector>
#include <string>
#include <iostream>

//#include <CGAL/Homogeneous_d.h>
//#include <CGAL/Gmpz.h>
//#include <CGAL/Delaunay_d.h>

#include "roadmap.h"
#include "poses_reader.h"
#include "graspfinder.h"

class PathPlanner {

	typedef Roadmap::CGAL_Point Point;
	typedef Roadmap::vertex_i vertex_i;
	typedef Roadmap::vertex_t vertex_t;
	typedef Roadmap::edge_i edge_i;
	typedef Roadmap::edge_t edge_t;


	GraspFinder d_graspfinder;
	Roadmap d_roadmap;

	poses_map_t d_poses;
	std::vector<std::string> d_config_names;

	std::string d_map_file;

public:

	PathPlanner(KinematicModel::Model& model, KinematicModel::Robot& robot, std::string map_file) :
		  d_graspfinder(model, robot),
		  d_map_file(map_file)
	{
		d_config_names.push_back("CFGSPACE_TORSO");
		d_config_names.push_back("CFGSPACE_HEAD");
		d_config_names.push_back("CFGSPACE_RIGHT_ARM");
		d_config_names.push_back("CFGSPACE_LEFT_ARM");
		if (d_map_file.size())
			load_map(d_map_file);
	}

	void load_map(std::string filename) {
		d_poses = read_poses(filename);
		insert_poses();
	}

	void connect_map(size_t n) {
		d_roadmap.graphConnect(n, CONFIGURATIONSPACE);
	}

	void update_map();//check for collisions and change weights
//	void update_map_2();

	std::vector<std::vector<double> > poses_to_configurations();

	Roadmap &roadmap() {return d_roadmap;}

	void insert_poses();

	std::vector<std::vector<double> > find_path(std::vector<double> source, std::vector<double> target);
	std::vector<std::vector<double> > find_workspace_path(std::vector<double> source, std::vector<double> target);

	size_t n_poses() {
		return d_poses["WORKSPACE"].size();
	}

	void add_bullshit() {
		for (float z(-.2); z < .2; z += .03)
			d_graspfinder.simulator().add_ball(-.3, -0.1, z);
	}


};

#endif
