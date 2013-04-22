/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef TYPES_H_
#define TYPES_H_

/******************
 *** CGAL Types ***
 *****************/

typedef CGAL::Cartesian_d<double>::Point_d Point_d;
typedef CGAL::Cartesian_d<double>::Vector_d Vector_d;
typedef CGAL::Point_3< CGAL::Cartesian<double> > Point_3;
typedef CGAL::Vector_3< CGAL::Cartesian<double> > Vector_3;
typedef CGAL::Creator_uniform_d<std::vector<double>::iterator,Point_d>  Creator_d;


/********************************************
 ***  Serialized versions of our classes  ***
 *** (which we write in and out of files) ***
 *******************************************/

/*struct Transition_s // state transition actions
{
    int origin;
    int dest;
    double value;
    double reward;
    int num;
};*/

/*struct S_Prime_s    // state transition beliefs
{
    int state;       
    double prob; 
    int num; 
};*/

typedef std::vector<std::vector<double> >   VectorVector_t;
typedef std::map<std::string, VectorVector_t> VectorMap_t;

typedef std::vector<Point_d>                PoseVector_t;
typedef std::map<std::string, PoseVector_t> PoseMap_t;

//typedef std::vector<Transition_s>           Transition_t;
//typedef std::map<std::string, Transition_t> TransitionMap_t;




#endif
/** @} */
