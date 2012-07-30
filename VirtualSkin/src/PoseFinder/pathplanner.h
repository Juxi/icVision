#ifndef _PATH_PLANNER_H_
#define _PATH_PLANNER_H_

#include <vector>
#include <string>
#include <iostream>
#include <map>
//#include <CGAL/Homogeneous_d.h>
//#include <CGAL/Gmpz.h>
//#include <CGAL/Delaunay_d.h>

#include "roadmap.h"
#include "poses_reader.h"
#include "posefinder.h"


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
	Roadmap d_main_roadmap;
	
	std::map<std::string, poses_map_t> d_poses;
	std::map<std::string, std::vector<double> > d_scale_map;
	std::vector<std::string> d_config_names;

	std::map<std::string, size_t> d_pose_sizes;
	size_t d_dimensionality;

 public:
	
	PathPlanner(KinematicModel::Model& model, KinematicModel::Robot& robot, size_t dimensionality = 41) :
	d_posefinder(model, robot),
	  d_dimensionality(dimensionality)
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

		d_main_roadmap.setDimensionality(d_dimensionality);
		d_main_roadmap.scale_vector = get_scale_vector();
		//add_bullshit();
	  }
		  
	void load_map(std::string mapname, std::string filename) {
	  if (hasMap(mapname))
		throw StringException("map already exist");

	  poses_map_t poses = read_poses(filename);

	  if (poses.size() > 0) {
		d_roadmaps[mapname] = new Roadmap();
		insert_poses(mapname, poses);
	  } else
		throw StringException("Loading failed");
	}

	void connect_map(std::string mapname, size_t n) {
	  if (!hasMap(mapname))
		throw StringException("map doesnt exist");
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

	Roadmap &roadmap(std::string mapname) {	  
	  if (!hasMap(mapname))
		throw StringException("map doesnt exist");
	  return *d_roadmaps[mapname];
	}
	
	bool hasMap(std::string mapname) {return d_roadmaps.count(mapname) > 0; }

	void insert_poses(std::string mapname, poses_map_t &poses);

	std::vector<double> closest_configurationspace(std::string mapname, std::vector<double> conf);
	std::vector<double> closest_workspace(std::string mapname, std::vector<double> work);

	std::vector<std::vector<double> > find_path(std::vector<double> source_conf, std::vector<double> target_conf);

	std::vector<std::vector<double> > move_to_path(std::vector<double> source, std::vector<double> target);

	std::vector<std::vector<double> > cut_pose(std::vector<double> &pose);

	bool check_map(std::string mapname) {if (!hasMap(mapname)) throw StringException("Map does not exist");}

	void add_bullshit() {
		for (float z(-.2); z < .2; z += .03)
			d_posefinder.simulator().add_ball(-.3, 0.13, z);
	}

	std::vector<double> get_scale_vector();


};

#endif
