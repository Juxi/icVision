/*******************************************************************
***         Copyright (C) 2012 Leo Pape, Marijn Stollenga       ***
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

#include "astar.h"
#include "general.h"
#include "treemode.h"
#include "astar.h"

class Roadmap
{
public:

	typedef CGAL::Cartesian_d<double> K;

	/* FIRST, THE BOOST GRAPH */
	class Pose;
	struct Vertex { 
		int	idx;
		char* type;							//just for debugging
		std::vector<double> q;				// robot configuration
		std::vector<double> qs;              // scaled robot configuration
		std::vector<double> x;				// workspace configuration
		std::string map_name;
		double fitness;
		Vertex() : type(NULL) {}
	};

	struct Edge {
		double length_x, length_q, length_qs, length_evaluated;
		double successRate;
		bool evaluated;
		Edge() : length_x(1.0), length_q(1.0), length_qs(1.0), length_evaluated(1.0), successRate(1.0), evaluated(false) {}
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

	class VertexTester {
	public:
		virtual bool check(std::vector<double> &q1, std::vector<double> &q2) = 0;
	};

	typedef K::Point_d CGAL_Point;
	typedef Map::vertex_descriptor vertex_t;
	typedef Map::vertex_iterator vertex_i;
	typedef Map::edge_descriptor edge_t;
	typedef Map::edge_iterator edge_i;
	typedef Map::out_edge_iterator out_edge_i;
	typedef std::vector<vertex_t> Path;

	struct path_t {
		path_t() : length(0.0) {}
		path_t(Path p, double l) : path(p), length(l) {}
		Path path;
		double length;
	};
	
	Roadmap();
	Roadmap(Roadmap const &other){}
	~Roadmap();

	Map	&getMap() { return map; };

	int dimensionality() { return dim; }
	size_t n_edges() {return num_edges(map);}
	size_t size() { return num_vertices(map); }

	void setDimensionality( int d);
	void setCurrentVertex( vertex_t );
	void setScaleVector(std::vector<double> &s) { scale_vector = s; };
	std::vector<std::vector<double> > vertex_list_to_q(Path &p);
	std::vector<std::vector<double> > vertex_list_to_qs(Path &p);
	std::vector<std::vector<double> > vertex_list_to_x(Path &p);
	std::vector<double> scale_q( std::vector<double> q);
	std::vector<double> unscale_q( std::vector<double> sq);

	std::pair<std::vector<float>, std::vector<float> > get_workspace_bounding_box();

	Tree &get_tree(TreeMode mode=CONNECTIONMODE);
	TreeMode getConMode() { return conmode; };

	std::vector<double> getStdPose( vertex_t v ) { return map[v].q; }
	CGAL_Point			getCgalPose( vertex_t v ) { return CGAL_Point( map[v].q.size(), map[v].q.begin(), map[v].q.end() ); }

	vertex_t insert( std::vector<double> _x, std::vector<double> _q, std::string name);
	void removeEdge( edge_t );
	void removeAllEdges();

	std::pair< edge_t, std::vector<double> > randomMove();
	std::list< std::pair< edge_t, vertex_t > > randomMoves();
	std::list< std::pair< edge_t, vertex_t > > aToB( vertex_t, vertex_t );

	void graphConnect( Pose, unsigned int n);
	void graphConnect( unsigned int n, TreeMode tree_mode);
	void graphConnect2( int p, unsigned int n);
	void graphConnect2( unsigned int n, TreeMode tree_mode);
	void mapDistances(int v, std::vector<double> &d);

	path_t shortestPath(vertex_t &from, vertex_t &to, EdgeTester<edge_t> &edge_tester, TreeMode mode=CONNECTIONMODE); //ASTAR
	vertex_t nearestVertex(std::vector<double> &v, VertexTester &evaluate, TreeMode mode, char* type="");

	//std::vector<double> randomSample();
	//void random_connect(size_t n);
	//void connect_delaunay();
	//void buildRandomMap( unsigned int numVertices, unsigned int numNeighbors );

	void load( std::vector< std::vector<double> >& graphNodes, std::vector< std::pair<int,int> >& graphEdges );
	void data( std::vector< std::vector<double> >* graphNodes, std::vector< std::pair<int,int> >* graphEdges );
	void readMapPoses(std::string filename, std::string mapname);
	void write_graph(std::string filename);

	/*class label_writer {
	Map &d_map;
	public:
	label_writer(Map &map) : d_map(map) {}
	void operator()(std::ostream& out, const vertex_t& v) const {
	out << "[label=\"" << d_map[v].x[0] << "\"]";
	}
	};*/

private:
	Map						map;
	unsigned int			dim;
	Tree					tree_q, tree_x, tree_qs;
	TreeMode                conmode;

	Map::vertex_descriptor	currentVertex;
	Map::edge_descriptor	currentEdge;

	std::vector<double> scale_vector;
	
};
#endif
/** @} */
