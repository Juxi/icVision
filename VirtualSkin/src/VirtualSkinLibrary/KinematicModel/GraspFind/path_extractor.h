#ifndef __PATH_EXTRACTOR_H__
#define __PATH_EXTRACTOR_H__

#include "roadmap.h"
#include <vector>
#include <iostream>

std::vector<std::vector<double> > extract_path(std::vector<std::vector<double> > configurations, std::vector<double> fitnesses, std::vector<double> start, std::vector<double> goal) {
	std::vector<std::vector<double> > path;

	Roadmap road_map;
	road_map.setDimensionality(configurations[0].size());

	road_map.insert(0, 0, start);

	for (size_t i(0); i < configurations.size(); ++i)
//		if (fitnesses[i] < 1.0)
			road_map.insert(0, 0, configurations[i]);

	std::cout << "n vertices: " << road_map.size() << std::endl;
	road_map.insert(0, 0, goal);
	road_map.graphConnect(10);

	std::vector<std::vector<double> > the_path = road_map.shortestPath(start, goal);
	return the_path;
}

#endif
