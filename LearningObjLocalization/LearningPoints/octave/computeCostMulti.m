function J = computeCostMulti(X, y, theta)
##COMPUTECOSTMULTI Compute cost for linear regression with multiple variables
##   J = COMPUTECOSTMULTI(X, y, theta) computes the cost of using theta as the
##   parameter for linear regression to fit the data points in X and y

  J = sum ( sum ((X*theta - y).^2)/(2*rows (y)) .^2 );
#  J = sum ((X*theta - y).^2)/(2*rows (y));

endfunction
