#include "demothreads.h"

void MapThread::run()
{
	if (exists("table_map.save")) {
		std::cout << "loading Map" << std::endl;
		load_points("table_map.save");
	}
	store_points("table_map.save");

	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	bool filter(true);

	if (filter) {
		filter_collisions(d_grasp_finder.simulator());
		store_points("table_map.save");
	}

	size_t n(0);
	bool test(false);
	if (test)
		while (true) {
	//				size_t n(qrand() % d_configuration_points.size());
			std::cout << "Testing!: "<< n << std::endl;
	//				n = 279;
			std::vector<double> random_pose = d_configuration_points[n];//d_grasp_finder.simulator().home_pos();
			for (size_t i(0); i < random_pose.size(); ++i)
				std::cout << random_pose[i] << " ";
			std::cout << std::endl;
			d_grasp_finder.simulator().set_motors(random_pose);
			double n_collisions = d_grasp_finder.simulator().computePose();
			std::cout << n_collisions << std::endl;
			usleep(500000);
			n = (n + 1) % d_configuration_points.size();
		}
	else
		while (true) {
			d_grasp_finder.find_pose(2000000, 0., 1.0e-6, .20, 200);
			add_best_pose();
			store_points("table_map.save");
		}
}
