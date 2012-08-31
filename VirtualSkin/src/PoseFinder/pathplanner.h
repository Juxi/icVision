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
	
	Roadmap::vertex_t nearestMainMapVertex(std::vector<double> &v, TreeMode mode, std::string mapname="");
	Roadmap::vertex_t nearestVertex(std::vector<double> &v, TreeMode mode, std::string mapname="");

	//Roadmap::Path shortestPath_backup( Map::vertex_descriptor from, Map::vertex_descriptor to ); //DIJKSTRA
	path_t find_path(vertex_t &source, vertex_t &target, TreeMode mode=CONNECTIONMODE, std::string mapname="");

	Vertex &getVertex(vertex_t index, std::string mapname="");
	Roadmap &get_map(std::string mapname="");

	void connect_map(size_t n, TreeMode mode, std::string mapname="");
	void connect_map2(size_t n, TreeMode mode, std::string mapname="");

	//void clear_connections();
	void insert_poses(std::string mapname, poses_map_t &poses);

	void load_map(std::string mapname, std::string filename);
	void write_graph(std::string filename, std::string mapname="");

	std::string info();

	//void update_maps();//check for collisions and change weights
	//void update_map(std::string mapname);
	//void update_map_2();

	std::string range_string(std::string map_name);
	std::string range_strings();

	std::vector<std::vector<double> > poses_to_configurations(poses_map_t &poses);
	std::vector<double> parse_scale_vector();
	std::vector<std::vector<double> > cut_pose(std::vector<double> &pose);
	std::vector<double> scale_q( std::vector<double> q, std::string mapname="");
	std::vector<double> unscale_q( std::vector<double> sq, std::string mapname="");

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
		TreeMode conmode;

		CollisionEdgeTester(Roadmap &roadmap, Simulator &simulator, double granularity, TreeMode mode=CONNECTIONMODE);

		void operator()(edge_t &edge);
		size_t n_evaluations(){return d_n_evaluations;}
		size_t n_vskincalls(){return d_n_vskincalls; }
		double n_seconds(){return static_cast<double>(d_clock) / CLOCKS_PER_SEC;}
	};


	class CollisionVertexTester : public Roadmap::VertexTester {
	public:
		Roadmap &d_roadmap;
		Simulator &d_simulator;
		double d_granularity;
		
		CollisionVertexTester(Roadmap &roadmap, Simulator &simulator, double granularity) :
		d_roadmap(roadmap), d_simulator(simulator), d_granularity(granularity) {};

		virtual bool check(std::vector<double> &q1, std::vector<double> &q2);
	};
};

#endif
