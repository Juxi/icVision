#include "roadmap.h"
//#include "graphwidget.h"
#include "widgetEdge.h"
#include "widgetNode.h"
//#include "poses_reader.h"
//#include <cmath>
//#include <vector>
//#include <algorithm>

/*void Roadmap::setDimensionality( int d )
 {
 // dimensionality can only be set when the map is empty
 std::pair<vertex_i, vertex_i> vp = vertices(map);
 if ( vp.first == vp.second )
 {
 dim = d;
 return;
 }
 printf("the map is not empty!!!\n");
 }*/

bool Roadmap::insert( std::vector<double> q, vertex_t& v, bool display )
{
	//printf("called insert\n");
	//if ( _q.size() != dim ) { printf("wrong size state vector %d\n",_q.size()); throw("wrong size state vector"); }
	
	// avoid duplicate points
	//K_neighbor_search search( cgalTree, vertex, 1);
	//if ( search.begin()->second < 1.0 )
	//	return false;
	
	v = boost::add_vertex( Vertex(q.size(),q.begin(),q.end() ), boostGraph );
	boostGraph[v].vertex_t = v;
	
	//std::cout << "Vertex " << v << " " << boostGraph[v].vertex_t << ": " << boostGraph[v] << std::endl;
	
	cgalTree->insert(boostGraph[v]);
	
	if ( display )
		emit appendedNode( v );
	
	//printf("inserted");
	return true;
}

bool Roadmap::connect( vertex_t v, vertex_t w )
{		
	std::pair<edge_t, bool> edge = boost::add_edge( v, w, boostGraph );
	boostGraph[edge.first].squaredLength = (boostGraph[v]-boostGraph[w]).squared_length();
	emit appendedEdge(	edge.first, 
						boostGraph[v].qtGraphNode ,
						boostGraph[w].qtGraphNode );
	return true;
}

bool Roadmap::randomNeighbor( vertex_t vertex, vertex_t& neighbor )
{
	std::vector<vertex_t> allNeighbors;
	out_edge_i e, e_end;
	for ( tie(e, e_end) = out_edges( vertex, boostGraph ); e != e_end; ++e)
		allNeighbors.push_back( target(*e,boostGraph) );
		
	if ( allNeighbors.size() > 0 )
	{
		int idx = rand() % allNeighbors.size();
		neighbor = allNeighbors.at(idx);
		return true;
	}
	return false;
}

bool Roadmap::shortestPath( vertex_t from, vertex_t to, std::list<vertex_t>& path )
{
	std::cout << std::endl << "Running Dijkstra's... " << from << " " << to << std::endl; 
	std::vector<vertex_t> parents(num_vertices(boostGraph));
	std::vector<double> distances(num_vertices(boostGraph));
	try {
		dijkstra_shortest_paths( boostGraph, from, 
								 weight_map( get(&Edge::squaredLength, boostGraph) )
								 .predecessor_map( make_iterator_property_map(parents.begin(),get(boost::vertex_index, boostGraph)) )
								 .distance_map( make_iterator_property_map(distances.begin(),get(boost::vertex_index, boostGraph)) )
								 .visitor( target_visit(to,boost::on_examine_vertex()) )
							   ); 
	} catch( ... ) { printf("dijkstra's threw an exception\n\n"); }
	
	path.push_front(to);
	//std::vector<vertex_t>::iterator i = path.front();
	//while ( path.front() != from )
	//{
		//if ( parents[path.front()] == path.front() )
		//{
		//	printf("target pose unreachable. no path through the graph\n");
		//	return false;
		//}
		//path.push_front(parents[path.front()]);
	//}
	
	//printf("path: ");
	//for (std::list<vertex_t>::iterator i = path.begin(); i != path.end(); ++i )
	//	printf("%d ",*i);
	//printf("\n");
	
	return true;
}

