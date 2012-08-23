#include "roadmap.h"
#include "poses_reader.h"
#include "exception.h"

//#include "graphwidget.h"
//#include "widgetEdge.h"
//#include "widgetNode.h"

#include <boost/graph/random.hpp>
#include <boost/random.hpp>
//#include <boost/graph/graphviz.hpp>

using namespace std;
using namespace boost;

Roadmap::Roadmap() : dim(0), currentVertex(0)
{
	srand ( time(NULL) );
}

Roadmap::~Roadmap()
{
}

void Roadmap::setDimensionality( int d )
{
	// dimensionality can only be set when the map is empty
	pair<vertex_i, vertex_i> vp = vertices(map);
	if ( vp.first == vp.second )
	{
		dim = d;
		return;
	}
	printf("the map is not empty!!!\n");
}

void Roadmap::setCurrentVertex( vertex_t v )
{
//	if ( map[v].qtGraphNode )
//		emit newNodeColor( map[v].qtGraphNode, Qt::red, Qt::darkRed );
//		//map[v].qtGraphNode->setColor( Qt::yellow, Qt::darkYellow );
//
//	if ( v != currentVertex )
//	{
//		if ( map[currentVertex].qtGraphNode )
//			emit newNodeColor( map[currentVertex].qtGraphNode, Qt::lightGray, Qt::gray );
//			//map[currentVertex].qtGraphNode->setColor( Qt::lightGray, Qt::gray );
//	}
//
//	currentVertex = v;
}

//void Roadmap::setEdgeColor( edge_t e, QColor color )
//{
//	if ( map[e].qtGraphEdge )
//		emit newEdgeColor( map[e].qtGraphEdge, color );
//		//map[e].qtGraphEdge->setColor( color );
//}

pair< Roadmap::edge_t, vector<double> > Roadmap::randomMove()
{
	pair< edge_t, vector<double> > result;
	if ( num_vertices( map ) > 0 )
	{
		vector<edge_t> moves;
		Map::out_edge_iterator e, e_end;
		
		vertex_t v = currentVertex;
		
		//int count = 0;
		for (boost::tuples::tie(e, e_end) = out_edges( v, map ); e != e_end; ++e)
		{
			moves.push_back( *e );
			//count++;
		}
		
		if ( moves.size() > 0 )
		{
			int idx = rand() % moves.size();
			result.first = moves.at(idx);
			result.second = map[target(moves.at(idx),map)].q;
		}
	}
	return result;
}

/*
list< pair< Roadmap::edge_t, Roadmap::vertex_t > > Roadmap::aToB( Roadmap::vertex_t from, Roadmap::vertex_t to )
{
	list< pair< edge_t, vertex_t > > result;
	if ( num_vertices( map ) > 0 )
	{
		list<Map::vertex_descriptor> vertex_list = shortestPath( from, to );
		for ( list<Map::vertex_descriptor>::iterator i = vertex_list.begin(); i != vertex_list.end(); )
		{
			vertex_t a = *i;
			vertex_t b = *(++i);
			pair<edge_t,bool> ab = edge(a,b,map);
			if (ab.second)
				result.push_back( pair< edge_t, vertex_t >( ab.first, b ) );
		}
	}
	return result;
}
*/

 /*
list< pair< Roadmap::edge_t, Roadmap::vertex_t > > Roadmap::randomMoves()
{
	list< pair< edge_t, vertex_t > > result;
	if ( num_vertices( map ) > 0 )
	{
		vertex_t rand_vertex = (vertex_t)(rand() % (int)num_vertices(map));
		result = aToB(currentVertex, rand_vertex);
	}
	return result;
}
 */

/*void Roadmap::buildRandomMap( unsigned int numVertices, unsigned int numNeighbors )
{
	for ( unsigned int j=0; j<numVertices; j++ )
	{
		//insert( randomSample(), numNeighbors );
	}
}*/

vector<double>  Roadmap::scale_q( vector<double> q) {
	assert(q.size() == scale_vector.size());
	vector<double> q_scaled(q);
	vector<double>::iterator it1(q_scaled.begin()), it1_end(q_scaled.end()), it2(scale_vector.begin());

	for (; it1 != it1_end; ++it1, ++it2)
		*it1 *= *it2;
	return q_scaled;
}

vector<double>  Roadmap::unscale_q( vector<double> q_scaled) {
	vector<double> q(q_scaled);
	vector<double>::iterator it1(q.begin()), it1_end(q.end()), it2(scale_vector.begin());

	for (; it1 != it1_end; ++it1, ++it2) {
		if (*it2 == 0)
			continue;
		*it1 /= *it2;
	}
	return q;
}


