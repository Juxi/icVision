#include "pathplanner.h"
#include "exception.h"
#include <cmath>
#include <algorithm>

using namespace std;


PathPlanner::PathPlanner(KinematicModel::Model& model, KinematicModel::Robot& robot, size_t dimensionality) :
d_posefinder(model, robot), d_dimensionality(dimensionality)
{
	d_config_names.push_back("CFGSPACE_TORSO");
	d_config_names.push_back("CFGSPACE_HEAD");
	d_config_names.push_back("CFGSPACE_RIGHT_ARM");
	d_config_names.push_back("CFGSPACE_LEFT_ARM");

	//hack
	/*d_scale_map["CFGSPACE_TORSO"] = string_to_vector<double>("1.0 1.0 1.0");
	d_scale_map["CFGSPACE_HEAD"] = string_to_vector<double>("1.0 1.0 1.0 0.0 0.0 0.0");
	d_scale_map["CFGSPACE_RIGHT_ARM"] = string_to_vector<double>("1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0");
	d_scale_map["CFGSPACE_LEFT_ARM"]  = string_to_vector<double>("1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0");*/

	//hack2
	d_scale_map["CFGSPACE_TORSO"] = string_to_vector<double>("0.75 0.75 1.0");
	d_scale_map["CFGSPACE_HEAD"] = string_to_vector<double>("0.1 0.1 0.1 0.0 0.0 0.0");
	d_scale_map["CFGSPACE_RIGHT_ARM"] = string_to_vector<double>("0.5 0.5 0.5 0.1 0.1 0.1 0.1 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0");
	d_scale_map["CFGSPACE_LEFT_ARM"] = string_to_vector<double>("0.5 0.5 0.5 0.1 0.1 0.1 0.1 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0");


	d_pose_sizes["CFGSPACE_TORSO"] = d_scale_map["CFGSPACE_TORSO"].size();
	d_pose_sizes["CFGSPACE_HEAD"] = d_scale_map["CFGSPACE_HEAD"].size();
	d_pose_sizes["CFGSPACE_RIGHT_ARM"] = d_scale_map["CFGSPACE_RIGHT_ARM"].size();
	d_pose_sizes["CFGSPACE_LEFT_ARM"] = d_scale_map["CFGSPACE_LEFT_ARM"].size();

	d_main_roadmap.setDimensionality(d_dimensionality);
	d_main_roadmap.setScaleVector(parse_scale_vector());
	//add_bullshit();
}


vector<double> PathPlanner::parse_scale_vector() {
	vector<double> scale_vector;
	for (size_t n(0); n < d_config_names.size(); ++n) {
		copy(d_scale_map[d_config_names[n]].begin(), d_scale_map[d_config_names[n]].end(), back_inserter(scale_vector));
	}

	return scale_vector;
}


void PathPlanner::load_map(std::string mapname, std::string filename) {
	if (hasMap(mapname))
		throw StringException("map already exist");

	poses_map_t poses = read_poses(filename);

	if (poses.size() > 0) {
		d_roadmaps[mapname] = new Roadmap();
		insert_poses(mapname, poses);
	} else
		throw StringException("Loading failed");
}


void PathPlanner::connect_map(std::string mapname, size_t n) {
	check_map(mapname);
	d_roadmaps[mapname]->graphConnect(n, SCALEDCONFIGURATIONSPACE);
	//d_roadmaps[mapname]->graphConnect(n, WORKSPACE);
}


void PathPlanner::connect_maps(size_t n) {
	clear_connections();

	if (d_main_roadmap.size() == 0)
		throw StringException("No maps loaded yet");
	
	//roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
	//for (; it != it_end; ++it)
	//connect_map(it->first, n);

	d_main_roadmap.graphConnect(n, SCALEDCONFIGURATIONSPACE);
	//d_main_roadmap.graphConnect(n, WORKSPACE);
	//d_main_roadmap./*om_connect(d_main_roadmap.size() / 10);
	//d_main_roadmap.connect_delaunay();
}


void PathPlanner::connect_map2(std::string mapname, size_t n) {
	check_map(mapname);
	d_roadmaps[mapname]->graphConnect2(n, SCALEDCONFIGURATIONSPACE);
}


void PathPlanner::connect_maps2(size_t n) {
	clear_connections();

	if (d_main_roadmap.size() == 0)
		throw StringException("No maps loaded yet");
	roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());

	d_main_roadmap.graphConnect2(n, SCALEDCONFIGURATIONSPACE);	  
}


