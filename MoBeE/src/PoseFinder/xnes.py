__author__ = 'Tom Schaul, tom@idsia.ch'

from scipy import dot, exp, log, sqrt, floor, ones, randn, zeros_like, Inf, argmax, eye, outer
from scipy.linalg import expm2


def xNES(f, x0, maxEvals=1e6, verbose=False, targetFitness= -1e-10):
    """ Exponential NES (xNES), as described in 
    Glasmachers, Schaul, Sun, Wierstra and Schmidhuber (GECCO'10).
    Maximizes a function f. 
    Returns (best solution found, corresponding fitness).
    """
    dim = len(x0)  
    I = eye(dim)
    learningRate = 0.6 * (3 + log(dim)) / dim / sqrt(dim)
    batchSize = 4 + int(floor(3 * log(dim)))    
    center = x0.copy()
    A = eye(dim)  # sqrt of the covariance matrix
    numEvals = 0
    bestFound = None
    bestFitness = -Inf
    while numEvals + batchSize <= maxEvals and bestFitness < targetFitness:
        # produce and evaluate samples
        samples = [randn(dim) for _ in range(batchSize)]
        fitnesses = [f(dot(A, s) + center) for s in samples]
        if max(fitnesses) > bestFitness:
            bestFitness = max(fitnesses)
            bestFound = samples[argmax(fitnesses)]
        numEvals += batchSize 
        if verbose: print "Step", numEvals / batchSize, ":", max(fitnesses), "best:", bestFitness
        #print A
        # update center and variances
        utilities = computeUtilities(fitnesses)
        center += dot(A, dot(utilities, samples))
        covGradient = sum([u * (outer(s, s) - I) for (s, u) in zip(samples, utilities)])
        A = dot(A, expm2(0.5 * learningRate * covGradient))                      

    return bestFound, bestFitness
    
    
def computeUtilities(fitnesses):
    L = len(fitnesses)
    ranks = zeros_like(fitnesses)
    l = zip(fitnesses, range(L))
    l.sort()
    for i, (_, j) in enumerate(l):
        ranks[j] = i
    # smooth reshaping
    utilities = array([max(0., x) for x in log(L / 2. + 1.0) - log(L - array(ranks))])
    utilities /= sum(utilities)       # make the utilities sum to 1
    utilities -= 1. / L  # baseline
    return utilities

    
if __name__ == "__main__":
    from scipy import dot, array, power
    
    # Rosenbrock function
    def rosen(x):
        return - sum(100 * (x[:-1] ** 2 - x[1:]) ** 2 + (x[:-1] - 1) ** 2)
    
    # example run (30-dimensional Rosenbrock)
    print xNES(rosen, ones(30), verbose=True)
