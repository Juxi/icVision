/*#include <CGAL/Homogeneous_d.h>
//#include <CGAL/leda_integer.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Delaunay_d.h>

#include <iostream>

#include "roadmap.h"

using namespace std;

void Roadmap::connect_delaunay()
{
	tree.build();
	tree_x.build();
	tree_qs.build();

	typedef CGAL::Gmpz RT;
	typedef CGAL::Homogeneous_d<float> Kernel;
	typedef CGAL::Delaunay_d<Kernel> Delaunay_d;
	typedef Delaunay_d::Point_d Point;
//	typedef Delaunay_d::Simplex_handle Simplex_handle;
//	typedef Delaunay_d::Vertex_handle Vertex_handle;
	typedef Delaunay_d::Simplex_iterator simplex_it;


	int dimensionality(dim);
	Delaunay_d delaunay(dimensionality);

	vertex_i vi, vi_end;

	boost::tuples::tie(vi, vi_end) = vertices(map);

	for (; vi != vi_end; ++vi) {
	  cout << "first: " << map[*vi].q[0] << endl;
	  Point p(dim, map[*vi].q.begin(), map[*vi].q.end());
	  delaunay.insert(p);
	}

	cout << "points:" << endl;
	for (Delaunay_d::Point_const_iterator it = delaunay.points_begin(); it != delaunay.points_end(); ++it) {
	  
	  cout << *it << endl;
	  //Delaunay_d::out_edge_iterator e, e_end;
	  
	  //int count = 0;
	  //for (boost::tuples::tie(e, e_end) = out_edges( *it, delaunay ); e != e_end; ++e)
	  //;
	}
	
	cout << "simplices:" << endl;
	for (simplex_it it = delaunay.simplices_begin(); it != delaunay.simplices_end(); ++it) {
	  cout << *it << endl;
	}

}*/
