#!/usr/bin/python

from iCubInteractor import *
import numpy as np
import time
import pylab
import matplotlib

environment = iCubEnvironment("/icubSim")

a = []

def home():
    global a
    a = np.array([-95, 30, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], np.float32)
    environment.iPosCtrls[9].positionMove(environment._numpyArray2DVector(a))

def test(sigma, wait):
    global a
    values = []
    for i in range(100):
#        a += np.random.normal(0, sigma, len(a))
        a[0] += 3
        a[1] += 3
        a[2] += 3
        #print a
        environment.iPosCtrls[9].positionMove(environment._numpyArray2DVector(a))
        environment.getHandPos()
        time.sleep(wait)
        values.append(environment.getHandPos())
    
    pylab.plot(values)
    pylab.show()



home()
test(sigma = 1, wait = .1)
