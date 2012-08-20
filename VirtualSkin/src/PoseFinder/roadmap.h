/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RoadmapDemo
 *	@{
 */

#ifndef ROADMAP_H_
#define ROADMAP_H_

#include <boost/graph/adjacency_list.hpp>

#include <CGAL/Cartesian_d.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits.h>

//#include <boost/graph/visitors.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <list>
#include <cmath>
#include <vector>

//#include "iCubController.h"

#include "astar.h"
#include "general.h"

#include <QThread>
//#include <QtGui/QGraphicsView>

//class Node;
class QtGraphNode;
class QtGraphEdge;

enum TreeMode {
	WORKSPACE = 0,
	CONFIGURATIONSPACE = 1,
	SCALEDCONFIGURATIONSPACE = 2
};

class Roadmap
{
public:
  
    typedef CGAL::Cartesian_d<double> K;
	
	/* FIRST, THE BOOST GRAPH */
	class Pose;
	struct Vertex { 
		int	idx;
		QtGraphNode* qtGraphNode;
		char* type;							//just for debugging
		std::vector<double> q;				// robot configuration
		std::vector<double> x;				// workspace configuration
	    std::string map_name;
		double fitness;
		int collisions;
		
		Vertex() : qtGraphNode(NULL), type(NULL) {}
	};
	
	struct Edge {
		QtGraphEdge* qtGraphEdge;
		double length, length2;
		double successRate;
		bool evaluated;
		Edge() : qtGraphEdge(NULL), length(1.0), successRate(1.0), evaluated(false) {}
	};
	
	typedef boost::adjacency_list<	boost::listS, boost::vecS, boost::undirectedS, 
									Vertex,
									Edge											>	Map;
	
	/* THEN A 'VISITOR' TO STOP DIJKSTRA'S WHEN THE TARGET VERTEX IS FOUND */
	template <class Vertex_Type, class Tag>
	struct target_visitor : public boost::default_dijkstra_visitor
	{
		target_visitor(Vertex_Type u) : v(u) { }
		
		template <class Graph>
		void examine_vertex(Vertex_Type u, Graph& g)
		{
			if( u == v )
			{
				printf("found target vertex\n");
				//printf("terminating dijkstra's\n");
				//throw(-1);
			}
		}
	private:
		Vertex_Type v;
	};
	
	template <class Vertex_Type, class Tag>
	target_visitor<Vertex_Type, Tag> target_visit(Vertex_Type u, Tag)
	{
		return target_visitor<Vertex_Type, Tag>(u);
	}
	
	/* NEAREST NEIGHBOR SEARCH WITH CGAL */
	
	class Pose : public K::Point_d {
		public:
			Pose (	int d, 
						std::vector<double>::iterator first,
						std::vector<double>::iterator last,
						Map::vertex_descriptor _boostGraphVertex = 0 ) :  K::Point_d(d,first,last), vertex(_boostGraphVertex) {}
			Map::vertex_descriptor vertex;
	};
	
	class Custom_search_traits {		
		public:
			typedef K::Cartesian_const_iterator_d Cartesian_const_iterator_d;
			typedef K::Construct_cartesian_const_iterator_d Construct_cartesian_const_iterator_d;
			typedef K::Construct_iso_box_d Construct_iso_box_d;
			typedef K::Construct_min_vertex_d Construct_min_vertex_d;
			typedef K::Construct_max_vertex_d Construct_max_vertex_d;
			typedef Pose Point_d;
			typedef K::Iso_box_d Iso_box_d;
			typedef K::FT FT;
			
			K::Construct_cartesian_const_iterator_d construct_cartesian_const_iterator_d_object() const {
					return K::Construct_cartesian_const_iterator_d();
			};
	};
	
	typedef CGAL::Orthogonal_k_neighbor_search<Custom_search_traits> K_neighbor_search;
	typedef K_neighbor_search::Tree Tree;
	/*****************************/
	
	//std::vector<double> randomSample();
	
	
	unsigned int			dim;
	Map						map;
	Tree					tree, workspace_tree, scaled_tree;

	Map::vertex_descriptor	currentVertex;
	Map::edge_descriptor	currentEdge;
	
	std::vector<double> scale_vector;

protected:
	void run();
	
public:
	
	typedef K::Point_d CGAL_Point;
	typedef Map::vertex_descriptor vertex_t;
	typedef Map::vertex_iterator vertex_i;
	typedef Map::edge_descriptor edge_t;
	typedef Map::edge_iterator edge_i;
	typedef Map::out_edge_iterator out_edge_i;
	typedef std::list<vertex_t> PathList;

	struct path_t {
	  path_t() : distance(0) {}
	  path_t(PathList p, double d, std::vector<double>& g) : path(p), distance(d), goal(g) {}

	  PathList path;
	  double distance;
	  std::vector<double> goal;
	};
	
	Roadmap();
	Roadmap(Roadmap const &other){}
	~Roadmap();
	
	std::vector<double> getStdPose( vertex_t v ) { return map[v].q; }
	CGAL_Point			getCgalPose( vertex_t v ) { return CGAL_Point( map[v].q.size(), map[v].q.begin(), map[v].q.end() ); }
	
	std::pair< edge_t, std::vector<double> > randomMove();
	std::list< std::pair< edge_t, vertex_t > > randomMoves();
	std::list< std::pair< edge_t, vertex_t > > aToB( vertex_t, vertex_t );
	
	
	int dimensionality() { return dim; }
	
	void setDimensionality( int );
	void setCurrentVertex( vertex_t );
//	void setEdgeColor( edge_t, QColor );
	

	vertex_t insert( std::vector<double> _x, std::vector<double> _q, std::string name);
	void graphConnect( Pose, unsigned int n, TreeMode tree_mode);
	void graphConnect( unsigned int n, TreeMode tree_mode = CONFIGURATIONSPACE);

	void random_connect(size_t n);
	void connect_delaunay();

	//void buildRandomMap( unsigned int numVertices, unsigned int numNeighbors );
	
	void load( std::vector< std::vector<double> >& graphNodes, std::vector< std::pair<int,int> >& graphEdges );
	void data( std::vector< std::vector<double> >* graphNodes, std::vector< std::pair<int,int> >* graphEdges );
	void readMapPoses(std::string filename, std::string mapname);

	void removeEdge( edge_t );
	void removeAllEdges();
	
	size_t n_edges() {return num_edges(map);}
	//bool startController();
	//bool gotoNearest();
	//bool isOnMap();
	//bool motionCompleted();	// wait until the motion is done or we are cut off from the robot
	//bool randomMove();
	
	//std::list< double[3] > 
	//void project2D( std::vector<double> d = std::vector<double>() );

	
	Map::vertex_descriptor nearestVertex( std::vector<double>, char* type="" );
	Map::vertex_descriptor nearestWorkspaceVertex( std::vector<double> _w );

	std::vector<double> nearestVector(std::vector<double> q);
	std::vector<double> nearestWorkspaceVector(std::vector<double> q);

	path_t shortestPath( std::vector<double> from, std::vector<double> to, EdgeTester<edge_t> &edge_tester);

	//PathList shortestPath_backup( Map::vertex_descriptor from, Map::vertex_descriptor to ); //DIJKSTRA

	path_t shortestPath( vertex_t from, vertex_t to, EdgeTester<edge_t> &edge_tester); //ASTAR

	std::vector<std::vector<double> > vertex_list_to_q(std::list<Roadmap::vertex_t> &list);


	size_t size() {
		return num_vertices(map);
	}
	//bool insert( std::vector< std::vector<double> > );

	std::pair<std::vector<float>, std::vector<float> > get_workspace_bounding_box() {
		std::pair<std::vector<float>, std::vector<float> > bbox;
		for (size_t i(0); i < workspace_tree.bounding_box().dimension(); ++i) {
			bbox.first.push_back(workspace_tree.bounding_box().min_coord(i));
			bbox.second.push_back(workspace_tree.bounding_box().max_coord(i));
		}
		return bbox;
	}

	std::vector<double>  scale_q( std::vector<double> q);
	std::vector<double>  unscale_q( std::vector<double> scaled_q);
	Tree &get_tree(TreeMode tree_mode);

//
//signals:
//
//	void appendedNode( vertex_t, qreal, qreal );
//	void appendedEdge( edge_t, QtGraphNode*, QtGraphNode* );
//	void update2DPosition( QtGraphNode*, QPointF );
//	void newNodeColor( QtGraphNode*, QColor, QColor );
//	void newEdgeColor( QtGraphEdge*, QColor );
//	//void removeQtGraphEdge( QtGraphEdge* );
//
//public slots:
//
//	void setQtGraphNode( vertex_t, QtGraphNode* );
//	void setQtGraphEdge( edge_t, QtGraphEdge* );
	
//	friend class ControlThread;
};
#endif
/** @} */
