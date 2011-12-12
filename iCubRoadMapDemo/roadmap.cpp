#include "roadmap.h"
//#include "graphwidget.h"
#include "widgetEdge.h"
#include "widgetNode.h"

Roadmap::Roadmap() : dim(0)
{
	srand ( time(NULL) );
}

Roadmap::~Roadmap()
{
}


void Roadmap::setDimensionality( int d )
{
	// dimensionality can only be set when the map is empty
	std::pair<vertex_i, vertex_i> vp = vertices(map);
	if ( vp.first == vp.second )
	{
		dim = d;
	}
}

void Roadmap::setCurrentVertex( vertex_t v )
{
	//TODO: should really check if the vertex exists, but how?
	currentVertex = v;
}


std::vector<double> Roadmap::randomMove()
{
	std::vector<edge_t> moves;
	Map::out_edge_iterator e, e_end;
	
	int count = 0;
	for (tie(e, e_end) = out_edges( currentVertex, map ); e != e_end; ++e)
	{
		moves.push_back( *e );
		count++;
	}
	int idx = rand() % moves.size();
	
	//std::cout << "count: " << count << std::endl;
	//std::cout << "size: " << moves.size() << std::endl;
	//std::cout << "chose edge IDX: " << idx << std::endl;
	
	return map[target(moves.at(idx),map)].q ;
}

/*bool Roadmap::openRobot( const char* iCubName )
{
	if ( iCub.open(iCubName) )
	{
		dim = (unsigned int)iCub.getNumJoints();
		return true;
	}
	return false;
}

void Roadmap::closeRobot()
{
	if ( iCub.isValid() )
	{
		dim = 0;
		iCub.close();
	}
}

std::vector<double> Roadmap::randomSample()
{
	std::vector<double> q;
	if ( iCub.isValid() )
	{
		q = iCub.getRandomPose();
		emit newSample(q);
	}
	return q;
	std::vector<double> q;
	q.clear();
	for ( unsigned int j=0; j<dim; j++ )
	{
		q.push_back( (double)rand()/(double)RAND_MAX );
	}
	return q;
}

std::vector<double> Roadmap::currentPose()
{
	std::vector<double> q;
	if ( iCub.isValid() )
	{
		q = iCub.getCurrentPose();
		emit newSample(q);
	}
	return q;
}*/

//void Roadmap::computeSample()
//{
//	emit newSample(randomSample());
//}

void Roadmap::buildRandomMap( unsigned int numVertices, unsigned int numNeighbors )
{
	for ( unsigned int j=0; j<numVertices; j++ )
	{
		//insert( randomSample(), numNeighbors );
	}
}

Roadmap::vertex_t Roadmap::insert( std::vector<double> _q, unsigned int n )
{
	//printf("called insert\n");
	if ( _q.size() != dim ) { throw("wrong size state vector"); }
	
	// put the configuration in the boost graph
	vertex_t vertex = boost::add_vertex( map );
	map[vertex].q = _q;
	//map[vertex].qtGraphNode = widget.addNode();
	
	emit appendedNode( vertex );
	
	// put it in the CGAL tree
	Pose p( _q.size(), _q.begin(), _q.end(), vertex );
	tree.insert( p );
	
	// connect it to its n nearest neighbors
	graphConnect( p, n );
	
	//printf("inserted");
	return vertex;
}

void Roadmap::load( std::vector< std::vector<double> >& graphNodes, std::vector< std::pair<int,int> >& graphEdges )
{
	//TODO Clean out the boost graph first
	//int count = 0;
	
	vertex_t vertex;
	std::vector< vertex_t > vertices;
	std::vector< std::vector<double> >::iterator v;
	
	for ( v=graphNodes.begin(); v!=graphNodes.end(); ++v )
	{
		vertex = insert(*v);
		vertices.push_back( vertex );
		//emit appendedNode( vertex );
	}
	
	sleep(1);
	std::vector< std::pair<int,int> >::iterator e;
	std::pair<edge_t, bool> edge;
	for ( e=graphEdges.begin(); e!=graphEdges.end(); ++e )
	{
		edge = boost::add_edge( vertices.at(e->first), vertices.at(e->second), map );
		if ( !edge.second )
			printf("boost::add_edge() failed.\n");
		else
		{
			printf("appended edge to boost graph\n");
			emit appendedEdge( edge.first, 
							   map[vertices.at(e->first)].qtGraphNode ,
							   map[vertices.at(e->second)].qtGraphNode );
	
		}
	}
	
}
void Roadmap::data( std::vector< std::vector<double> >* graphNodes, std::vector< std::pair<int,int> >* graphEdges )
{
	int count = 0;
	
	std::pair<vertex_i, vertex_i> vp;
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		map[*(vp.first)].idx = count;
		graphNodes->push_back( map[*(vp.first)].q );
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
}

