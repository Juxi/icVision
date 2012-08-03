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

class WorkspaceFunction
{
public:
  WorkspaceFunction() { }
  virtual ~WorkspaceFunction() { }
  
  virtual double eval(const Matrix& point) = 0;
};


#endif