Roadmap::vertex_t Roadmap::insert( vector<double> _x, vector<double> _q, string name)
{
  //printf("called insert\n");
  //if (!dim)
  //setDimensionality(_q.size());

  if ( _q.size() != dim ) { printf("wrong size state vector, is %lu, not %lu\n",_q.size(), dim); throw StringException("wrong size state vector"); }
	
	// put the configuration in the boost graph
	vertex_t vertex = boost::add_vertex( map );
	map[vertex].q = _q;
	map[vertex].qs = scale_q(_q);
	map[vertex].x = _x;
	map[vertex].map_name = name;

	map[vertex].fitness = 0;
	
	// put it in the CGAL tree
	Pose p( _q.size(), _q.begin(), _q.end(), vertex );
	tree.insert( p );
	
	Pose p_workspace( _x.size(), _x.begin(), _x.end(), vertex );
	workspace_tree.insert(p_workspace);

	vector<double> q_scaled = scale_q(_q);
	Pose p_scaled( q_scaled.size(), q_scaled.begin(), q_scaled.end(), vertex );
	scaled_tree.insert(p_scaled);
	// connect it to its n nearest neighbors
	//graphConnect( p, n );
	
//	printf("inserted");
	return vertex;
}

void Roadmap::load( vector< vector<double> >& graphNodes, vector< pair<int,int> >& graphEdges )
{
//	//TODO Clean out the boost graph first
//	//int count = 0;
//
//	vertex_t vertex;
//	vector< vertex_t > vertices;
//	vector< vector<double> >::iterator v;
//
//	for ( v=graphNodes.begin(); v!=graphNodes.end(); ++v )
//	{
//		cout << "inserting: " << *(v->begin()) << " " << *(v->begin()+1) << " - ";
//
//		vector<double> q( v->begin()+2, v->end() );
//		for ( vector<double>::iterator j = q.begin(); j !=q.end(); ++j )
//			cout << *j << " ";
//		cout << endl;
//
//		vertex = insert( *(v->begin()),
//						 *(v->begin()+1),
//						 q
//						);
//		vertices.push_back( vertex );
//	}
//
//	sleep(1);
//
//	vector< pair<int,int> >::iterator e;
//	pair<edge_t, bool> edge;
//	for ( e=graphEdges.begin(); e!=graphEdges.end(); ++e )
//	{
//		edge = boost::add_edge( vertices.at(e->first), vertices.at(e->second), map );
//		//cout << edge.first << endl;
//		if ( !edge.second )
//			printf("boost::add_edge() failed.\n");
//		else
//		{
//			//printf("appended edge to boost graph\n");
//			emit appendedEdge( edge.first,
//							   map[vertices.at(e->first)].qtGraphNode ,
//							   map[vertices.at(e->second)].qtGraphNode );
//		}
//	}
//	printf("loaded file: %d nodes, %d edges\n",graphNodes.size(),graphEdges.size());
}

void Roadmap::data( vector< vector<double> >* graphNodes, vector< pair<int,int> >* graphEdges )
{
//	int count = 0;
//
//	pair<vertex_i, vertex_i> vp;
//	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
//	{
//		map[*(vp.first)].idx = count;
//		vector<qreal> thisLine;
//		if ( map[*(vp.first)].qtGraphNode ) {
//			thisLine.push_back( map[*(vp.first)].qtGraphNode->getNormX() );
//			thisLine.push_back( map[*(vp.first)].qtGraphNode->getNormY() );
//		} else {
//			thisLine.push_back( 0.0 );
//			thisLine.push_back( 0.0 );
//		}
//		thisLine.insert(thisLine.end(), map[*(vp.first)].q.begin(), map[*(vp.first)].q.end());
//		graphNodes->push_back( thisLine );
//		count ++;
//	}
//
//	pair<edge_i, edge_i> ep;
//	pair<int,int> thisEdge;
//	for (ep = edges(map); ep.first != ep.second; ++ep.first)
//	{
//		thisEdge.first = map[source(*(ep.first), map)].idx;
//		thisEdge.second = map[target(*(ep.first), map)].idx;
//		graphEdges->push_back( thisEdge );
//	}
}


