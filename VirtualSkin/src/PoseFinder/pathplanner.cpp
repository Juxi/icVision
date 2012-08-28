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
    vector<double> parsed_scale_vector(parse_scale_vector());
	d_main_roadmap.setScaleVector(parsed_scale_vector);
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
	if (d_roadmaps.count(mapname) > 0)
		throw StringException("map already exist");

	poses_map_t poses = read_poses(filename);

	if (poses.size() > 0) {
		d_roadmaps[mapname] = new Roadmap();
		insert_poses(mapname, poses);
	} else
		throw StringException("Loading failed");
}


void PathPlanner::connect_map(size_t n, TreeMode mode, string mapname) {
	Roadmap &map = get_map(mapname);
	if (map.size() == 0)
		throw StringException("Cannot connect empty map");
	if (mapname == "") { // connect main roadmap and all other roadmaps
		roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
		for (; it != it_end; ++it)
			connect_map(n, mode, it->first);
	}
	map.removeAllEdges();
	map.graphConnect(n, mode);
}


void PathPlanner::connect_map2(size_t n, TreeMode mode, string mapname) {
	Roadmap &map = get_map(mapname);
	if (map.size() == 0)
		throw StringException("Cannot connect empty map");
	if (mapname == "") { // connect main roadmap and all other roadmaps
		roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());
		for (; it != it_end; ++it)
			connect_map2(n, mode, it->first);
	}
	map.removeAllEdges();
	map.graphConnect2(n, mode);
}

/*void PathPlanner::clear_connections() {
	roadmap_iterator it(d_roadmaps.begin()), it_end(d_roadmaps.end());

	for (; it != it_end; ++it)
		it->second->removeAllEdges();
	d_main_roadmap.removeAllEdges();
}*/


