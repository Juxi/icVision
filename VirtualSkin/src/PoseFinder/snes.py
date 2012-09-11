__author__ = 'Tom Schaul, tom@idsia.ch'

from scipy import dot, exp, log, sqrt, floor, ones, randn, zeros_like, Inf, argmax


def SNES(f, x0, maxEvals=1e6, verbose=False, targetFitness=-1e-10):
    """ Separable NES, as described in Schaul, Glasmachers and Schmidhuber (GECCO'11).
    Maximizes a function f. 
    Returns (best solution found, corresponding fitness) """
    dim = len(x0)  
    learningRate = 0.2 * (3 + log(dim)) / sqrt(dim)
    batchSize = 4 + int(floor(3 * log(dim)))    
    center = x0.copy()
    sigmas = ones(dim)
    numEvals = 0
    bestFound = None
    bestFitness = -Inf
    while numEvals + batchSize <= maxEvals and bestFitness < targetFitness:
        # produce and evaluate samples
        samples = [randn(dim) for _ in range(batchSize)]
        fitnesses = [f(sigmas * s + center) for s in samples]
        if max(fitnesses) > bestFitness:
            bestFitness = max(fitnesses)
            bestFound = samples[argmax(fitnesses)]
        numEvals += batchSize 
        if verbose: print "Step", numEvals/batchSize, ":", max(fitnesses), "best:", bestFitness
        
        # update center and variances
        utilities = computeUtilities(fitnesses)
        center += sigmas * dot(utilities, samples) 
        covGradient = dot(utilities, [s ** 2 - 1 for s in samples])        
        sigmas = sigmas * exp(0.5 * learningRate * covGradient)            

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
    
    # 100-dimensional ellipsoid function
    dim = 100
    A = array([power(1000, 2 * i / (dim - 1.)) for i in range(dim)])
    def elli(x):
        return -dot(A * x, x)
    
    # example run
    print SNES(elli, ones(dim), verbose=True)

