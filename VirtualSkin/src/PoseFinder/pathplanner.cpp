#include "pathplanner.h"
#include "exception.h"
#include <cmath>
#include <algorithm>

using namespace std;


vector<vector<double> > PathPlanner::poses_to_configurations(poses_map_t &poses) {
  size_t map_size(poses["WORKSPACE"].size());
	vector<vector<double> > configurations;

	for (size_t i(0); i < map_size; ++i) {

		vector<double> configuration;
		for (size_t n(0); n < d_config_names.size(); ++n) {
//			cout << d_config_names[n] << endl;
			copy(poses[d_config_names[n]][i].begin(), poses[d_config_names[n]][i].end(), back_inserter(configuration));
		}

//		for (size_t n(0); n < configuration.size(); ++n)
//			cout << configuration[n] << " ";
//		cout << endl;
		configurations.push_back(configuration);
	}

	return configurations;
}

vector<double> PathPlanner::get_scale_vector() {
	vector<double> scale_vector;
	for (size_t n(0); n < d_config_names.size(); ++n) {
	  copy(d_scale_map[d_config_names[n]].begin(), d_scale_map[d_config_names[n]].end(), back_inserter(scale_vector));
	}

	return scale_vector;
}

void PathPlanner::insert_poses(std::string mapname, poses_map_t &poses) {
  check_map(mapname);

  vector<vector<double> > configurations = poses_to_configurations(poses);
	//cout << configurations.size() << endl;
	int dimensionality(configurations[0].size());
	cout << "INSERTING POSES" << endl;

	d_roadmaps[mapname]->setDimensionality(d_dimensionality);
	d_roadmaps[mapname]->scale_vector = get_scale_vector();
			
	for (size_t i(0); i < configurations.size(); ++i) {
		vector<double> &q_configuration(configurations[i]);
		vector<double> &x_configuration(poses["WORKSPACE"][i]);
		d_roadmaps[mapname]->insert(x_configuration, q_configuration, mapname);
		d_main_roadmap.insert(x_configuration, q_configuration, mapname);
	}
}

vector<double> PathPlanner::closest_configurationspace(string mapname, vector<double> conf) {
  check_map(mapname);
  return d_roadmaps[mapname]->nearestVector(conf);
}

vector<double> PathPlanner::closest_workspace(string mapname, vector<double> work) {
  check_map(mapname);
  return d_roadmaps[mapname]->nearestWorkspaceVector(work);
}

vector<vector<double> > PathPlanner::find_path(vector<double> source, vector<double> target) {
	Roadmap::PathList the_path_nodes = d_main_roadmap.shortestPath(source, target);
	vector<vector<double> > the_path = d_main_roadmap.vertex_list_to_q(the_path_nodes);
	cout << "Path len: " << the_path.size() << endl;
	return the_path;
}

vector<vector<double> > PathPlanner::move_to_path(vector<double> source, vector<double> target) {
  vector<vector<double> > the_path;
  the_path.push_back(source);
  the_path.push_back(target);
  return the_path;
}

vector<vector<double> > PathPlanner::cut_pose(std::vector<double> &pose) {
	vector<vector<double> > cut_pose;
	size_t counter(0);
	for (size_t i(0); i < d_config_names.size(); ++i) {
		size_t len(d_pose_sizes[d_config_names[i]]);
		//cut_pose.push_back(vector<double>(&pose[counter], &pose[counter + len]));
		cut_pose.push_back(vector<double>(pose.begin() + counter, pose.begin() + counter + len));
		counter += len;
	}
	return cut_pose;
}

//void PathPlanner::update_map() {
//	//get simulator
//	Simulator &simulator(d_posefinder.simulator());
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


string PathPlanner::range_string(string map_name) {
  check_map(map_name);
  ostringstream oss("range:");
  try {
	oss << "range of map [" << map_name << "]" << endl;
	pair<vector<float>, vector<float> > bbox = roadmap(map_name).get_workspace_bounding_box();
	
	oss << "[";
	for (size_t i(0); i < bbox.first.size(); ++i)
							  oss << bbox.first[i] << " ";
	oss << "] [";
	for (size_t i(0); i < bbox.second.size(); ++i)
	  oss << bbox.second[i] << " ";
	oss << "]" << endl;
  }
  catch (...) {
	oss << "Failed" << endl;
  }
  return oss.str();
}

string PathPlanner::range_strings() {
  roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
  
  string str;
  for (; it != it_end; ++it)
	str += range_string(it->first) + "\n";
  return str;
}


void PathPlanner::update_map(std::string mapname) {
	//get simulator
	Simulator &simulator(d_posefinder.simulator());

	//loop over vertexes
	Roadmap &roadmap(*d_roadmaps[mapname]);
	pair<vertex_i, vertex_i> map_vertices(vertices(roadmap.map));
	vertex_i vertex_it(map_vertices.first);

	for (; vertex_it != map_vertices.second; ++vertex_it) {
		vector<double> q_norm(roadmap.map[*vertex_it].q);
		vector<double> q = simulator.real_to_normal_motors(q_norm);

		simulator.set_motors(q);
		size_t n_collisions = simulator.computePose();
		
		put(&Roadmap::Vertex::collisions, roadmap.map, *vertex_it, n_collisions);
		roadmap.map[*vertex_it].collisions = n_collisions;
	}

	//loop all edges
	pair<edge_i, edge_i> map_edges(edges(roadmap.map));
	edge_i edge_it(map_edges.first);
	for (; edge_it != map_edges.second; ++edge_it) {
		vector<double> q_start = roadmap.map[source(*edge_it, roadmap.map)].q;
		vector<double> q_end = roadmap.map[target(*edge_it, roadmap.map)].q;
		size_t resolution(10);
		int n_collisions = roadmap.map[source(*edge_it, roadmap.map)].collisions + roadmap.map[source(*edge_it, roadmap.map)].collisions;

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

		double length = get(&Roadmap::Edge::length, roadmap.map, *edge_it);
		put(&Roadmap::Edge::length2, roadmap.map, *edge_it, length + 100000. * n_collisions);

//		size_t n_collisions =  roadmap.map[source(*edge_it, roadmap.map)].collisions + roadmap.map[target(*edge_it, roadmap.map)].collisions;
		//roadmap.map[*edge_it].length = 1. + n_collisions * 99999.;+


		//length should be distance of q's
	}
//		road_map.insert(0, 0, configurations[i], fitnesses[i], collisions[i]);
}


void PathPlanner::update_maps() {
  roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
  
  for (; it != it_end; ++it)
	update_map(it->first);
}
