/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RoadmapDemo
 *	@{
 */

#ifndef ROADMAP_H_
#define ROADMAP_H_

//#include <boost/graph/visitors.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <CGAL/Cartesian_d.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits.h>

//#include <list>
//#include <cmath>
//#include <vector>

//#include "iCubController.h"

//#include <QThread>
#include <QtGui/QGraphicsView>

//class Node;
class QtGraphNode;
class QtGraphEdge;

class Roadmap : public QObject
{
	Q_OBJECT
	
public:
	
	/************************************************************************
	 *** Here we set up a graph using the Boost Graph Library,            ***
	 *** where the Vertices of the graph are d-dimensional points in CGAL ***
	 ************************************************************************/
	
	typedef CGAL::Cartesian_d<double> K;

	// this boost graph vertex that is also a d-dimensional point in CGAL 
	class Vertex : public K::Point_d {
	public:
		Vertex() : qtGraphNode(NULL){}
		Vertex( size_t d, 
				std::vector<double>::iterator first,
				std::vector<double>::iterator last ) :	K::Point_d(d,first,last),
														qtGraphNode(NULL){}
		/*Vertex( int d, 
				qreal* _q ) : qtGraphNode(NULL)
		{
			std::vector<qreal> q;
			for ( int i=0; i<d; i++ ) q.push_back(_q[i]);
			setPose(q.size(),q.begin(),q.end());
		}
		
		void setPose( int d, 
					  std::vector<qreal>::iterator first,
					  std::vector<qreal>::iterator last)
		{
			Vertex v(d,first,last);
			v.qtGraphNode = this->qtGraphNode;
			*this = v;
		}*/
		
		//size_t vertex_descriptor;
		size_t vertex_t;
		QtGraphNode* qtGraphNode;
		//char* type;							//just for debugging
		//std::vector<qreal> q;				// robot configuration
		//std::vector<qreal> w;				// workspace 
		//qreal x,y;
	};
	
	// these properties control CGAL's d-dimensional nearest neighbor search
	class Custom_search_traits {		
	public:
		typedef K::Cartesian_const_iterator_d Cartesian_const_iterator_d;
		typedef K::Construct_cartesian_const_iterator_d Construct_cartesian_const_iterator_d;
		typedef Vertex Point_d;
		typedef K::FT FT;
	};
	
	// these simplify the naming of CGAL's d-dimensional nearest neighbor search 
	typedef CGAL::Orthogonal_k_neighbor_search<Custom_search_traits> K_neighbor_search;
	typedef K_neighbor_search::Tree CGAL_Tree;
	
	// an edge for the Boost graph
	struct Edge {
		QtGraphEdge* qtGraphEdge;
		qreal squaredLength;
		Edge() : qtGraphEdge(NULL)
		{}
	};
	
	// The actual boost graph
	typedef boost::adjacency_list<	boost::listS, 
									boost::vecS, 
									boost::bidirectionalS, 
									Vertex, 
									Edge, 
									boost::disallow_parallel_edge_tag >	Boost_Graph;
	
	/* DIJKSTRA'S SHORTEST PATH */
	template <class Vertex_Type, class Tag>
	struct target_visitor : public boost::default_dijkstra_visitor
	{
		target_visitor(Vertex_Type u) : v(u) {}
		
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
	/*****************************/
	
	//unsigned int			dim;

public:
	Boost_Graph						boostGraph;
	CGAL_Tree						*cgalTree;
	Boost_Graph::vertex_descriptor	currentVertex;
	
	//Map::edge_descriptor	currentEdge;
	//QtGraphEdge*			deletedEdge;
	
//protected:
	//void run();
	
public:
	
	typedef K::Vector_d CGAL_Vector;
	typedef K::Point_d CGAL_Point;
	typedef Boost_Graph::vertex_descriptor vertex_t;
	typedef Boost_Graph::vertex_iterator vertex_i;
	typedef Boost_Graph::edge_descriptor edge_t;
	typedef Boost_Graph::edge_iterator edge_i;
	typedef Boost_Graph::out_edge_iterator out_edge_i;
	typedef Boost_Graph::in_edge_iterator in_edge_i;
	
	Roadmap(){ cgalTree = new CGAL_Tree(); }
	~Roadmap(){}
	
	bool insert( std::vector<qreal> q, vertex_t& v, bool display = true );
	bool connect( vertex_t v, vertex_t w );
	//bool graphConnect( vertex_t v, unsigned int n = 3 );
	
	bool	randomNeighbor( vertex_t vertex, vertex_t& neighbor );
	bool	shortestPath( vertex_t from, vertex_t to, std::list<vertex_t>& path );
	bool	nearestVertices( vertex_t vertex, std::vector<vertex_t>& neighbors, std::vector<double>& distances, int n );
	
	//int dimensionality() { return dim; }
	
	//void setDimensionality( int );
	void setCurrentVertex( vertex_t );
	void setEdgeColor( edge_t, QColor );
	void setEdgeWeight( edge_t, int );
	
	
	
	
	//void load( std::vector< std::vector<qreal> >& graphNodes, std::vector< std::pair<int,int> >& graphEdges, bool display = true );
	//void data( std::vector< std::vector<qreal> >* graphNodes, std::vector< std::pair<int,int> >* graphEdges );
	
	//void readMapPoses(std::string filename);

	void removeNode( vertex_t );
	void removeEdge( edge_t );
	void removeAllEdges();
	
	//bool startController();
	//bool gotoNearest();
	//bool isOnMap();
	//bool motionCompleted();	// wait until the motion is done or we are cut off from the robot
	//bool randomMove();
	
	//std::list< qreal[3] > 
	//void project2D( std::vector<qreal> d = std::vector<qreal>() );
	
	
	//std::list<vertex_t> shortestPath( vertex_t from, vertex_t to );
	
private:
	//qreal calculate_distance( std::vector<qreal> const &v1,  std::vector<qreal> const &v2);
	//Roadmap::vertex_t nearestWorkspaceVertex( std::vector<qreal> _w );
	//bool insert( std::vector< std::vector<qreal> > );
	
signals:
	
	void appendedNode( vertex_t );
	void appendedEdge( edge_t, QtGraphNode*, QtGraphNode* );
	void update2DPosition( QtGraphNode*, QPointF );
	void newNodeColor( QtGraphNode*, QColor, QColor );
	void newEdgeColor( QtGraphEdge*, QColor );
	void newEdgeWeight( QtGraphEdge*, int );
	void removeQtGraphNode( QtGraphNode* );
	void removeQtGraphEdge( QtGraphEdge* );
	
public slots:
	
	void setQtGraphNode( vertex_t, QtGraphNode* );
	void setQtGraphEdge( edge_t, QtGraphEdge* );
	
	//friend class ControlThread;
};
#endif
/** @} */