void Roadmap::write_graph(std::string filename) {  
  /*
  dynamic_properties dp;
  dp.property("id", get(vertex_index, map));
  write_graphviz(out_file, map, dp);
  */
  std::ofstream out_file(filename.c_str());

  pair<edge_i, edge_i> map_edges(edges(map));
  edge_i edge_it(map_edges.first);
  for (; edge_it != map_edges.second; ++edge_it) {
	write_vector(map[source(*edge_it, map)].x, out_file);
	write_vector(map[target(*edge_it, map)].x, out_file);
	out_file << endl;
  }
}

void Roadmap::readMapPoses(string filename, string mapname) {
	poses_map_t poses_map = read_poses(filename);
	poses_vector_t poses(poses_map["CFGSPACE"]);
	poses_vector_t work_space(poses_map["WORKSPACE"]);
	for (size_t i(0); i < poses.size(); ++i) {
	  insert(work_space[i], poses[i], mapname);
	}
}

void Roadmap::removeEdge( Roadmap::edge_t edge )
{
	//if ( map[edge].qtGraphEdge )
		//map[edge].qtGraphEdge->flagRemoval();
	
	//emit removeQtGraphEdge(map[edge].qtGraphEdge);
	
	
	remove_edge( edge, map );
}

void Roadmap::removeAllEdges()
{
	edge_i ei, ei_end, next;
	boost::tuples::tie(ei, ei_end) = edges(map);
	for (next = ei; ei != ei_end; ei = next) {
		++next;
		removeEdge(*ei);
	}
}


Roadmap::Tree &Roadmap::get_tree(TreeMode tree_mode){
	switch(tree_mode) {
	case WORKSPACE:
		return workspace_tree;
	case CONFIGURATIONSPACE:
		return tree;
	case SCALEDCONFIGURATIONSPACE:
		return scaled_tree;
	}
	throw StringException("Tree not found");
}



void Roadmap::graphConnect( Pose p, unsigned int n, TreeMode tree_mode)
{	
	Tree &the_tree = get_tree(tree_mode);
	K_neighbor_search search(the_tree, p , n+1);
	for(K_neighbor_search::iterator it = search.begin(); it != search.end(); ++it)
	{
	  //cout << sqrt(it->second) << " ";
	  //for (size_t i(0); i < map[it->first.vertex].x.size(); ++i)
	  //  cout << map[it->first.vertex].x[i] << " ";
	  //cout << endl;
		size_t counter(0);
		if ( it->second != 0)
		{
			if (!boost::edge( p.vertex, it->first.vertex, map ).second) {
				pair<edge_t, bool> edge = boost::add_edge( p.vertex, it->first.vertex, map );
				map[edge.first].length = sqrt(it->second);
				//cout << sqrt(it->second) << endl;
				++counter;
			}
			if (!boost::edge(it->first.vertex, p.vertex, map ).second) {
				pair<edge_t, bool> edge = boost::add_edge( it->first.vertex, p.vertex, map );
				map[edge.first].length = sqrt(it->second);
				++counter;
			}

//			cout << sqrt(it->second) << endl;
			//cout << "connected " << p.vertex << " - " << it->first.vertex << " " << "(" << map[edge.first].length << ")" << endl;
//			emit appendedEdge( edge.first,
//							  map[p.vertex].qtGraphNode ,
//							  map[it->first.vertsqrt(it->second)ex].qtGraphNode );
		}
	}
}


void Roadmap::mapDistances(int p, std::vector<double>& d) {
	int n = d.size();
	double diff;
	for (int m = 0; m<n; m++) {		
		for (int i = 0; i<map[m].q.size(); i++) {
			diff = map[m].q[i] - map[p].q[i];
			d[m] = d[m] + (diff * diff);
		}
	}
}


void Roadmap::graphConnect2( int p, unsigned int n)
{	
	double nearby_penalty = 100.0;

	pair<vertex_i, vertex_i> vp;
	vp = vertices(map);
	int nmap = *(vp.second) - *(vp.first);
	vector<double> mdist(nmap, 0.0); // distances
	vector<int> neighbors; // neighbor indices
	mapDistances(p, mdist);
	vector<double> odist = mdist; // make a copy of original distances
	mdist[p] = FLT_MAX;
	int neighbor_i;

	// cycle through map to find neighbors
	for (int k=0; k<n; k++) {
		neighbor_i = min_element(mdist.begin(), mdist.end()) - mdist.begin(); // index of k-nearest neighbor

		// compute distances between k-nearest neighbour and all other points on the map
		vector<double> pdist(nmap, 0.0);
		mapDistances(neighbor_i, pdist);

		// add inverse pdist to mdist
		for (int m = 0; m<nmap; m++) {
			//mdist[m] = mdist[m] + nearby_penalty/pdist[m];
			//double penalty = nearby_penalty*exp(-sqrt(pdist[m]));
			//double penalty = 100*exp(-pdist[m]/10);
			double penalty = 200*exp(-pdist[m]/20);
			mdist[m] = mdist[m] + penalty;
		}
		
		// set neighbor distance to FLT_MAX, such that another neighbor is selected next
		//cout << mdist[neighbor_i] << " ";
		mdist[neighbor_i] = FLT_MAX;

		// add neighbor
		neighbors.push_back(neighbor_i);
	}

	for(int k=0; k<n; k++) {
		if (!boost::edge( p, neighbors[k], map ).second) {
			pair<edge_t, bool> edge = boost::add_edge( p, neighbors[k], map );
			map[edge.first].length = sqrt(odist[k]);
		}
	}
}


