/* LinFilter.h
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-10
 * Class declaration file for the LinFilter class used in the DisparityMapper class to filter
 * images produced in the neighborhood step with a simple linear filter.
 */
#pragma once

#include "Filter.h"

class LinFilter : public Filter { 
private:
// variables
	// the weight for the multiplication of this linear filter
	double m_filterWeight;

public:
	// constructor & destructor
	LinFilter() : Filter() {};
	LinFilter(DisparityMapper *parent) : Filter(parent) {};
	~LinFilter() {};

	// set the paramter
	void setParameter(double in) { m_filterWeight = in; printf("[LinFilter] filter weight now: %f\n", m_filterWeight); };

	// apply the filter - main function
	virtual void applyFilter(int i);
};