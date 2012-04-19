#include "pathplanner.h"
#include "exception.h"
#include <cmath>
#include <algorithm>

using namespace std;


vector<vector<double> > PathPlanner::poses_to_configurations() {
	size_t map_size(n_poses());
	vector<vector<double> > configurations;

	for (size_t i(0); i < map_size; ++i) {

		vector<double> configuration;
		for (size_t n(0); n < d_config_names.size(); ++n) {
//			cout << d_config_names[n] << endl;
			copy(d_poses[d_config_names[n]][i].begin(), d_poses[d_config_names[n]][i].end(), back_inserter(configuration));
		}

//		for (size_t n(0); n < configuration.size(); ++n)
//			cout << configuration[n] << " ";
//		cout << endl;
		configurations.push_back(configuration);
	}

	return configurations;
}

void PathPlanner::insert_poses() {
	vector<vector<double> > configurations = poses_to_configurations();
	cout << configurations.size() << endl;
	int dimensionality(configurations[0].size());
	cout << "INSERTING POSES" << endl;

	d_roadmap.setDimensionality(dimensionality);

	for (size_t i(0); i < configurations.size(); ++i) {
		vector<double> &q_configuration(configurations[i]);
		vector<double> &x_configuration(d_poses["WORKSPACE"][i]);
		d_roadmap.insert(x_configuration, q_configuration);
	}
}

vector<vector<double> > PathPlanner::find_path(vector<double> source, vector<double> target) {
	Roadmap::PathList the_path_nodes = d_roadmap.shortestPath(source, target);
	vector<vector<double> > the_path = d_roadmap.vertex_list_to_q(the_path_nodes);
	return the_path;
}

vector<vector<double> > PathPlanner::find_workspace_path(vector<double> source, vector<double> target) {
	Roadmap::PathList the_path_nodes = d_roadmap.shortestWorkspacePath(source, target);
	vector<vector<double> > the_path = d_roadmap.vertex_list_to_q(the_path_nodes);
	return the_path;
}

vector<vector<double> > PathPlanner::cut_pose(std::vector<double> &pose) {
	vector<vector<double> > cut_pose;
	size_t counter(0);
	for (size_t i(0); i < d_config_names.size(); ++i) {
		size_t len(d_poses[d_config_names[i]][0].size());
		cut_pose.push_back(vector<double>(&pose[counter], &pose[counter + len]));
		counter += len;
	}
	return cut_pose;
}

//void PathPlanner::update_map() {
//	//get simulator
//	Simulator &simulator(d_graspfinder.simulator());
//
//	//loop over vertexes
//	pair<vertex_i, vertex_i> map_vertices(vertices(d_roadmap.map));
//	vertex_i vertex_it(map_vertices.first);
//
//	for (; vertex_it != map_vertices.second; ++vertex_it) {
//		vector<double> q_norm(d_roadmap.map[*vertex_it].q);
//		vector<double> q = simulator.real_to_normal_motors(q_norm);
//
//		simulator.set_motors(q);
//		size_t n_collisions = simulator.computePose();
//
//		cout << n_collisions << endl;
//		d_roadmap.map[*vertex_it].collisions = n_collisions;
//	}
//
//	//loop all edges
//	pair<edge_i, edge_i> map_edges(edges(d_roadmap.map));
//	edge_i edge_it(map_edges.first);
//	for (; edge_it != map_edges.second; ++edge_it) {
//		size_t n_collisions =  d_roadmap.map[source(*edge_it, d_roadmap.map)].collisions + d_roadmap.map[target(*edge_it, d_roadmap.map)].collisions;
//		//d_roadmap.map[*edge_it].length = 1. + n_collisions * 99999.;+
//		double length = get(&Roadmap::Edge::length, d_roadmap.map, *edge_it);
//		put(&Roadmap::Edge::length2, d_roadmap.map, *edge_it, length + 100000. * n_collisions);
//
//		//length should be distance of q's
//	}
//
////		d_road_map.insert(0, 0, configurations[i], fitnesses[i], collisions[i]);
//}

void PathPlanner::update_map() {
	//get simulator
	Simulator &simulator(d_graspfinder.simulator());

	//loop over vertexes
	pair<vertex_i, vertex_i> map_vertices(vertices(d_roadmap.map));
	vertex_i vertex_it(map_vertices.first);

	for (; vertex_it != map_vertices.second; ++vertex_it) {
		vector<double> q_norm(d_roadmap.map[*vertex_it].q);
		vector<double> q = simulator.real_to_normal_motors(q_norm);

		simulator.set_motors(q);
		size_t n_collisions = simulator.computePose();
		
		put(&Roadmap::Vertex::collisions, d_roadmap.map, *vertex_it, n_collisions);
		d_roadmap.map[*vertex_it].collisions = n_collisions;

	}
	//loop all edges
	pair<edge_i, edge_i> map_edges(edges(d_roadmap.map));
	edge_i edge_it(map_edges.first);
	for (; edge_it != map_edges.second; ++edge_it) {
		vector<double> q_start = d_roadmap.map[source(*edge_it, d_roadmap.map)].q;
		vector<double> q_end = d_roadmap.map[target(*edge_it, d_roadmap.map)].q;
		size_t resolution(10);
		int n_collisions = d_roadmap.map[source(*edge_it, d_roadmap.map)].collisions + d_roadmap.map[source(*edge_it, d_roadmap.map)].collisions;

		for (size_t i(0); i < resolution; ++i) {
			if (n_collisions > 0)
				break;
			float portion(static_cast<float>(i) / resolution);
			vector<double> q(q_start.size());
			for (size_t n(0); n < q.size(); ++n)
				q[n] = q_start[n] * portion + q_end[n] * (1. - portion);
			vector<double> q_real = simulator.real_to_normal_motors(q);
			simulator.set_motors(q_real);
			n_collisions += max(simulator.computePose(), n_collisions);
		}

		double length = get(&Roadmap::Edge::length, d_roadmap.map, *edge_it);
		put(&Roadmap::Edge::length2, d_roadmap.map, *edge_it, length + 100000. * n_collisions);

//		size_t n_collisions =  d_roadmap.map[source(*edge_it, d_roadmap.map)].collisions + d_roadmap.map[target(*edge_it, d_roadmap.map)].collisions;
		//d_roadmap.map[*edge_it].length = 1. + n_collisions * 99999.;+


		//length should be distance of q's
	}
//		d_road_map.insert(0, 0, configurations[i], fitnesses[i], collisions[i]);
}