bool Roadmap::nearestVertices( vertex_t vertex, std::vector<vertex_t>& neighbors, std::vector<double>& distances, int n )
{
	printf("tree size: %d\n", cgalTree->size());
	printf("graph size: %d\n", num_vertices(boostGraph));
	
	//std::cout << "QUERYING VERTEX " << vertex << ". vertex_t = " << boostGraph[vertex].vertex_t << ". Point_d = " << boostGraph[vertex] << std::endl;
	printf("SEARCHING FOR %d NEAREST NEIGHBORS\n", n);
	K_neighbor_search search( *cgalTree, boostGraph[vertex], n );
	
	int count = 0;
	K_neighbor_search::iterator i;
	for ( i=search.begin(); i!=search.end(); ++i ) {
		neighbors.push_back(i->first.vertex_t);
		distances.push_back(i->second);
		count++;
	}
	
	printf("FOUND %d NEIGHBORS\n", count);
	printf("BoostGraph Vertex: %p, CGAL Point: %p\n", &boostGraph[vertex], &(search.begin()->first) );
	//printf("size of neighbors: %d\n", neighbors.size());
	//printf("size of distances: %d\n", distances.size());
	
	std::vector<vertex_t>::iterator j;
	std::vector<double>::iterator k;
	for ( j=neighbors.begin(), k=distances.begin(); j!=neighbors.end() && k!=distances.end(); j++, k++ )
		std::cout << "vertex " << *j << ": " << boostGraph[*j] << ", distance: " << *k << std::endl;
	std::cout << std::endl;
	
	return true;
}






/*void Roadmap::load( std::vector< std::vector<double> >& graphNodes, std::vector< std::pair<int,int> >& graphEdges, bool display )
{
	//TODO Clean out the boost graph first
	//int count = 0;
	
	vertex_t vertex;
	std::vector< vertex_t > vertices;
	std::vector< std::vector<double> >::iterator v;
	
	for ( v=graphNodes.begin(); v!=graphNodes.end(); ++v )
	{
		std::cout << "inserting: " << *(v->begin()) << " " << *(v->begin()+1) << " - ";
		
		std::vector<double> q( v->begin()+2, v->end() );
		for ( std::vector<double>::iterator j = q.begin(); j !=q.end(); ++j )
			std::cout << *j << " ";
		std::cout << std::endl;
		
		vertex = insert( *(v->begin()),
						 *(v->begin()+1),
						 q,
						 display
						);
		vertices.push_back( vertex );
	}
	
	sleep(1);
	
	std::vector< std::pair<int,int> >::iterator e;
	std::pair<edge_t, bool> edge;
	for ( e=graphEdges.begin(); e!=graphEdges.end(); ++e )
	{
		edge = boost::add_edge( vertices.at(e->first), vertices.at(e->second), map );
		//std::cout << edge.first << std::endl;
		if ( !edge.second )
			printf("boost::add_edge() failed.\n");
		else if ( display )
		{
			//printf("appended edge to boost graph\n");
			emit appendedEdge( edge.first, 
							   map[vertices.at(e->first)].qtGraphNode ,
							   map[vertices.at(e->second)].qtGraphNode );
		}
	}
	printf("loaded file: %d nodes, %d edges\n",graphNodes.size(),graphEdges.size());
}

void Roadmap::data( std::vector< std::vector<double> >* graphNodes, std::vector< std::pair<int,int> >* graphEdges )
{
	int count = 0;
	
	std::pair<vertex_i, vertex_i> vp;
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		map[*(vp.first)].idx = count;
		std::vector<qreal> thisLine;
		if ( map[*(vp.first)].qtGraphNode ) {
			thisLine.push_back( map[*(vp.first)].qtGraphNode->getNormX() );
			thisLine.push_back( map[*(vp.first)].qtGraphNode->getNormY() );
		} else {
			thisLine.push_back( 0.0 );
			thisLine.push_back( 0.0 );
		}
		thisLine.insert(thisLine.end(), map[*(vp.first)].q.begin(), map[*(vp.first)].q.end());
		graphNodes->push_back( thisLine );
		count ++;
	}
	
	std::pair<edge_i, edge_i> ep;
	std::pair<int,int> thisEdge;
	for (ep = edges(map); ep.first != ep.second; ++ep.first)
	{
		thisEdge.first = map[source(*(ep.first), map)].idx;
		thisEdge.second = map[target(*(ep.first), map)].idx;
		graphEdges->push_back( thisEdge );
	}
}*/

