#include "roadmap.h"
//#include "graphwidget.h"
#include "widgetEdge.h"
#include "widgetNode.h"

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
	std::pair<vertex_i, vertex_i> vp = vertices(map);
	if ( vp.first == vp.second )
	{
		dim = d;
		return;
	}
	printf("the map is not empty!!!\n");
}

void Roadmap::setCurrentVertex( vertex_t v )
{
	if ( map[v].qtGraphNode )
		emit newNodeColor( map[v].qtGraphNode, Qt::yellow, Qt::darkYellow );
		//map[v].qtGraphNode->setColor( Qt::yellow, Qt::darkYellow );
	
	if ( v != currentVertex )
	{
		if ( map[currentVertex].qtGraphNode )
			emit newNodeColor( map[currentVertex].qtGraphNode, Qt::lightGray, Qt::gray );
			//map[currentVertex].qtGraphNode->setColor( Qt::lightGray, Qt::gray );
	}

	currentVertex = v;
}

void Roadmap::setEdgeColor( edge_t e, QColor color )
{
	if ( map[e].qtGraphEdge )
		emit newEdgeColor( map[e].qtGraphEdge, color );
		//map[e].qtGraphEdge->setColor( color );
}

std::pair< Roadmap::edge_t, std::vector<double> > Roadmap::randomMove()
{
	std::pair< edge_t, std::vector<double> > result;
	if ( num_vertices( map ) > 0 )
	{
		std::vector<edge_t> moves;
		Map::out_edge_iterator e, e_end;
		
		vertex_t v = currentVertex;
		
		//int count = 0;
		for (tie(e, e_end) = out_edges( v, map ); e != e_end; ++e)
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

std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > Roadmap::randomMoves()
{
	std::list< std::pair< edge_t, vertex_t > > result;
	if ( num_vertices( map ) > 0 )
	{
		vertex_t rand_vertex = (vertex_t)(rand() % (int)num_vertices(map));
		std::list<Map::vertex_descriptor> vertex_list = shortestPath( currentVertex, rand_vertex );
		for ( std::list<Map::vertex_descriptor>::iterator i = vertex_list.begin(); i != vertex_list.end(); )
		{
			vertex_t a = *i;
			vertex_t b = *(++i);
			std::pair<edge_t,bool> ab = edge(a,b,map);
			if (ab.second)
				result.push_back( std::pair< edge_t, vertex_t >( ab.first, b ) );
		}
	}
	return result;
}

/*void Roadmap::buildRandomMap( unsigned int numVertices, unsigned int numNeighbors )
{
	for ( unsigned int j=0; j<numVertices; j++ )
	{
		//insert( randomSample(), numNeighbors );
	}
}*/

Roadmap::vertex_t Roadmap::insert( qreal _x, qreal _y, std::vector<double> _q /*, unsigned int n*/ )
{
	//printf("called insert\n");
	if ( _q.size() != dim ) { printf("wrong size state vector %d\n",_q.size()); throw("wrong size state vector"); }
	
	// put the configuration in the boost graph
	vertex_t vertex = boost::add_vertex( map );
	map[vertex].q = _q;
	//map[vertex].x = _x;
	//map[vertex].y = _y;
	
	emit appendedNode( vertex, _x, _y );
	
	// put it in the CGAL tree
	Pose p( _q.size(), _q.begin(), _q.end(), vertex );
	tree.insert( p );
	
	// connect it to its n nearest neighbors
	//graphConnect( p, n );
	
	printf("inserted");
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
		std::cout << "inserting: " << *(v->begin()) << " " << *(v->begin()+1) << " - ";
		
		std::vector<double> q( v->begin()+2, v->end() );
		for ( std::vector<double>::iterator j = q.begin(); j !=q.end(); ++j )
			std::cout << *j << " ";
		std::cout << std::endl;
		
		vertex = insert( *(v->begin()),
						 *(v->begin()+1),
						 q
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
		else
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
}

void Roadmap::removeEdge( Roadmap::edge_t edge )
{
	//if ( map[edge].qtGraphEdge )
		//map[edge].qtGraphEdge->flagRemoval();
	
	//emit removeQtGraphEdge(map[edge].qtGraphEdge);
	
	
	//remove_edge( edge, map );
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
	//printf("new node addy: %p\n", n);
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
			map[edge.first].length = it->second;
			//std::cout << "connected " << p.vertex << " - " << it->first.vertex << " " << "(" << map[edge.first].length << ")" << std::endl;
			emit appendedEdge( edge.first, 
							  map[p.vertex].qtGraphNode ,
							  map[it->first.vertex].qtGraphNode );
		}
	}
}

void Roadmap::graphConnect( unsigned int n )
{
	std::pair<vertex_i, vertex_i> vp;
	for (vp = vertices(map); vp.first != vp.second; ++vp.first)
	{
		graphConnect( Pose(map[*(vp.first)].q.size(),map[*(vp.first)].q.begin(),map[*(vp.first)].q.end(),*(vp.first)), n );
	}
}

Roadmap::vertex_t Roadmap::nearestVertex( std::vector<double> _q, char* type )
{
	if ( _q.size() != dim ) { throw("wrong size state vector"); }
	
	//std::cout << "(" << _q.size() << "," << (unsigned int)iCub.getNumJoints() << ")" << std::endl;
	
	K_neighbor_search search(tree, Pose( _q.size(), _q.begin(), _q.end() ) , 1);	
	map[search.begin()->first.vertex].type = type;
	return search.begin()->first.vertex;
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
		if ( parents[path.front()] == path.front() )
		{
			printf("target pose unreachable. no path through the graph\n");
			break;
		}
		path.push_front(parents[path.front()]);
	}
	
	printf("path: ");
	for (std::list<vertex_t>::iterator i = path.begin(); i != path.end(); ++i )
	{
		printf("%d ",*i);
	}
	printf("\n");
	
	
	return path;
}

void Roadmap::project2D( std::vector<double> direction )
{
	if ( direction.size() == 0 ) {
		//printf("choosing random direction\n");
		for ( unsigned int i = 0; i < dim; i++ ) {
			direction.push_back( (double)rand()/(double)RAND_MAX );
		}
	}
	
	/*std::vector<double>::iterator fuck;
	for ( fuck = direction.begin(); fuck !=direction.end(); ++fuck )
	{
		printf("%f ",*fuck);
	}
	printf("\n");*/
	
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
	
	
}