void PathPlanner::clear_connections() {
	roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());

	for (; it != it_end; ++it)
		it->second->removeAllEdges();
	d_main_roadmap.removeAllEdges();
}


void PathPlanner::write_graph(std::string filename) {
	d_main_roadmap.write_graph(filename);
}


void PathPlanner::write_graph(std::string filename, std::string mapname) {
	check_map(mapname);
	d_roadmaps[mapname]->write_graph(filename);
}


std::string PathPlanner::info() {
	std::ostringstream oss;
	oss << " ==INFO== " << std::endl;
	oss << d_roadmaps.size() << " roadmaps:" << std::endl;

	roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
	for (; it != it_end; ++it)
		oss << "[" << it->first << "] : " << it->second->size() << " poses, " << it->second->n_edges() << " edges" << std::endl;
	oss << std::endl << "total: " << d_main_roadmap.size() << " poses, " << d_main_roadmap.n_edges() << " edges" << std::endl;
	return oss.str();
}


Roadmap & PathPlanner::roadmap(std::string mapname) {	  
	if (!hasMap(mapname))
		throw StringException("map doesnt exist");
	return *d_roadmaps[mapname];
}


bool PathPlanner::hasMap(std::string mapname) {
	return d_roadmaps.count(mapname) > 0; 
}


Roadmap::Vertex &PathPlanner::getVertex(int index, std::string mapname) {
	if (mapname == "") {
		if ( index >=0 && index < d_main_roadmap.size())
			return d_main_roadmap.getMap()[index];
		else
			throw StringException("Vertex out of range");
	} else if (!hasMap(mapname)) {
		throw StringException("Map does not exist");
	} else {
		if ( index >=0 && index < d_roadmaps[mapname]->size())
			return d_roadmaps[mapname]->getMap()[index];
		else
			throw StringException("Vertex out of range");
	}
}

bool PathPlanner::check_map(std::string mapname) {
	if (!hasMap(mapname)) throw StringException("Map does not exist"); 
	return true; 
}


void PathPlanner::add_bullshit() {
	for (float z(-.2); z < .2; z += .03)
		d_posefinder.simulator().add_ball(-.3, 0.13, z);
}


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


void PathPlanner::insert_poses(std::string mapname, poses_map_t &poses) {
	check_map(mapname);

	vector<vector<double> > configurations = poses_to_configurations(poses);
	//cout << configurations.size() << endl;
	int dimensionality(configurations[0].size());
	cout << "INSERTING POSES" << endl;

	d_roadmaps[mapname]->setDimensionality(d_dimensionality);
	d_roadmaps[mapname]->setScaleVector(parse_scale_vector());

	for (size_t i(0); i < configurations.size(); ++i) {
		vector<double> &q_configuration(configurations[i]);
		vector<double> &x_configuration(poses["WORKSPACE"][i]);
		d_roadmaps[mapname]->insert(x_configuration, q_configuration, mapname);
		d_main_roadmap.insert(x_configuration, q_configuration, mapname);
	}
}


Roadmap::path_t PathPlanner::find_path(vector<double> &source, vector<double> &target, TreeMode mode) {
	double granularity(2.0);
	CollisionEdgeTester collision_edge_tester(d_main_roadmap, d_posefinder.simulator(), granularity);
	Roadmap::vertex_t from_v = d_main_roadmap.nearestVertex(source, 0, mode);
	Roadmap::vertex_t to_v = d_main_roadmap.nearestVertex(target, 0, mode);

	Roadmap::path_t the_path = d_main_roadmap.shortestPath(from_v, to_v, collision_edge_tester, mode);

	cout << "dimensions: " << source.size() << " x " << target.size() << endl;
	cout << "vertices: " << the_path.path.size() << endl;
	cout << "N evaluated: " << collision_edge_tester.n_evaluations() << endl;
	cout << "N collision checks per edge: " << (double) collision_edge_tester.n_vskincalls() / (double) collision_edge_tester.n_evaluations() << endl;
	cout << "avg time: " << collision_edge_tester.n_seconds() / collision_edge_tester.n_evaluations() << endl;
	cout << "total time: " << collision_edge_tester.n_seconds() << endl;
	return the_path;
}


/*
vector<vector<double> > PathPlanner::move_to_path(vector<double> source, vector<double> target) {
  vector<vector<double> > the_path;
  //the_path.push_back(source);
  the_path.push_back(target);
  return the_path;
}
*/

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


