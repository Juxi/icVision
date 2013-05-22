/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <CGAL/Cartesian.h>
#include <CGAL/Cartesian_d.h>
#include <CGAL/Point_3.h>
#include <CGAL/Vector_3.h>
#include <CGAL/point_generators_d.h>

/******************
 *** CGAL Types ***
 *****************/

typedef CGAL::Cartesian_d<double>::Point_d Point_d;
typedef CGAL::Cartesian_d<double>::Vector_d Vector_d;
typedef CGAL::Point_3< CGAL::Cartesian<double> > Point_3;
typedef CGAL::Vector_3< CGAL::Cartesian<double> > Vector_3;
typedef CGAL::Creator_uniform_d<std::vector<double>::iterator,Point_d>  Creator_d;

//typedef std::vector<std::vector<double> >   VectorVector_t;
//typedef std::map<std::string, VectorVector_t> VectorMap_t;
//typedef std::vector<Point_d>                PoseVector_t;
//typedef std::map<std::string, PoseVector_t> PoseMap_t;

static std::vector<Point_3> tableSample( double xMin, double xMax, double yMin, double yMax, double z, double delta )
{
    // table height: z = -0.075
    std::vector<Point_3> result;
    for ( double x = xMin; x<=xMax; x+=delta ) {
        for ( double y = yMin; y<=yMax; y+=delta ) {
            result.push_back(Point_3(x,y,z));
        }
    }
    return result;
}

static std::vector<Point_d> gridSample( int dim, int num, double scaling )
{
    // Sample num grid points from dim dimensional space in the cube between min and max
    std::vector<Point_d> points;
    points.reserve(num);
    
    
    CGAL::points_on_cube_grid_d( dim, scaling*0.5, (std::size_t)num, std::back_inserter(points), Creator_d(dim));
    
    std::cout << "Using Samples: \n" <<std::endl;
    
    // shift the sample set to the interval [0,1]
    std::vector<double> v;
    for (int i=0; i<dim; i++) v.push_back(0.5);
    Vector_d shift(dim,v.begin(),v.end());
    for ( std::vector<Point_d>::iterator i=points.begin(); i!=points.end(); ++i ){
        *i += shift;
        std::cout << *i << std::endl;
    }
    
    return points;
}

#endif
/** @} */
