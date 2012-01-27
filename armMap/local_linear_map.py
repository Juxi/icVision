#!/usr/bin/python

#from sympy import *
from numpy import *
import cPickle as serial

input_amplification = 8.

def vec(in_vec):
    return matrix(in_vec).reshape((len(in_vec), 1))
                  
class Deviator:
    def __init__(self, n_x, n_q, sigma, small_sigma):
        self.sigma = sigma
        self.small_sigma = small_sigma
        self.n_x = n_x
        self.n_q = n_q
        self.W = matrix(random.normal(zeros((n_q, n_x)), sigma))
        self.o = matrix(random.normal(zeros((n_q, 1)), sigma))
        self.deminish = self.sigma ** 2 / (self.sigma ** 2 + self.small_sigma ** 2)
    
    def activate(self, x):
        return self.W * x + self.o

    def set_sigma(self, sigma, small_sigma):
        self.sigma = sigma
        self.small_sigma = small_sigma

    def move(self):
        if self.small_sigma != 0.0:
            self.W = add(self.W, matrix(random.normal(zeros((self.n_q, self.n_x)), self.small_sigma)))
            self.o = add(self.o, matrix(random.normal(zeros((self.n_q, 1)), self.small_sigma)))
            self.W *= self.deminish
            self.o *= self.deminish

    
class LinearPoint:
    def __init__(self, W, o, center, d, sigma, small_sigma):
        self.center = center
        self.W = W
        self.o = o
        self.d = d
        self.deviator = Deviator(W.shape[1], W.shape[0], sigma, small_sigma)
            
    def distance(self, x):
        return (x - self.center) / self.d

    def set_sigma(self, sigma, small_sigma):
        self.deviator.set_sigma(sigma, small_sigma)

    def activate(self, x):
        distance = self.distance(x)
        b = self.b(distance)
        g = add(self.g(distance), self.deviator.activate(distance * input_amplification))
        self.deviator.move()
        return b * g, b, distance

    def g(self, x):
        return self.W * x + self.o
        
    def diff_g(self, x, delta):
        return delta * x.transpose(), delta
    
    def grad(self, x, n, delta):
        d = self.distance(x)
        base = 1. / n * self.b(d)
#        print "base", base, n, self.b(d)
        diff_W, diff_o = self.diff_g(d, delta)
        return base * diff_W, base * diff_o
        
    def b(self, x):
        return exp(-linalg.norm(x)**2)

    def adjust(self, dW, do, alpha):
        self.W = add(self.W, dW * float(alpha))
        self.o = add(self.o, do * float(alpha))
        
def create_clean_linear_point(center, q, n_x, n_q, d, sigma, small_sigma):
    W = matrix(zeros((n_q, n_x)))
    o = q
    return LinearPoint(W, o, center, d, sigma, small_sigma)

def create_linear_point(center, W, o, d, sigma, small_sigma):
#    print "ooo",o
    return LinearPoint(W, o, center, d, sigma, small_sigma)


def load_linear_map(filename):
    (points, n_x, n_q, d, sigma, small_sigma) = serial.load(open(filename, "r"))
    llm = LocalLinearMap(n_x, n_q, d, sigma, small_sigma)
    llm.points = points
    return llm

class LocalLinearMap:
    points = []

    def __init__(self, n_x, n_q, d, sigma, small_sigma):
        self.n_x = n_x
        self.n_q = n_q
        self.d = d
        self.sigma = sigma
        self.small_sigma = small_sigma

    def save(self, filename):
        serial.dump((self.points, self.n_x, self.n_q, self.d, self.sigma, self.small_sigma), open(filename, "w+"))

    def activate(self, x, q = None, add_point = False):
        added_new_point = False
        n = 0.0

        if len(self.points) == 0:
            self.set_first_point(x, q)
            added_new_point = True

        total_out = self.output_ident()

        min_distance = 999999999.

        for p in self.points:
            out, b, distance = p.activate(x)
            n += b
            total_out = add(total_out, out)
            min_distance = min(min_distance, linalg.norm(distance))
        
        out = total_out / n
        if add_point and min_distance > 1.0:
            self.add_new_point(x, n, out)
            added_new_point = True
        return out, n, added_new_point

    def estimate_W(self, x, n):
        delta_ident = matrix(ones((self.n_q, 1)))
        grad = matrix(zeros((self.n_q, self.n_x)))
        for p in self.points:
            grad = add(grad, p.W * p.b(p.distance(x)))

        grad /= n
        return grad
    
    def add_new_point(self, x, n, current_guess):
        guess_w = self.estimate_W(x, n)
        self.points.append(create_linear_point(x, guess_w, current_guess, self.d, self.sigma, self.small_sigma))
        print "point:", len(self.points)

    def set_sigma(self, sigma, small_sigma):
        for p in self.points:
            p.set_sigma(sigma, small_sigma)
    
    def set_d(self, d):
        self.d = d
    
    def output_ident(self):
        return matrix(zeros((self.n_q, 1)))
    
    def set_first_point(self, x, q):
        self.points.append(create_clean_linear_point(x, q, self.n_x, self.n_q, self.d, self.sigma, self.small_sigma))
        
    def activate_and_train(self, x, q, alpha):
        out, n, added_new_point = self.activate(x, q, add_point = True)
        if added_new_point:
            return
        
        delta = add(q, -out)
        for p in self.points:
            dW, dO = p.grad(x, n, delta)
            p.adjust(dW, dO * 0.01, alpha)

if __name__ == "__main__":    
    m = LocalLinearMap(2, 2, .2, 0.0001, 0.0001)
    
    for i in range(4000):
        if i % 100 == 0:
            print i
        x1 = random.random()
        x2 = random.random()
        
        y1 = x1 * 2. + 1.
        y2 = x2 * 4. - 1.
        m.activate_and_train(vec([x1, x2]), vec([y1, y2]), .1)
    #for p in m.points:
    #    print p.o, p.W
    
    z = .0
    while True:
        out, _, _ = m.activate(vec([z, .0]))
        print z, out
        z += .05
        if z > .99:
            break