void PathPlanner::write_graph(std::string filename, std::string mapname) {
	Roadmap &map = get_map(mapname);
	map.write_graph(filename);
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


Roadmap::Vertex &PathPlanner::getVertex(vertex_t index, std::string mapname) {
	Roadmap &map = get_map(mapname);
	if ( index <0 || index >= map.size())
		throw StringException("Vertex out of range");
	return map.getMap()[index];
}


Roadmap &PathPlanner::get_map(std::string mapname) {
	if (mapname == "")
		return d_main_roadmap;
	else if (d_roadmaps.count(mapname) <= 0)
		throw StringException("Map does not exist");
	return *d_roadmaps[mapname];
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
	Roadmap &map = get_map(mapname);
	if (mapname == "")
		throw StringException("Cannot directly insert poses in main roadmap");

	vector<vector<double> > configurations = poses_to_configurations(poses);
	//cout << configurations.size() << endl;
	int dimensionality(configurations[0].size());
	cout << "INSERTING POSES" << endl;

	map.setDimensionality(d_dimensionality);
    vector<double> parsed_scale_vector(parse_scale_vector());
	map.setScaleVector(parsed_scale_vector);

	for (size_t i(0); i < configurations.size(); ++i) {
		vector<double> &q_configuration(configurations[i]);
		vector<double> &x_configuration(poses["WORKSPACE"][i]);
		map.insert(x_configuration, q_configuration, mapname);
		d_main_roadmap.insert(x_configuration, q_configuration, mapname);
	}
}


Roadmap::path_t PathPlanner::find_path(vertex_t &source, vertex_t &target, TreeMode mode, string mapname) {
	double granularity(2.0);
	Roadmap &map = get_map(mapname);
	CollisionEdgeTester collision_edge_tester(map, d_posefinder.simulator(), granularity, mode);

	Roadmap::path_t the_path = map.shortestPath(source, target, collision_edge_tester, mode);

	cout << "vertices in path: " << the_path.path.size() << endl;
	cout << "evaluated edges: " << collision_edge_tester.n_evaluations() << endl;
	cout << "collision checks per edge: " << (double) collision_edge_tester.n_vskincalls() / (double) collision_edge_tester.n_evaluations() << endl;
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


string PathPlanner::range_string(string mapname) {
  Roadmap &map = get_map(mapname);
  ostringstream oss("range:");
  try {
	oss << "range of map [" << mapname << "]" << endl;
	pair<vector<float>, vector<float> > bbox = map.get_workspace_bounding_box();
	
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


Roadmap::vertex_t PathPlanner::nearestMainMapVertex(vector<double> &v, TreeMode mode, string mapname) {
	double granularity = 2.0;
	Roadmap &map = get_map(mapname);
	CollisionVertexTester collision_vertex_tester(map, d_posefinder.simulator(), granularity);
	Roadmap::vertex_t map_vertex = map.nearestVertex(v, collision_vertex_tester, mode);
	if (mapname == "")
		return map_vertex;
	else // return vertex in main roadmap
		return d_main_roadmap.nearestVertex(getVertex(map_vertex, mapname).x, collision_vertex_tester, WORKSPACE);
}


Roadmap::vertex_t PathPlanner::nearestVertex(vector<double> &v, TreeMode mode, string mapname) {
	double granularity = 2.0;
	Roadmap &map = get_map(mapname);
	CollisionVertexTester collision_vertex_tester(map, d_posefinder.simulator(), granularity);
	return map.nearestVertex(v, collision_vertex_tester, mode);
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


PathPlanner::CollisionEdgeTester::CollisionEdgeTester(Roadmap &roadmap, Simulator &simulator, double granularity, TreeMode mode) :
d_roadmap(roadmap), d_simulator(simulator), d_granularity(granularity), d_n_evaluations(0), d_clock(0), d_n_vskincalls(0)
{
	if (mode == CONNECTIONMODE) {
		conmode = d_roadmap.getConMode();
	}
}


void PathPlanner::CollisionEdgeTester::operator()(edge_t &edge) {
	Roadmap::Map &map = d_roadmap.getMap();
	if (get(&Roadmap::Edge::evaluated, map, edge))
		return;

	std::clock_t start_clock(std::clock());
	std::vector<double> &q_start = map[source(edge, map)].q;
	std::vector<double> &q_end = map[target(edge, map)].q;

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

	double length = 0.0;
	switch(conmode) {
	case WORKSPACE:
		length = get(&Roadmap::Edge::length_x, map, edge);
		break;
	case CONFIGURATIONSPACE:
		length = get(&Roadmap::Edge::length_q, map, edge);
		break;
	case SCALEDCONFIGURATIONSPACE:
		length = get(&Roadmap::Edge::length_qs, map, edge);
		break;
	default:
		throw StringException("Invalide connection mode");
	}
	
	if (!n_collisions)
		put(&Roadmap::Edge::length_evaluated, map, edge, length);
	else
		put(&Roadmap::Edge::length_evaluated, map, edge, FLT_MAX);
	put(&Roadmap::Edge::evaluated, map, edge, true);

	++d_n_evaluations;
	std::clock_t end_clock(std::clock());
	d_clock += end_clock - start_clock;
}


bool PathPlanner::CollisionVertexTester::check(std::vector<double> &q1, std::vector<double> &q2) {
	int n_collisions = 0;

	// adaptive resolution
	size_t resolution = std::floor(std::max(1.0, calculate_distance(q1, q2) / d_granularity));
	for (size_t i(0); i < resolution+1; ++i) {
		if (n_collisions > 0)
			break;
		double portion(static_cast<double>(i) / resolution);
		std::vector<double> q(q1.size());
		for (size_t n(0); n < q.size(); ++n)
			q[n] = q1[n] * portion + q2[n] * (1. - portion);
		std::vector<double> q_real = d_simulator.real_to_normal_motors(q);
		d_simulator.set_motors(q_real);
		n_collisions = std::max(d_simulator.computePose(), n_collisions);
		//cout << "testing collision percentage: " << portion*100 << "%" << endl;
	}
	
	return n_collisions==0;
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