/*void Roadmap::readMapPoses(std::string filename) {
	cout << "loading poses" << endl;
	poses_map_t poses_map = read_poses(filename);
	poses_vector_t poses_torso(poses_map["CFGSPACE_TORSO"]);
	poses_vector_t poses_left(poses_map["CFGSPACE_LEFT_ARM"]);
	poses_vector_t poses_right(poses_map["CFGSPACE_RIGHT_ARM"]);

	poses_vector_t work_space(poses_map["WORKSPACE"]);
	for (size_t i(0); i < poses_torso.size(); ++i) {
		cout << i << endl;
		vector<double> combined_pose;
		copy(poses_torso[i].begin(), poses_torso[i].end(), back_inserter(combined_pose));
		copy(poses_right[i].begin(), poses_right[i].end(), back_inserter(combined_pose));
		copy(poses_left[i].begin(), poses_left[i].end(), back_inserter(combined_pose));

		insert(work_space[i][0], work_space[i][1], combined_pose, work_space[i]);
	}
	cout << "DONE loading poses" << endl;
}*/

void Roadmap::removeNode( Roadmap::vertex_t node )
{
	printf("\nremoving node %d\n", boostGraph[node].vertex_t);
	emit removeQtGraphNode(boostGraph[node].qtGraphNode);

	clear_vertex( node, boostGraph );
	remove_vertex( node, boostGraph );
	
	
	//cgalTree->invalidate_built();
	CGAL_Tree newTree;
	cgalTree = new CGAL_Tree();
	cgalTree->statistics(std::cout);
	
	Roadmap::vertex_i vi, vi_end;
	std::vector<Roadmap::Vertex> graphNodes;
	tie(vi, vi_end) = vertices(boostGraph);
	for (  ; vi != vi_end; ++vi ) {
		graphNodes.push_back( boostGraph[*vi] );
	}

	cgalTree->insert(graphNodes.begin(),graphNodes.end());
}

void Roadmap::removeEdge( Roadmap::edge_t edge )
{
	emit removeQtGraphEdge(boostGraph[edge].qtGraphEdge);
	remove_edge( edge, boostGraph );
}

void Roadmap::removeAllEdges()
{
	edge_i ei, ei_end, next;
	tie(ei, ei_end) = edges( boostGraph );
	for (next = ei; ei != ei_end; ei = next) {
		++next;
		removeEdge(*ei);
	}
}



/*void Roadmap::graphConnect( unsigned int n )
{
	std::pair<vertex_i, vertex_i> vp;
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		graphConnect( Pose(map[*(vp.first)].q.size(),map[*(vp.first)].q.begin(),map[*(vp.first)].q.end(),*(vp.first)), n );
	}
}*/

/*double Roadmap::calculate_distance( std::vector<double> const &v1,  std::vector<double> const &v2) {
	assert(v1.size() == v2.size());
	double distance(0.0);
	for (size_t i(0); i < v1.size(); ++i)
		distance += pow(v1[i] - v2[i], 2.0);
	return sqrt(distance);
}*/

/*Roadmap::vertex_t Roadmap::nearestWorkspaceVertex( std::vector<double> _w )
{
//	if ( _q.size() != dim ) { throw("wrong size state vector"); }

	//std::cout << "(" << _q.size() << "," << (unsigned int)iCub.getNumJoints() << ")" << std::endl;

//	K_neighbor_search search(tree, Pose( _q.size(), _q.begin(), _q.end() ) , 1);
//	map[search.begin()->first.vertex].type = type;
//	return search.begin()->first.vertex;

	double min_dist(9999999999999.);

	std::pair<vertex_i, vertex_i> vp = vertices(map);
	vertex_i best_vertex(vp.first);
	for (; vp.first != vp.second; ++(vp.first)) {
		double distance = calculate_distance(map[*(vp.first)].w, _w);
		if (min_dist > distance) {
			min_dist = distance;
			best_vertex = vp.first;
		}
	}

	return *best_vertex;
}*/