void Roadmap::removeEdge( Roadmap::edge_t edge )
{
	emit removeQtGraphEdge(map[edge].qtGraphEdge);
	remove_edge( edge, map );
}
void Roadmap::removeAllEdges()
{
	edge_i ei, ei_end, next;
	tie(ei, ei_end) = edges(map);
	for (next = ei; ei != ei_end; ei = next) {
		++next;
		removeEdge(*ei);
	}
}


void Roadmap::setQtGraphNode( vertex_t v, QtGraphNode* n )
{
	map[v].qtGraphNode = n;
}

void Roadmap::setQtGraphEdge( edge_t e, QtGraphEdge* n )
{
	map[e].qtGraphEdge = n;
}

void Roadmap::graphConnect( Pose p, unsigned int n )
{	
	K_neighbor_search search(tree, p , n+1);
	for(K_neighbor_search::iterator it = search.begin(); it != search.end(); ++it)
	{
		if ( it->second != 0 && !boost::edge( p.vertex, it->first.vertex, map ).second )
		{
			std::pair<edge_t, bool> edge = boost::add_edge( p.vertex, it->first.vertex, map );
			//widget.addEdge (map[p.vertex].qtGraphNode , map[it->first.vertex].qtGraphNode );
			
			emit appendedEdge( edge.first, map[p.vertex].qtGraphNode , map[it->first.vertex].qtGraphNode );
			
			map[edge.first].length = it->second;
			//put(edge_weight, map,edge.first , map[edge.first].length);
			std::cout << "connected " << p.vertex << " - " << it->first.vertex << " " << "(" << map[edge.first].length << ")" << std::endl;
			
		}
	}
}

/*bool Roadmap::gotoNearest()
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
}*/

Roadmap::vertex_t Roadmap::nearestVertex( std::vector<double> _q, char* type )
{
	if ( _q.size() != dim ) { throw("wrong size state vector"); }
	
	//std::cout << "(" << _q.size() << "," << (unsigned int)iCub.getNumJoints() << ")" << std::endl;
	
	K_neighbor_search search(tree, Pose( _q.size(), _q.begin(), _q.end(), NULL ) , 1);	
	map[search.begin()->first.vertex].type = type;
	return search.begin()->first.vertex;
}


/*bool Roadmap::randomMove()
{	
	printf("Trying a random position move on the roadmap\n");
	
	//if ( !isOnMap() ) { return 0; }
	
	std::vector<edge_t> moves;
	Map::out_edge_iterator e, e_end;
	
	int count = 0;
	for (tie(e, e_end) = out_edges( currentVertex, map ); e != e_end; ++e)
	{
		moves.push_back( *e );
		count++;
	}
	
	//std::cout << "count: " << count << std::endl;
	//std::cout << "size: " << moves.size() << std::endl;
	int idx = rand() % moves.size();
	//std::cout << "choose edge IDX: " << idx << std::endl;
	
	std::vector<double> pose = map[target(moves.at(idx),map)].q ; //map[ moves.at(idx) ].q;
	
	printf("Position move:\n");
	for ( std::vector<double>::iterator i = pose.begin(); i != pose.end(); ++i )
	{
		printf("%f\n", *i);
	}
	
	//bool result = iCub.positionMove( map[ moves.at(idx) ].q );
	
	if ( !iCub.positionMove( pose ) )
	{ 
		//widget.removeEdge( map[ moves.at(idx) ].qtGraphEdge );
		printf("failed\n"); return 0;
	}
	else { printf("succeded"); }
	

	return isOnMap();
}

std::vector<double> Roadmap::toStdVector( Pose p )
{
	std::vector<double> q;
	for ( Pose::Cartesian_const_iterator j = p.cartesian_begin(); j != p.cartesian_end(); ++j )
	{ 
		q.push_back(*j);
	}
	return q;
}*/



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


