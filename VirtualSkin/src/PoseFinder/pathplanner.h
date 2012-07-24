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
#include "posefinder.h"
#include <map>

template <typename T>
inline std::vector<T> string_to_vector(std::string str) {
	std::vector<T> vec;
	std::istringstream iss(str.c_str());
	while (true) {
		T val;
		iss >> val;
		if (!iss)
			break;
		vec.push_back(val);
	}
	return vec;
}


class PathPlanner {

	typedef Roadmap::CGAL_Point Point;
	typedef Roadmap::vertex_i vertex_i;
	typedef Roadmap::vertex_t vertex_t;
	typedef Roadmap::edge_i edge_i;
	typedef Roadmap::edge_t edge_t;
	//typedef typename std::map<std::string, Roadmap>::iterator roadmap_iterator;
	typedef std::map<std::string, Roadmap*>::iterator roadmap_iterator; // C99: typename cannot be used outside a template declaration

	PoseFinder d_posefinder;
	std::map<std::string, Roadmap*> d_roadmaps;
	
	std::map<std::string, poses_map_t> d_poses;
	std::map<std::string, std::vector<double> > d_scale_map;
	std::vector<std::string> d_config_names;

	std::map<std::string, size_t> d_pose_sizes;

 public:
	
 PathPlanner(KinematicModel::Model& model, KinematicModel::Robot& robot) :
	d_posefinder(model, robot)
	  {
		d_config_names.push_back("CFGSPACE_TORSO");
		d_config_names.push_back("CFGSPACE_HEAD");
		d_config_names.push_back("CFGSPACE_RIGHT_ARM");
		d_config_names.push_back("CFGSPACE_LEFT_ARM");

		//hack
		d_scale_map["CFGSPACE_TORSO"] = string_to_vector<double>("1.0 1.0 1.0");
		d_scale_map["CFGSPACE_HEAD"] = string_to_vector<double>("1.0 1.0 1.0 0.0 0.0 0.0");
		d_scale_map["CFGSPACE_RIGHT_ARM"] = string_to_vector<double>("1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0");
		d_scale_map["CFGSPACE_LEFT_ARM"]  = string_to_vector<double>("1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0");
		
		
		d_pose_sizes["CFGSPACE_TORSO"] = d_scale_map["CFGSPACE_TORSO"].size();
		d_pose_sizes["CFGSPACE_HEAD"] = d_scale_map["CFGSPACE_HEAD"].size();
		d_pose_sizes["CFGSPACE_RIGHT_ARM"] = d_scale_map["CFGSPACE_RIGHT_ARM"].size();
		d_pose_sizes["CFGSPACE_LEFT_ARM"] = d_scale_map["CFGSPACE_LEFT_ARM"].size();
		
		//add_bullshit();
	  }
		  
	void load_map(std::string mapname, std::string filename) {
	  d_roadmaps[mapname] = new Roadmap();
	  poses_map_t poses = read_poses(filename);
	  d_roadmaps[mapname]->scale_vector = get_scale_vector();
	  insert_poses(mapname, poses);
	}

	void connect_map(std::string mapname, size_t n) {
		d_roadmaps[mapname]->graphConnect(n, SCALEDCONFIGURATIONSPACE);
	}
	
	void connect_maps(size_t n) {
	  roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
	  
	  for (; it != it_end; ++it)
		connect_map(it->first, n);
	}


	void update_maps();//check for collisions and change weights
	void update_map(std::string mapname);

//	void update_map_2();

	std::vector<std::vector<double> > poses_to_configurations(poses_map_t &poses);

	Roadmap &roadmap(std::string mapname) {return *d_roadmaps[mapname];}

	void insert_poses(std::string mapname, poses_map_t &poses);

	std::vector<std::vector<double> > find_path(std::vector<double> source, std::vector<double> target, std::string mapname = "default");
	std::vector<std::vector<double> > find_workspace_path(std::vector<double> source, std::vector<double> target, std::string mapname = "default");
	std::vector<std::vector<double> > find_configuration_workspace_path(std::vector<double> source, std::vector<double> target, std::string mapname = "default");


	std::vector<std::vector<double> > cut_pose(std::vector<double> &pose);

	void add_bullshit() {
		for (float z(-.2); z < .2; z += .03)
			d_posefinder.simulator().add_ball(-.3, 0.13, z);
	}

	std::vector<double> get_scale_vector();


};

#endif
