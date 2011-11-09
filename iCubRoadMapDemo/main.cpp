//#include <CGAL/Simple_cartesian.h>
#include <CGAL/Cartesian_d.h>
//#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
//#include <CGAL/K_neighbor_search.h>
#include <CGAL/Search_traits_d.h>

#include <list>
#include <cmath>
#include <vector>

#include "partController.h"


typedef CGAL::Cartesian_d<double> K;
typedef K::Point_d Point_d;
typedef CGAL::Search_traits_d<K> TreeTraits;
typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
typedef Neighbor_search::Tree Tree;

int main()
{
	PartController left_arm;
	left_arm.open( "icubSim", "left_arm" );
	
	// leave the hand still when getting random configurations
	left_arm.setJointMask( 7, false );
	left_arm.setJointMask( 8, false );
	left_arm.setJointMask( 9, false );
	left_arm.setJointMask( 10, false );
	left_arm.setJointMask( 11, false );
	left_arm.setJointMask( 12, false );
	left_arm.setJointMask( 13, false );
	left_arm.setJointMask( 14, false );
	left_arm.setJointMask( 15, false );
	
	std::vector<double> q = left_arm.getRandomPose();
	for ( std::vector<double>::iterator i = q.begin(); i != q.end(); ++i )
	{ 
		if ( i == q.begin() ) printf("q: ");
		else printf("   ");
		printf("%f\n", *i);
	}
	printf("\n");
	
	/******************************************************************/

	const unsigned int N = 3;
	
	int d = 2;
	
	std::vector<double> o;
	for ( int i=0; i<d; i++ ) { o.push_back( (i+1)*0.0 ); }
	Point_d O = Point_d( d , o.begin(), o.end() );
	
	std::vector<double> a;
	for ( int i=0; i<d; i++ ) { a.push_back( (i+1)*1.0 ); }
	Point_d A = Point_d( d , a.begin(), a.end() );
	
	std::vector<double> b;
	for ( int i=0; i<d; i++ ) { b.push_back( (i+1)*1.5 ); }
	Point_d B = Point_d( d , b.begin(), b.end() );
	
	std::vector<double> c;
	for ( int i=0; i<d; i++ ) { c.push_back( (i+1)*2 ); }
	Point_d C = Point_d( d , c.begin(), c.end() );
	
	C.first();
	
	std::list<Point_d> points;
	points.push_back( A );
	points.push_back( B );
	points.push_back( C );
	
	Tree tree(points.begin(), points.end());
	
	Point_d query(O);
	
	// Initialize the search structure, and search all N points
	
	Neighbor_search search(tree, query, N);
	
	// report the N nearest neighbors and their distance
	// This should sort all N points by increasing distance from origin
	for(Neighbor_search::iterator it = search.begin(); it != search.end(); ++it)
	{
		std::cout << it->first << " "<< std::sqrt(it->second) << std::endl;
		/*for ( std::vector<double>::iterator j = q.begin(); i != q.end(); ++i )
		{ 
			if ( i == q.begin() ) printf("q: ");
			else printf("   ");
			printf("%f\n", *i);
		}
		printf("\n");*/
	}
	
	
		
	left_arm.close();
	return 0;
}
