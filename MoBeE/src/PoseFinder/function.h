#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "matrix.h"

////////////////////////////////////////////////////////////
// encapsulation of an objective function


class Function
{
public:
	Function() { }
	virtual ~Function() { }

	virtual double eval(const Matrix& point) = 0;
};

struct WorkspaceFunction {
  WorkspaceFunction(){}
  virtual std::vector<double> get_workspace(Function &fitness_funtion) = 0; //use fitness function object to gather workspace info
};


#endif