vector<double> PathPlanner::nearestQ(Roadmap &map, vector<double> &v, TreeMode mode) {
	return map.getMap()[map.nearestVertex(v, 0, mode)].q;
}


vector<double> PathPlanner::nearestQ(string mapname, vector<double> &v, TreeMode mode) {
	check_map(mapname);
	return nearestQ(*d_roadmaps[mapname], v, mode);
}




/*
std::pair<Path, double> Roadmap::shortestPath_backup( vertex_t from, vertex_t to )
{
	cout << endl << "Running Dijkstra's... " << from << " " << to << endl; 
	vector<vertex_t> parents(num_vertices(map));
	vector<double> distances(num_vertices(map));


//	 pair<edge_i, edge_i> map_edges(edges(map));
//	 edge_i edge_it(map_edges.first);
//	 cout << "adding lengths2" << endl;
//	 for (; edge_it != map_edges.second; ++edge_it) {
//		 double value = (map[source(*edge_it, map)].fitness + map[target(*edge_it, map)].fitness);
//		 int collisions1 = map[source(*edge_it, map)].collisions;
//		 int collisions2 = map[target(*edge_it, map)].collisions;
//		 double length = get(&Edge::length, map, *edge_it);
//
//		 length = length * length;
//
////		  cout << value << " " << collisions << endl;
//		 put(&Edge::length_evaluated, map, *edge_it, length + 1000. * (collisions1 + collisions2));
//	 }
//	 cout << "done" << endl;

	try {
		dijkstra_shortest_paths(	map, from, 
									weight_map( get(&Edge::length_evaluated, map) )
									.predecessor_map( make_iterator_property_map(parents.begin(),get(boost::vertex_index, map)) )
									.distance_map( make_iterator_property_map(distances.begin(),get(boost::vertex_index, map)) )
									.visitor( target_visit(to,boost::on_examine_vertex()) )
								); 
	} catch( ... ) {}
	
	list<vertex_t> path;
	path.push_front(to);
	
	while ( path.front() != from )
	{
		if ( parents[path.front()] == path.front() )
		{
			printf("target pose unreachable. no path through the graph\n");
			break;
		}
		path.push_front(parents[path.front()]);
	}
	
	printf("path: ");
	for (list<vertex_t>::iterator i = path.begin(); i != path.end(); ++i )
	{
		printf("%lu ",*i);
	}
	printf("/\n");
	
	
	return path;
}
*/

void PathPlanner::CollisionEdgeTester::operator()(edge_t &edge) {
	if (get(&Roadmap::Edge::evaluated, d_roadmap.getMap(), edge))
		return;

	std::clock_t start_clock(std::clock());
	std::vector<double> &q_start = d_roadmap.getMap()[source(edge, d_roadmap.getMap())].q;
	std::vector<double> &q_end = d_roadmap.getMap()[target(edge, d_roadmap.getMap())].q;

	int n_collisions = 0;

	// adaptive resolution
	// start is not checked, because it is end of previous expansion
	size_t resolution = std::floor(std::max(1.0, calculate_distance(q_start, q_end) / d_granularity));
	for (size_t i(0); i < resolution; ++i) {
		if (n_collisions > 0)
			break;
		double portion(static_cast<double>(i+1) / resolution);
		std::vector<double> q(q_start.size());
		for (size_t n(0); n < q.size(); ++n)
			q[n] = q_start[n] * portion + q_end[n] * (1. - portion);
		std::vector<double> q_real = d_simulator.real_to_normal_motors(q);
		d_simulator.set_motors(q_real);
		n_collisions = std::max(d_simulator.computePose(), n_collisions);
		//if (n_collisions > 0) { std::cout << "collisions: " << n_collisions << std::endl; };
		++d_n_vskincalls;
	}

	double length = get(&Roadmap::Edge::length, d_roadmap.getMap(), edge);
	if (!n_collisions)
		put(&Roadmap::Edge::length_evaluated, d_roadmap.getMap(), edge, length);
	else
		put(&Roadmap::Edge::length_evaluated, d_roadmap.getMap(), edge, FLT_MAX);
	put(&Roadmap::Edge::evaluated, d_roadmap.getMap(), edge, true);

	++d_n_evaluations;
	std::clock_t end_clock(std::clock());
	d_clock += end_clock - start_clock;
}

/*
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
		put(&Roadmap::Edge::length_evaluated, roadmap.map, *edge_it, length + 100000. * n_collisions);

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
*/
