#include "roadmap.h"

Roadmap::Roadmap( char* iCubName ) : dim(0)
{
	if ( !iCub.open(iCubName) ) { throw("failed to open iCub"); }
	dim = (unsigned int)iCub.getNumJoints();
}

Roadmap::~Roadmap()
{
}

void Roadmap::buildRandomMap( unsigned int numVertices, unsigned int numNeighbors )
{
	for ( unsigned int j=0; j<numVertices; j++ )
	{
		insert( iCub.getRandomPose(), numNeighbors );
	}
}

void Roadmap::insert( std::vector<double> _q, unsigned int n )
{
	if ( _q.size() != dim ) { throw("wrong size state vector"); }
	
	// put the configuration in the boost graph
	vertex_t vertex = boost::add_vertex( map );
	map[vertex].q = _q;
	
	// put it in the CGAL tree
	Pose p( _q.size(), _q.begin(), _q.end(), vertex );
	tree.insert( p );
	
	// connect it to its n nearest neighbors
	graphConnect( p, n );
}

void Roadmap::graphConnect( Pose p, unsigned int n )
{	
	K_neighbor_search search(tree, p , n+1);
	for(K_neighbor_search::iterator it = search.begin(); it != search.end(); ++it)
	{
		if ( it->second != 0 && !boost::edge( p.vertex, it->first.vertex, map ).second )
		{
			std::pair<edge_t, bool> edge = boost::add_edge( p.vertex, it->first.vertex, map );
			map[edge.first].length = it->second;
			//put(edge_weight, map,edge.first , map[edge.first].length);
			std::cout << "connected " << p.vertex << " - " << it->first.vertex << " " << "(" << map[edge.first].length << ")" << std::endl;
		}
	}
}

bool Roadmap::gotoNearest()
{
	printf("Trying a move to the nearest roadmap vertex\n");
	//iCub.setWaypoint();
	
	std::vector<double> p = iCub.getCurrentPose();
	vertex_t v = nearestVertex( p );
	
	if ( !iCub.positionMove( map[v].q ) ) { return 0; }
	
	return isOnMap();
}

bool Roadmap::motionCompleted()
{
	bool flag = false;
	do { 
		if ( !iCub.checkMotionDone(&flag) ) { break; }
		usleep(200000);
	}
	while ( !flag );
	
	return flag;
}

bool Roadmap::isOnMap()
{
	if ( !motionCompleted() ) { return 0; }
	
	std::vector<double> p = iCub.getCurrentPose();
	vertex_t v = nearestVertex( p );
	Pose b( map[v].q.size(), map[v].q.begin(), map[v].q.end(), NULL );
	Pose a(  p.size(), p.begin(), p.end(), NULL );
	
	if ( (b-a).squared_length() < 5.0 )
	{
		currentVertex = v;
		return iCub.setWaypoint();
		//return 1;
	}
	
	return 0;
}

Roadmap::vertex_t Roadmap::nearestVertex( std::vector<double> _q, char* type )
{
	if ( _q.size() != dim ) { throw("wrong size state vector"); }
	
	//std::cout << "(" << _q.size() << "," << (unsigned int)iCub.getNumJoints() << ")" << std::endl;
	
	K_neighbor_search search(tree, Pose( _q.size(), _q.begin(), _q.end(), NULL ) , 1);	
	map[search.begin()->first.vertex].type = type;
	return search.begin()->first.vertex;
}


bool Roadmap::randomMove()
{	
	printf("Trying a random position move on the roadmap\n");
	
	//if ( !isOnMap() ) { return 0; }
	
	std::vector<vertex_t> moves;
	Map::out_edge_iterator e, e_end;
	
	int count = 0;
	for (tie(e, e_end) = out_edges( currentVertex, map ); e != e_end; ++e)
	{
		moves.push_back( target(*e, map) );
		count++;
	}
	
	//std::cout << "count: " << count << std::endl;
	//std::cout << "size: " << moves.size() << std::endl;
	int idx = rand() % moves.size();
	//std::cout << "choose edge IDX: " << idx << std::endl;
	
	std::vector<double> pose = map[ moves.at(idx) ].q;
	
	printf("Position move:\n");
	for ( std::vector<double>::iterator i = pose.begin(); i != pose.end(); ++i )
	{
		printf("%f\n", *i);
	}
	
	//bool result = iCub.positionMove( map[ moves.at(idx) ].q );
	
	if ( !iCub.positionMove( pose ) ) { printf("failed\n"); return 0; }
	else { printf("succeded"); }

	return isOnMap();
}

/*bool Roadmap::insert( std::vector< std::vector<double> > _q )
{
	std::vector< std::vector<double> >::iterator i;
	for ( i=_q.begin(); i!=_q.end(); ++i )
	{
		if ( !insert( *i) ) { return 0; }
	}
	
	return 1;
}*/



/*bool Roadmap::nearestVertex( std::vector<double>& vertex, std::vector<double> queryPoint, char* type )
{
	if ( queryPoint.size() != (unsigned int)iCub.getNumJoints() ) { return 0; }
	
	vertex_t v;
	if ( nearestVertex( v, queryPoint ) )
	{
		vertex = map[v].q;
		return 1;
	}
	return 0;
}*/

std::vector<double> Roadmap::toStdVector( Pose p )
{
	std::vector<double> q;
	for ( Pose::Cartesian_const_iterator j = p.cartesian_begin(); j != p.cartesian_end(); ++j )
	{ 
		q.push_back(*j);
	}
	return q;
}

std::list<Roadmap::vertex_t> Roadmap::shortestPath( vertex_t from, vertex_t to )
{
	std::cout << std::endl << "Running Dijkstra's... " << from << " " << to << std::endl; 
	std::vector<vertex_t> parents(num_vertices(map));
	std::vector<double> distances(num_vertices(map));
	try {
		dijkstra_shortest_paths(	map, from, 
									weight_map( get(&Edge::length, map) )
									.predecessor_map( make_iterator_property_map(parents.begin(),get(boost::vertex_index, map)) )
									.distance_map( make_iterator_property_map(distances.begin(),get(boost::vertex_index, map)) )
									.visitor( target_visit(to,boost::on_examine_vertex()) )
								); 
	} catch( ... ) {}
	
	std::list<vertex_t> path;
	path.push_front(to);

	while ( path.front() != from )
	{
		path.push_front(parents[path.front()]);
	}
	
	return path;
}