// project into 2D
/*void Roadmap::project2D( std::vector<double> direction )
{
	std::pair<vertex_i, vertex_i> vp;
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		if ( map[*(vp.first)].qtGraphNode )
			emit update2DPosition(map[*(vp.first)].qtGraphNode, QPointF((double)rand()/RAND_MAX,(double)rand()/RAND_MAX));
	}
}*/

/*void Roadmap::project2D( std::vector<double> direction )
{
	if ( direction.size() == 0 ) {
		//printf("choosing random direction\n");
		for ( unsigned int i = 0; i < dim; i++ ) {
			direction.push_back( (double)rand()/(double)RAND_MAX );
		}
	}
	
	//std::vector<double>::iterator fuck;
	//for ( fuck = direction.begin(); fuck !=direction.end(); ++fuck )
	//{
	//	printf("%f ",*fuck);
	//}
	//printf("\n");
	
	if ( direction.size() != dim ) { throw("wrong size direction vector"); }
	
	
	K::Vector_d i,j,k,r;
	double c;
	
	k = K::Vector_d( direction.size(), direction.begin(), direction.end() );	// the view direction
	
	//std::cout << std::endl << "VIEW DIRECTION..." << std::endl;
	//std::cout << "k: " << k << std::endl;
	//std::cout << "length: " << sqrt(k.squared_length()) << std::endl;
	
	k /= sqrt(k.squared_length());
	
	//std::cout << "normalized k: " << k << std::endl;
	//std::cout << "length: " << sqrt(k.squared_length()) <<  std::endl << std::endl;

	//std::list< std::pair< K::Vector_d, double > > pointSet;
	//std::list< std::pair< K::Vector_d, double > >::iterator point_i;

	// find a basis orthogonal to k
	bool iFound = false,
		 jFound = false;
	std::pair<vertex_i, vertex_i> vp;
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		c = 0;
		r = K::Vector_d(	map[*(vp.first)].q.size(), 
							map[*(vp.first)].q.begin(),
							map[*(vp.first)].q.end() );
		//r = getCgalPose(*(vp.first));
		
		//std::cout << "r: " << r << std::endl;
		
		if ( iFound && jFound ) { 
			break; 
		}
		else if ( iFound && !jFound ) {
			//std::cout << std::endl << "COMPUTE j..." << std::endl;
			//std::cout << "k: " << k << std::endl;
			//std::cout << "i: " << i << std::endl;
			
			//std::cout << std::endl << "PROJECT ONTO HYPERPLANE DEFINED BY k..." << std::endl;
			c = k*r;
			//std::cout << "c = k*r: " << c << std::endl;
			if ( c != sqrt(r.squared_length()) ) { 
				//std::cout << "kComponent = c*k: " << c*k << std::endl;
				r = r-c*k;												// project r onto the hyperplane perpendicular to k
				//j /= sqrt(i.squared_length());
				//std::cout << "r = r-c*k: " << r << std::endl;
				
				c = i*r;
				//std::cout << " c = i*r: " << c << std::endl;
				if ( c != sqrt(r.squared_length()) ) {
					//std::cout << "iComponent = c*i: " << c*i << std::endl;
					j = r-c*i;
					//std::cout << "j = r-c*i: " << j << std::endl;
					j /= sqrt(j.squared_length());
					//std::cout << "normalized j: " << j << std::endl;
					
					jFound = true;
					//std::cout << "j: " << j << std::endl;
					//std::cout << "length: " << sqrt(i.squared_length()) << std::endl << std::endl;
				}
			}
		}
		else if ( !iFound ) {
			c = k*r;
			//std::cout << std::endl << "COMPUTE i..." << std::endl << "k*r: " << c << std::endl;
			if ( c*c != r.squared_length() ) { 
				//std::cout << "c*k: " << c*k << std::endl;
				//std::cout << "r-c*k: " << r-c*k << std::endl;
				i = r-c*k;											// i is the projection of r onto the hyperplane perpendicular to k
				i /= sqrt(i.squared_length());
				iFound = true;
				//std::cout << "i: " << i << std::endl;
				//std::cout << "length: " << sqrt(i.squared_length()) << std::endl << std::endl;
			}
		}
	}
	
	//std::cout << "i dot j:" << i*j << std::endl;
	//std::cout << "i dot k:" << i*k << std::endl;
	//std::cout << "j dot k:" << j*k << std::endl;
			
	// find extreme points for scaling
	double	iMin,iMax,jMin,jMax,iVal,jVal;
	bool initialized = false;
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		r = K::Vector_d( map[*(vp.first)].q.size(), 
						 map[*(vp.first)].q.begin(),
						 map[*(vp.first)].q.end() );
		iVal = r*i;
		jVal = r*j;
		
		if (!initialized)
		{
			iMin = iVal; iMax = iVal;
			jMin = jVal; jMax = jVal;
			initialized = true;
		}
		
		if ( iVal > iMax ) iMax = iVal;
		else if ( iVal < iMin ) iMin = iVal;
		
		if ( jVal > jMax ) jMax = jVal;
		else if ( jVal < jMin ) jMin = jVal;
	}
	
	// project into 2D
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		r = K::Vector_d( map[*(vp.first)].q.size(), 
						map[*(vp.first)].q.begin(),
						map[*(vp.first)].q.end() );
		iVal = (r*i-iMin)/(iMax-iMin);
		jVal = (r*j-jMin)/(jMax-jMin);
	
		//map[*(vp.first)].qtGraphNode->setNormPos( iVal, jVal );
		//emit update2DPosition(map[*(vp.first)].qtGraphNode, iVal, jVal);
		//printf("node addy: %p\n", map[*(vp.first)].qtGraphNode);
		
		if ( map[*(vp.first)].qtGraphNode )
			emit update2DPosition(map[*(vp.first)].qtGraphNode, QPointF(iVal,jVal));
			//map[*(vp.first)].qtGraphNode->setNormPos( QPointF(iVal,jVal) );
			
	}
	
	
}*/

