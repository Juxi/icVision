#ifndef _PATH_PLANNER_H_
#define _PATH_PLANNER_H_

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <cmath>
#include <ctime>

#include "roadmap.h"
#include "poses_reader.h"
#include "posefinder.h"


class PathPlanner {
	typedef Roadmap::CGAL_Point Point;
	typedef Roadmap::vertex_i vertex_i;
	typedef Roadmap::vertex_t vertex_t;
	typedef Roadmap::edge_i edge_i;
	typedef Roadmap::edge_t edge_t;
	typedef Roadmap::path_t path_t;
	typedef Roadmap::Vertex Vertex;

	//typedef typename std::map<std::string, Roadmap>::iterator roadmap_iterator;
	typedef std::map<std::string, Roadmap*>::iterator roadmap_iterator; // C99: typename cannot be used outside a template declaration

public:
	
	PathPlanner(KinematicModel::Model& model, KinematicModel::Robot& robot, size_t dimensionality = 41);
	
	std::vector<double> nearestQ(std::string mapname, std::vector<double> &v, TreeMode mode=CONNECTIONMODE);
	std::vector<double> nearestQ(Roadmap &map, std::vector<double> &v, TreeMode mode=CONNECTIONMODE);
				
	//Roadmap::Path shortestPath_backup( Map::vertex_descriptor from, Map::vertex_descriptor to ); //DIJKSTRA
	path_t find_path(std::vector<double> &source_conf, std::vector<double> &target_conf, TreeMode mode=CONNECTIONMODE);

	Vertex &getVertex(int index, std::string mapname="");

	void connect_map(std::string mapname, size_t n);
	void connect_maps(size_t n);
	void connect_map2(std::string mapname, size_t n);
	void connect_maps2(size_t n);

	void clear_connections();
	void insert_poses(std::string mapname, poses_map_t &poses);

	void load_map(std::string mapname, std::string filename);
	void write_graph(std::string filename);
	void write_graph(std::string filename, std::string mapname);

	std::string info();

	//void update_maps();//check for collisions and change weights
	//void update_map(std::string mapname);
	//void update_map_2();

	std::string range_string(std::string map_name);
	std::string range_strings();

	std::vector<std::vector<double> > poses_to_configurations(poses_map_t &poses);
	std::vector<double> parse_scale_vector();
	std::vector<std::vector<double> > cut_pose(std::vector<double> &pose);

	Roadmap &roadmap(std::string mapname);
	bool hasMap(std::string mapname);
	bool check_map(std::string mapname);

	//Roadmap::path_t move_to_path(std::vector<double> source, std::vector<double> target);

	void add_bullshit();
	

private:
	PoseFinder d_posefinder;
	std::map<std::string, Roadmap*> d_roadmaps;	
	Roadmap d_main_roadmap;

	std::map<std::string, poses_map_t> d_poses;
	std::map<std::string, std::vector<double> > d_scale_map;
	std::vector<std::string> d_config_names;

	std::map<std::string, size_t> d_pose_sizes;
	size_t d_dimensionality;


public:
	class CollisionEdgeTester : public EdgeTester<edge_t>{
	public:
		Roadmap &d_roadmap;
		Simulator &d_simulator;
		double d_granularity;
		size_t d_n_evaluations;
		std::clock_t d_clock;
		size_t d_n_vskincalls;

		CollisionEdgeTester(Roadmap &roadmap, Simulator &simulator, double granularity) : d_roadmap(roadmap), d_simulator(simulator), d_granularity(granularity), d_n_evaluations(0), d_clock(0), d_n_vskincalls(0) {}

		void operator()(edge_t &edge);
		size_t n_evaluations(){return d_n_evaluations;}
		size_t n_vskincalls(){return d_n_vskincalls; }
		double n_seconds(){return static_cast<double>(d_clock) / CLOCKS_PER_SEC;}
	};
};

#endif
