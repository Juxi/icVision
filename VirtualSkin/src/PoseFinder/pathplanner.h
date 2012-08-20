#ifndef _PATH_PLANNER_H_
#define _PATH_PLANNER_H_

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <cmath>
#include <ctime>

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
	
	void clear() {
	  roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
	  
	  for (; it != it_end; ++it)
		delete(it->second);
	  
	  d_roadmaps.clear();
	  d_main_roadmap.clear();
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
	  check_map(mapname);
	  d_roadmaps[mapname]->graphConnect(n, SCALEDCONFIGURATIONSPACE);
	}
	
	void connect_maps(size_t n) {
	  clear_connections();

	  if (d_main_roadmap.size() == 0)
		throw StringException("No maps loaded yet");
	  roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
	  
	  for (; it != it_end; ++it)
		connect_map(it->first, n);

	  d_main_roadmap.graphConnect(n, SCALEDCONFIGURATIONSPACE);
	  d_main_roadmap.random_connect(d_main_roadmap.size() / 10);
	  //d_main_roadmap.connect_delaunay();
	}

	void clear_connections() {
	  roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());

	  for (; it != it_end; ++it)
		it->second->removeAllEdges();
	  d_main_roadmap.removeAllEdges();
	}
	
	std::string info() {
	  std::ostringstream oss;
	  oss << " ==INFO== " << std::endl;
	  oss << d_roadmaps.size() << " roadmaps:" << std::endl;

	  roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
	  for (; it != it_end; ++it)
		oss << "[" << it->first << "] : " << it->second->size() << " poses, " << it->second->n_edges() << " edges" << std::endl;
	  oss << std::endl << "total: " << d_main_roadmap.size() << " poses, " << d_main_roadmap.n_edges() << " edges" << std::endl;
	  return oss.str();
	}

	//void update_maps();//check for collisions and change weights
	//void update_map(std::string mapname);

	std::string range_string(std::string map_name);
	std::string range_strings();

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

	bool check_map(std::string mapname) {if (!hasMap(mapname)) throw StringException("Map does not exist"); return true; }

	void add_bullshit() {
		for (float z(-.2); z < .2; z += .03)
			d_posefinder.simulator().add_ball(-.3, 0.13, z);
	}

	std::vector<double> get_scale_vector();



	class CollisionEdgeTester : public EdgeTester<edge_t>{
	public:
	  Roadmap &d_roadmap;
	  Simulator &d_simulator;
	  double d_granularity;
	  size_t d_n_evaluations;
	  std::clock_t d_clock;

	CollisionEdgeTester(Roadmap &roadmap, Simulator &simulator, double granularity) : d_roadmap(roadmap), d_simulator(simulator), d_granularity(granularity), d_n_evaluations(0), d_clock(0) {}
	  
	  void operator()(edge_t &edge) {
		if (get(&Roadmap::Edge::evaluated, d_roadmap.map, edge))
		  return;

		std::clock_t start_clock(std::clock());
		std::vector<double> &q_start = d_roadmap.map[source(edge, d_roadmap.map)].q;
		std::vector<double> &q_end = d_roadmap.map[target(edge, d_roadmap.map)].q;
		
		int n_collisions = d_roadmap.map[source(edge, d_roadmap.map)].collisions + d_roadmap.map[source(edge, d_roadmap.map)].collisions;

		//adaptive resolution, at least 2 (check start and end)
		size_t resolution = std::floor(std::max(2., calculate_distance(q_start, q_end) / d_granularity));

		for (size_t i(0); i < resolution; ++i) {
		  if (n_collisions > 0)
			break;
		  float portion(static_cast<float>(i) / (resolution - 1));
		  std::vector<double> q(q_start.size());
		  for (size_t n(0); n < q.size(); ++n)
			q[n] = q_start[n] * portion + q_end[n] * (1. - portion);
		  std::vector<double> q_real = d_simulator.real_to_normal_motors(q);
		  d_simulator.set_motors(q_real);
		  n_collisions = std::max(d_simulator.computePose(), n_collisions);
		}
		
		double length = get(&Roadmap::Edge::length, d_roadmap.map, edge);
		if (!n_collisions)
		  put(&Roadmap::Edge::length2, d_roadmap.map, edge, length);
		else
		  put(&Roadmap::Edge::length2, d_roadmap.map, edge, FLT_MAX);
		put(&Roadmap::Edge::evaluated, d_roadmap.map, edge, true);

		++d_n_evaluations;
		std::clock_t end_clock(std::clock());
		d_clock += end_clock - start_clock;
	  }

	  size_t n_evaluations(){return d_n_evaluations;}

	  double n_seconds(){return static_cast<double>(d_clock) / CLOCKS_PER_SEC;}
	};

	
};

#endif
