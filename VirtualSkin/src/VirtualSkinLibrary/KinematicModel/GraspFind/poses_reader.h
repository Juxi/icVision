#ifndef __POSES_READER__
#define __POSES_READER__

#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>

#include <iostream>

typedef std::vector<std::vector<float> > poses_vector_t;
typedef std::map<std::string, poses_vector_t> poses_map_t;

void write_poses(poses_vector_t &vectors, std::string filename) {
	std::ofstream out_file(filename.c_str());
	out_file << "CFGSPACE" << endl;
	for (size_t i(0); i < vectors.size(); ++i) {
		std::vector<float> &vec(vectors[i]);
		for (size_t n(0); n < vec.size(); ++n)
			out_file << vec[n] << " ";
		out_file << endl;
	}
}

poses_map_t read_poses(std::string filename) {
	std::ifstream in_file(filename.c_str());

	poses_map_t the_map;
	poses_vector_t the_poses;

	std::string current_name;
	while (true) {
		std::string line;
		getline(in_file, line);
		if (!in_file)
			break;

		if (std::isalpha(line[0])) {
			if (current_name.size())
				the_map[current_name] = the_poses;
			std::remove(line.begin(), line.end(), ' ');
			current_name = line;
		} else {
			std::istringstream line_reader(line);
			std::vector<float> pose;
			while (true) {
				float number;
				line_reader >> number;
				if (!line_reader)
					break;
				pose.push_back(number);
			}
			the_poses.push_back(pose);
		}
	}
	the_map[current_name] = the_poses;
	return the_map;
}

#endif
