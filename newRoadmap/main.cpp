//#include "iCubController.h"
#include "roadmap.h"

#include <QtGui>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>
typedef CGAL::Cartesian_d<double> K;
//typedef K::Point_d Point_d;
class Custom_search_traits {		
public:
	typedef K::Cartesian_const_iterator_d Cartesian_const_iterator_d;
	typedef K::Construct_cartesian_const_iterator_d Construct_cartesian_const_iterator_d;
	typedef K::Point_d Point_d;
	typedef K::FT FT;
};
typedef CGAL::Orthogonal_k_neighbor_search<Custom_search_traits> Neighbor_search;
typedef Neighbor_search::Tree Tree;


//#include "graphwidget.h"
//#include "fileDialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{	
	QApplication app(argc, argv);
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	MainWindow window;
    window.show();
	return app.exec();
}

/*** CGAL K_NEAREST_NEIGHBOR SEARCH WITH POINT_D ***/
/*printf("Using Point_d...\n\n");
 std::list<Custom_search_traits::Point_d> points;
 points.push_back(Custom_search_traits::Point_d(0.5,0.0));
 points.push_back(Custom_search_traits::Point_d(0.0,0.5));
 points.push_back(Custom_search_traits::Point_d(1.0,0.6));
 points.push_back(Custom_search_traits::Point_d(0.0,4.7));
 points.push_back(Custom_search_traits::Point_d(0.0,0.8));
 points.push_back(Custom_search_traits::Point_d(1.2,1.9));
 points.push_back(Custom_search_traits::Point_d(0.5,0.1));
 points.push_back(Custom_search_traits::Point_d(1.0,0.2));
 points.push_back(Custom_search_traits::Point_d(0.0,3.3));
 points.push_back(Custom_search_traits::Point_d(1.5,0.4));
 
 Tree tree(points.begin(), points.end());
 
 printf("tree size: %d\n", tree.size());
 
 Custom_search_traits::Point_d queryPoint(0,0);
 
 int num = 5;
 std::cout << "QUERYING VERTEX: " << queryPoint << std::endl;
 printf("SEARCHING FOR %d NEAREST NEIGHBORS\n", num);
 Neighbor_search search(tree, queryPoint, num);
 
 int count = 0;
 for(Neighbor_search::iterator it = search.begin(); it != search.end(); ++it){
 std::cout << it->first << " "<< std::sqrt(it->second) << std::endl;
 count++;
 }
 
 printf("Found %d neighbors.\n",count);*/



/*** CGAL K_NEAREST_NEIGHBOR SEARCH WITH ROADMAP ***/
/* printf("\n\nUsing Vertex...\n\n");
 Roadmap torso;
 std::vector<Roadmap::vertex_t> Points;
 for ( int count=0; count<100; count++ ) {
 std::vector<qreal> point;
 point.push_back((double)rand()/RAND_MAX*10);
 point.push_back((double)rand()/RAND_MAX*10);
 point.push_back((double)rand()/RAND_MAX*10);
 point.push_back((double)rand()/RAND_MAX*10);
 point.push_back((double)rand()/RAND_MAX*10);
 Roadmap::vertex_t v;
 torso.insert(point, v);
 Points.push_back( v );
 }
 
 Roadmap::vertex_t query = *(Points.begin());
 std::vector<Roadmap::vertex_t> neighbors;
 std::vector<double> distances;
 torso.nearestVertices( query, neighbors, distances, 5 );*/