void Roadmap::graphConnect2( unsigned int n, TreeMode tree_mode)
{
    pair<vertex_i, vertex_i> vp;
	vp = vertices(map);
	int nmap = *(vp.second) - *(vp.first);
	
	for (int m = 0; m<nmap; m++) {		
	    graphConnect2( m, n);
		//cout << m << " / " << nmap << " processed" << endl;
	}
}

void Roadmap::graphConnect( unsigned int n, TreeMode tree_mode)
{
  //tree.build();
  //workspace_tree.build();
  //scaled_tree.build();

	pair<vertex_i, vertex_i> vp;
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
	  switch(tree_mode) {
	  case CONFIGURATIONSPACE:
	    graphConnect( Pose(map[*(vp.first)].q.size(),map[*(vp.first)].q.begin(),map[*(vp.first)].q.end(),*(vp.first)), n, tree_mode);
	    break;
	  case WORKSPACE:
	    graphConnect( Pose(map[*(vp.first)].x.size(),map[*(vp.first)].x.begin(),map[*(vp.first)].x.end(),*(vp.first)), n, tree_mode);
	    break;
	  case SCALEDCONFIGURATIONSPACE:
		graphConnect( Pose(map[*(vp.first)].qs.size(),map[*(vp.first)].qs.begin(),map[*(vp.first)].qs.end(),*(vp.first)), n, tree_mode);
	    break;
	  }
	}
}


/*void Roadmap::random_connect(size_t n) {
  static mt19937 gen(23);
  cout << "==random" << endl;
  for (size_t i(0); i < n; ++i) {
	vertex_t v1(random_vertex(map, gen)), v2(random_vertex(map, gen));
	if (!boost::edge( v1, v2, map ).second) {
	  pair<edge_t, bool> edge = boost::add_edge( v1, v2, map );
	  double distance = calculate_distance(scale_q(map[v1].q), scale_q(map[v2].q));
	  map[edge.first].length = distance;
	  cout << calculate_distance(scale_q(map[v1].q), scale_q(map[v2].q)) << endl;
	}
  }
}*/

Roadmap::vertex_t Roadmap::nearestVertex( vector<double> _q, char* type )
{
  if ( _q.size() != dim ) { printf("2: wrong size state vector, is %lu, not %lu\n",_q.size(), dim); throw StringException("wrong size state vector"); }
	if ( tree.size() == 0 ) { throw StringException("nothing in tree"); }
	
	//cout << "(" << _q.size() << "," << (unsigned int)iCub.getNumJoints() << ")" << endl;

	size_t const check_n(40);
	K_neighbor_search search(tree, Pose( _q.size(), _q.begin(), _q.end() ) , check_n);
	K_neighbor_search::iterator it = search.begin();
	for(; it != search.end(); ++it) {
		//TODO: realtime test for collisions
	    //std::vector<double> q_real = d_posefinder.simulator().real_to_normal_motors(q);
		//d_posefinder.simulator().set_motors(q_real);
		//if (d_simulator.computePose() == 0)
			break;
	}
	if (it == search.end())
	  throw StringException("couldnt find nearest vertex");
	map[it->first.vertex].type = type;
	return it->first.vertex;
}

vector<double> Roadmap::nearestVector(vector<double> q) {
  return map[nearestVertex(q)].q;
}

