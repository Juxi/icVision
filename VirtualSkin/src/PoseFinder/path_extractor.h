#ifndef __PATH_EXTRACTOR_H__
#define __PATH_EXTRACTOR_H__

#include "roadmap.h"
#include <vector>
#include <iostream>

#include <CGAL/Homogeneous_d.h>
//#include <CGAL/leda_integer.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Delaunay_d.h>

/*
inline std::vector<std::vector<double> > extract_path(std::vector<std::vector<double> > configurations, std::vector<double> fitnesses, std::vector<int> collisions, std::vector<double> start, std::vector<double> goal) {
	typedef CGAL::Gmpz RT;
	typedef CGAL::Homogeneous_d<RT> Kernel;
	typedef CGAL::Delaunay_d<Kernel> Delaunay_d;
	typedef Delaunay_d::Point_d Point;
//	typedef Delaunay_d::Simplex_handle Simplex_handle;
//	typedef Delaunay_d::Vertex_handle Vertex_handle;
//	typedef Delaunay_d::Simplex_iterator simplex_it;



	int dimensionality(configurations[0].size());
//	Delaunay_d datastructure(dimensionality);

//	for (simplex_it it = datastructure.simplices_begin(); it != datastructure.simplices_end(); ++it) {
//		Delaunay_d::Simplex_handle::Point_const_iterator start = it->points_begin();
//	}

	std::vector<std::vector<double> > path;

	Roadmap road_map;
	road_map.setDimensionality(dimensionality);

	std::cout << "adding points:" << std::endl;
	time_t t(time(0));
	road_map.insert(std::vector<double>(), start);
	for (size_t i(0); i < configurations.size(); ++i) {
		Point p(dimensionality, configurations[i].begin(), configurations[i].end(), 0);
		//datastructure.insert(p);
//		if (fitnesses[i] < 1.0)
		road_map.insert(std::vector<double>(), configurations[i], fitnesses[i], collisions[i]);
	}

	std::cout << "took: " << time(0) - t << endl;
	std::cout << "n vertices: " << road_map.size() << std::endl;
	road_map.insert(std::vector<double>(), goal);
	t = time(0);
	std::cout << "connecting points:" << std::endl;
	road_map.graphConnect(50);
	std::cout << "took: " << time(0) - t << endl;
	std::cout << "finding path:" << std::endl;
	Roadmap::PathList the_path_nodes = road_map.shortestPath(start, goal);
	std::vector<std::vector<double> > the_path = road_map.vertex_list_to_q(the_path_nodes);
	return the_path;

*/


#endif