/****************************************************
 *** FOR MAINTENANCE OF "STATE" AND VISUALIZATION ***
 ****************************************************/
void Roadmap::setCurrentVertex( vertex_t v )
{
	if ( boostGraph[v].qtGraphNode )
		emit newNodeColor( boostGraph[v].qtGraphNode, Qt::red, Qt::darkRed );
	//map[v].qtGraphNode->setColor( Qt::yellow, Qt::darkYellow );
	
	if ( v != currentVertex )
	{
		if ( boostGraph[currentVertex].qtGraphNode )
			emit newNodeColor( boostGraph[currentVertex].qtGraphNode, Qt::lightGray, Qt::gray );
		//map[currentVertex].qtGraphNode->setColor( Qt::lightGray, Qt::gray );
	}
	
	currentVertex = v;
}

void Roadmap::setQtGraphNode( vertex_t v, QtGraphNode* n )
{
	//printf("new node addy: %p\n", n);
	boostGraph[v].qtGraphNode = n;
}

void Roadmap::setQtGraphEdge( edge_t e, QtGraphEdge* n )
{
	boostGraph[e].qtGraphEdge = n;
}

void Roadmap::setEdgeColor( edge_t e, QColor color )
{
	if ( boostGraph[e].qtGraphEdge )
		emit newEdgeColor( boostGraph[e].qtGraphEdge, color );
}

void Roadmap::setEdgeWeight( edge_t e, int w )
{
	if ( boostGraph[e].qtGraphEdge )
		emit newEdgeWeight( boostGraph[e].qtGraphEdge, w );
}
