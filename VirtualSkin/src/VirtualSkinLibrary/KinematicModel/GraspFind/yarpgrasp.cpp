#include "yarpgrasp.h"
#include "exception.h"
#include <cmath>

using namespace std;

void YarpGrasp::run () {
	cout << "yarp grasp running" << endl;
	if (!i_cub.open("icubSimF"))
		throw StringException("Couldnt open port to icubsimf");

	load_map(d_map_file);
	connect_map(5);
	add_bullshit();
	update_map();
	vector<vector<double> > the_path = find_path();
//
	size_t counter(0);

	while (true) {
		vector<double> q(the_path[counter]);
		vector<double> q_norm = d_graspfinder.simulator().real_to_normal_motors(q);
		for (size_t i(0); i < q.size(); ++i)
			cout << q_norm[i] << " ";
		cout << endl;
		d_graspfinder.simulator().set_motors(q_norm);
		cout << "N COLLISIONS: " << d_graspfinder.simulator().computePose() << endl;

		int velocity(40);
		i_cub.setVelocity( velocity );
		if (!i_cub.positionMove(q))
			cout << "Position move error" << endl;
		bool motionInterrupted = !waitForMotion();

		msleep(500);
		counter = (counter + 1) % the_path.size();
	}
}


vector<vector<double> > YarpGrasp::poses_to_configurations() {
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

void YarpGrasp::insert_poses() {
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

vector<vector<double> > YarpGrasp::find_path() {
	vertex_t start(0);
	while (true) {
		start = rand() % num_vertices(d_roadmap.map);
		if (!(d_roadmap.map)[start].collisions)
			break;
	}

	vertex_t goal(0);
	while (true) {
		goal = rand() % num_vertices(d_roadmap.map);
		if (!(d_roadmap.map)[goal].collisions)
			break;
	}
	Roadmap::PathList the_path_nodes = d_roadmap.shortestPath(start, goal);
	vector<vector<double> > the_path = d_roadmap.vertex_list_to_q(the_path_nodes);
	return the_path;
}

//void YarpGrasp::update_map() {
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

void YarpGrasp::update_map() {
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
			n_collisions = max(simulator.computePose(), n_collisions);
		}

		double length = get(&Roadmap::Edge::length, d_roadmap.map, *edge_it);
		put(&Roadmap::Edge::length2, d_roadmap.map, *edge_it, length + 100000. * n_collisions);

//		size_t n_collisions =  d_roadmap.map[source(*edge_it, d_roadmap.map)].collisions + d_roadmap.map[target(*edge_it, d_roadmap.map)].collisions;
		//d_roadmap.map[*edge_it].length = 1. + n_collisions * 99999.;+


		//length should be distance of q's
	}

//		d_road_map.insert(0, 0, configurations[i], fitnesses[i], collisions[i]);
}
