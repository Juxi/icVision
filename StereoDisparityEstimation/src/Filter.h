/* Filter.h
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-10
 * Class declaration file for the Filter (base) class used in the DisparityMapper class to filter
 * images produced in the neighborhood step.
 */
#pragma once

class DisparityMapper;

class Filter {
// variables
protected:
	DisparityMapper *mapper;

public:
	// cosntructor and destructor
	Filter() { mapper = NULL; };
	Filter(DisparityMapper *parent) { mapper = parent; };
	~Filter(void) {};

	// main function for the filter, applies the filter to the mappers filtered neighborhood pictures
	virtual void applyFilter(int i) {};
};