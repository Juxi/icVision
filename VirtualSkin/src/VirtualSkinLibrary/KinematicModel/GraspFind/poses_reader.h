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

typedef std::vector<std::vector<double> > poses_vector_t;
typedef std::map<std::string, poses_vector_t> poses_map_t;

inline void write_poses(poses_map_t &poses_map, std::string filename) {
	std::ofstream out_file(filename.c_str());

	poses_map_t::iterator it(poses_map.begin());
	for (; it != poses_map.end(); ++it) {
		out_file << it->first << std::endl;
		poses_vector_t &vectors(it->second);
		for (size_t i(0); i < vectors.size(); ++i) {
			std::vector<double> &vec(vectors[i]);
			for (size_t n(0); n < vec.size(); ++n)
				out_file << vec[n] << " ";
			out_file << std::endl;
		}
	}
}

class MapError : public std::exception {
	std::string d_msg;
public:
	MapError(std::string str) : d_msg(str) {}
	~MapError() throw(){}
	const char* what() const throw() {return d_msg.c_str();}

};

inline bool check_map(poses_map_t &the_map) {
	if (the_map.size() == 0)
		throw MapError("Empty map");

	poses_map_t::iterator it(the_map.begin());

	size_t n(it->second.size());
	for (; it != the_map.end(); ++it)
		if (it->second.size() != n)
			throw MapError("Number of vectors inconsistent");

	for (it = the_map.begin(); it != the_map.end(); ++it) {
		poses_vector_t &vector(it->second);
		if (vector.size() == 0)
			throw MapError("0 size vector");
		size_t n_vec(vector[0].size());
		for (size_t i(0); i < vector.size(); ++i)
			if (vector[i].size() != n_vec)
				throw MapError("Vector not of correct size");
	}


}

inline poses_map_t read_poses(std::string filename) {
	std::ifstream in_file(filename.c_str());

	if (!in_file) {
		std::cerr << "No File Found: " << filename << std::endl;
		return poses_map_t();
	}

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
			the_poses.clear();
		} else {
			std::istringstream line_reader(line);
			std::vector<double> pose;
			while (true) {
				float number;
				line_reader >> number;
				if (!line_reader)
					break;
				pose.push_back(number);
			}
			if (!pose.size())
				continue;
			the_poses.push_back(pose);
		}
	}
	the_map[current_name] = the_poses;
	check_map(the_map);

	return the_map;
}

#endif
