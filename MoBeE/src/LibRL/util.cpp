/*#include "util.h"

std::vector<Point_3> tableSample( double xMin, double xMax, double yMin, double yMax, double z, double delta )
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

std::vector<Point_d> gridSample( int dim, int num, double scaling )
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
}*/