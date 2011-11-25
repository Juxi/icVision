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

#include "iCubController.h"

/** \brief foo
 */
class Roadmap
{

private:

	/* FIRST, THE BOOST GRAPH */
	struct Vertex { 
		char* type;							//just for debugging
		double proj[2];						// for visualization
		std::vector<double> q;				// robot configuration
		Vertex():type("") { proj[0]=0; proj[1]=0; }
	};
	
	struct Edge {
		double length;
		double successRate;
		Edge():length(1.0),successRate(1.0){}
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
	
	typedef CGAL::Cartesian_d<double> K;
	class Pose : public K::Point_d {
		public:
			Pose (	int d, 
					std::vector<double>::iterator first,
					std::vector<double>::iterator last,
					Map::vertex_descriptor _boostGraphVertex			) :  K::Point_d(d,first,last), vertex(_boostGraphVertex) {}
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
	};
	
	typedef CGAL::Orthogonal_k_neighbor_search<Custom_search_traits> K_neighbor_search;
	typedef K_neighbor_search::Tree Tree;
	
	/*****************************/
	
	Map::vertex_descriptor nearestVertex( std::vector<double>, char* type="" );
	//bool nearestVertex( std::vector<double>&, std::vector<double>, char* type="" );
	
	void graphConnect( Pose, unsigned int n = 3 );
	
	std::vector<double> toStdVector( Pose ); 
	
	std::list<Map::vertex_descriptor> shortestPath( Map::vertex_descriptor from, Map::vertex_descriptor to );
	
	Map				map;
	Tree			tree;
	
	Map::vertex_descriptor currentVertex;
	unsigned int dim;
	
public:
	
	iCubController	iCub;
	
	typedef Map::vertex_descriptor vertex_t;
	typedef Map::vertex_iterator vertex_i;
	typedef Map::edge_descriptor edge_t;
	
	Roadmap( char* iCubName );
	~Roadmap();
	
	void buildRandomMap( unsigned int numVertices, unsigned int numNeighbors );
	void insert( std::vector<double> _q, unsigned int n );
	
	bool gotoNearest();
	bool isOnMap();
	bool motionCompleted();	// wait until the motion is done or we are cut off from the robot
	
	bool randomMove();
	
	//bool insert( std::vector< std::vector<double> > );
	
	
	
};
#endif
/** @} */
