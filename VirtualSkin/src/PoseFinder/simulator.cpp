#include "simulator.h"

std::vector<double> Simulator::normal_to_real_motors(std::vector<double> normal_values) {
	std::vector<double> real_values(normal_values.size());
	size_t i(0);
	int n_parts = d_robot.numBodyParts();
	for (size_t p(0); p < n_parts; ++p) {
		int n_motors = d_robot.getNumMotors(p);
		for (size_t m(0); m < n_motors; ++m, ++i) {
			if (i >= normal_values.size()) {
				throw StringException("normal to real conversion exceeds size of vector");
			}
			real_values[i] = d_robot.NormalToEncoderPosition(p, m, normal_values[i]);
		}
	}
	return real_values;
}

std::vector<double> Simulator::real_to_normal_motors(std::vector<double> real_values) {
	std::vector<double> normal_values(real_values.size());
	size_t i(0);
	int n_parts = d_robot.numBodyParts();
	for (size_t p(0); p < n_parts; ++p) {
		int n_motors = d_robot.getNumMotors(p);
		for (size_t m(0); m < n_motors; ++m, ++i) {
			if (i >= real_values.size()) {
				throw StringException("real to normal conversion exceeds size of vector");
			}
			normal_values[i] = d_robot.EncoderToNormalPosition(p, m, real_values[i]);
		}
	}
	return normal_values;
}