/*
std::pair<PathList, double> Roadmap::shortestPath_backup( vertex_t from, vertex_t to )
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
//		 put(&Edge::length2, map, *edge_it, length + 1000. * (collisions1 + collisions2));
//	 }
//	 cout << "done" << endl;

	try {
		dijkstra_shortest_paths(	map, from, 
									weight_map( get(&Edge::length2, map) )
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

Roadmap::path_t Roadmap::shortestPath( vertex_t from, vertex_t to, EdgeTester<edge_t> &edge_tester)
{
	cout << endl << "Running A*... " << from << " " << to << endl; 
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
//		 put(&Edge::length2, map, *edge_it, length + 1000. * (collisions1 + collisions2));
//	 }
//	 cout << "done" << endl;
	
	//set visited to zero
	{
	  pair<edge_i, edge_i> map_edges(edges(map));
	  edge_i edge_it(map_edges.first);
	  for (; edge_it != map_edges.second; ++edge_it)
		put(&Edge::evaluated, map, *edge_it, false);
	}
		
	vector<vertex_t> p(num_vertices(map));
	vector<double> d(num_vertices(map));

	list<vertex_t> path;
	double distance(FLT_MAX);

	try {
	  // call astar named parameter interface 
	  cout << "Starting Search: " << endl;
	  cout << "num vertices: " << num_vertices(map) << endl;
	  cout << "num edges: " << num_edges(map) << endl;


	  astar_search
		(map, from,
		 distance_heuristic<Map, double>
		 (map, to),
		 weight_map( get(&Edge::length2, map) )
		 .predecessor_map(&p[0])
		 .distance_map(&d[0])
		 .visitor(astar_goal_visitor<vertex_t, edge_t>(to, edge_tester)));

	  
	} catch(found_goal fg) { // found a path to the goal 
	  cout << "Found Goal" << endl;

	  for(vertex_t v = to;; v = p[v]) {
		path.push_front(v);
		if(p[v] == v)
		  break;
	  }
	  distance = d[to];
	  printf("goal: %f", map[to].x[0]);
	  printf("path: ");
	  
	  for (list<vertex_t>::iterator i = path.begin(); i != path.end(); ++i )
		{
		  printf("%lu ",*i);
		}
	  printf("/\n");

	}

	return path_t(path, distance, map[to].x);
}  

Roadmap::path_t  Roadmap::shortestPath( vector<double> from, vector<double> to, EdgeTester<edge_t> &edge_tester) {
  cout << "shortest path: " << from.size() << " " << to.size() << endl;
	Roadmap::vertex_t from_desc = nearestVertex(from);
	Roadmap::vertex_t to_desc = nearestVertex(to);

	return shortestPath(from_desc, to_desc, edge_tester);
}

vector<vector<double> > Roadmap::vertex_list_to_q(list<Roadmap::vertex_t> &path) {
	vector<vector<double> > vector_path;
	list<Roadmap::Map::vertex_descriptor>::iterator it(path.begin());
	cout << "path len: " << path.size() << endl;
	for (; it != path.end(); ++it) {
	  cout << "v: " << *it << endl;
	  vector_path.push_back(map[*it].q);
	}

	return vector_path;
}


vector<vector<double> > Roadmap::vertex_list_to_x(list<Roadmap::vertex_t> &path) {
	vector<vector<double> > vector_path;
	list<Roadmap::Map::vertex_descriptor>::iterator it(path.begin());
	for (; it != path.end(); ++it) {
	  vector_path.push_back(map[*it].x);
	}

	return vector_path;
}

Roadmap::vertex_t Roadmap::nearestWorkspaceVertex( vector<double> _x)
{
//	if ( _x.size() != dim_x ) { throw StringException("wrong size state vector"); }
  if (! workspace_tree.size() > 0 ) { throw StringException("nothing in tree"); } // doesn't throw but access violation on workspace_tree instead
	if (_x.size() != 3) { throw StringException("Nothing in X"); }
	cout << "(" << _x.size() << "," << workspace_tree.size() << " " << tree.size() << endl;


	size_t const check_n(40);
	K_neighbor_search search(workspace_tree, Pose( _x.size(), _x.begin(), _x.end() ), check_n);
	K_neighbor_search::iterator it = search.begin();
	for(; it != search.end(); ++it) {
		//TODO: realtime test for collisions
	    //std::vector<double> q_real = d_posefinder.simulator().real_to_normal_motors(q);
		//d_posefinder.simulator().set_motors(q_real);
		//if (d_simulator.computePose() == 0)
		break;
	}
	if (it == search.end())
	  throw StringException("couldnt find nearest workspace vertex");

	return it->first.vertex;
}

vector<double> Roadmap::nearestWorkspaceVector(vector<double> x) {
  return map[nearestWorkspaceVertex(x)].q;
}