void Roadmap::project2D( std::vector<double> direction )
{
	if ( direction.size() == 0 ) {
		for ( unsigned int i = 0; i < dim; i++ ) {
			direction.push_back( (double)rand()/(double)RAND_MAX );
		}
	}
	
	if ( direction.size() != dim ) { throw("wrong size direction vector"); }
	
	
	K::Vector_d i,j,k,r;
	double c;
	
	k = K::Vector_d( direction.size(), direction.begin(), direction.end() );	// the view direction
	
	std::cout << std::endl << "VIEW DIRECTION..." << std::endl;
	std::cout << "k: " << k << std::endl;
	std::cout << "length: " << sqrt(k.squared_length()) << std::endl;
	
	k /= sqrt(k.squared_length());
	
	std::cout << "normalized k: " << k << std::endl;
	std::cout << "length: " << sqrt(k.squared_length()) <<  std::endl << std::endl;

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
		
		std::cout << "r: " << r << std::endl;
		
		if ( iFound && jFound ) { 
			break; 
		}
		else if ( iFound && !jFound ) {
			std::cout << std::endl << "COMPUTE j..." << std::endl;
			std::cout << "k: " << k << std::endl;
			std::cout << "i: " << i << std::endl;
			
			std::cout << std::endl << "PROJECT ONTO HYPERPLANE DEFINED BY k..." << std::endl;
			c = k*r;
			std::cout << "c = k*r: " << c << std::endl;
			if ( c != sqrt(r.squared_length()) ) { 
				std::cout << "kComponent = c*k: " << c*k << std::endl;
				r = r-c*k;												// project r onto the hyperplane perpendicular to k
				//j /= sqrt(i.squared_length());
				std::cout << "r = r-c*k: " << r << std::endl;
				
				c = i*r;
				std::cout << " c = i*r: " << c << std::endl;
				if ( c != sqrt(r.squared_length()) ) {
					std::cout << "iComponent = c*i: " << c*i << std::endl;
					j = r-c*i;
					std::cout << "j = r-c*i: " << j << std::endl;
					j /= sqrt(j.squared_length());
					std::cout << "normalized j: " << j << std::endl;
					
					jFound = true;
					std::cout << "j: " << j << std::endl;
					std::cout << "length: " << sqrt(i.squared_length()) << std::endl << std::endl;
				}
			}
		}
		else if ( !iFound ) {
			c = k*r;
			std::cout << std::endl << "COMPUTE i..." << std::endl << "k*r: " << c << std::endl;
			if ( c*c != r.squared_length() ) { 
				std::cout << "c*k: " << c*k << std::endl;
				std::cout << "r-c*k: " << r-c*k << std::endl;
				i = r-c*k;											// i is the projection of r onto the hyperplane perpendicular to k
				i /= sqrt(i.squared_length());
				iFound = true;
				std::cout << "i: " << i << std::endl;
				std::cout << "length: " << sqrt(i.squared_length()) << std::endl << std::endl;
			}
		}
	}
	
	std::cout << "i dot j:" << i*j << std::endl;
	std::cout << "i dot k:" << i*k << std::endl;
	std::cout << "j dot k:" << j*k << std::endl;
	
	double	iMin = 0,
			iMax = 0,
			jMin = 0,
			jMax = 0,
			iVal = 0,
			jVal = 0;
			
	// find extreme points for scaling
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		r = K::Vector_d( map[*(vp.first)].q.size(), 
						 map[*(vp.first)].q.begin(),
						 map[*(vp.first)].q.end() );
		iVal = r*i;
		jVal = r*j;
		
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
	
		//map[*(vp.first)].qtGraphNode->setPos( iVal, jVal );
		emit update2DPosition(map[*(vp.first)].qtGraphNode, iVal, jVal);
	}
}

/*
bool Roadmap::startController()
{
	if ( hasRobot() )
	{
		start();
		return true;
	}
	return false;
}

void Roadmap::run()
{
	yarp::os::Network yarp;
	yarp::os::Port vSkinStatus;
	vSkinStatus.open("/statusOut");
	if ( !yarp.connect("/filterStatus","/statusOut") )
	{ 
		printf("failed to connect to robot filter status port\n");
	}
	if ( !gotoNearest() )
	{ 
		printf("Failed to move iCub onto the roadmap\n");
	} else {
		std::cout << "iCub is on the roadmap" << std::endl;
		
		yarp::os::Bottle b;
		while (true)
		{
			vSkinStatus.read(b);
			printf("Filter status: %s\n",b.toString().c_str());
			if (  b.get(0).asInt() == 1 )
			{
				if ( !isOnMap() ) 
				{
					if ( !gotoNearest() )  { printf("Failed to move iCub onto the roadmap\n"); } 
				}
				if ( randomMove() ) std::cout << "OK" << std::endl;			
				else std::cout << "OH NO!!!" << std::endl;
				
				// MODIFY GRAPH EDGES HERE
			
			}
			else { printf("waiting for filter to open\n"); }
			usleep(100000);
		}
	}
}*/