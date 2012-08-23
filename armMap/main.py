#!/usr/bin/python

import iCubInteractor as interactor

import numpy as np
import time
import pylab
import matplotlib
import local_linear_map as llm
import explorer
import cPickle
import sys
import eyes
import grab_mouse
import yarp

from matplotlib.pyplot import plot, draw, show, figure, ion
from mpl_toolkits.mplot3d import Axes3D
from multiprocessing import Process

global an_environment
an_environment = interactor.iCubEnvironment("/icubSimF")

KEY_HEAD      =  6
KEY_TORSO     =  7
KEY_LEFT_ARM  =  8
KEY_RIGHT_ARM =  9

keys = [KEY_HEAD, KEY_TORSO, KEY_LEFT_ARM, KEY_RIGHT_ARM]

initSequence1 = dict({KEY_HEAD:np.array([0, 0, 0, 0, 0, 0]),
                  KEY_RIGHT_ARM:np.array([-60, 80, 0, 45, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0]),
                  KEY_LEFT_ARM:np.array([-60, 80, 0, 45, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0]),
                  KEY_TORSO:np.array([0, 0, 0])})

initSequence2 = dict({KEY_HEAD:np.array([-35, 0, -17, -12, 0, 0]),
                  KEY_RIGHT_ARM:np.array([-60, 50, 17, 49, -35, 0, -12, 15, 0, 0, 0, 0, 0, 0, 0, 0]),
                  KEY_LEFT_ARM:np.array([-60, 80, 0, 45, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0]),
                  KEY_TORSO:np.array([0, 0, 30])})

#home_q = an_environment._numpyArray2DVector([-55, 30, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
#home_q = an_environment._numpyArray2DVector([-55, 30, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
#home_q = an_environment._numpyArray2DVector([-55, 10, 10, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
home_q = an_environment._numpyArray2DVector(initSequence2[KEY_RIGHT_ARM])

move_wait = .1
orientation_factor = .3

USE_TORSO = False
USE_ARM = True
USE_FINGERS = False

HOME_TORSO = True
HOME_HEAD = True
HOME_LEFT_ARM = True


#low_limit_x = [-.35, .0, .03]
#high_limit_x = [-.26, .22, .05]
#low_limit_x = [-.35, .0, -.02]
#high_limit_x = [-.19, .30, .05]
high_limit_x = [-.2, .3, .1]
low_limit_x = [-.4, .1, 0]
    
d = .05 #in simulation meter

sigma = .1 #std perturbation in q-space
small_sigma = .01 # random walk parameter

home_chance = .1 # chance to go home

def start_up():
    print "first init"
    for key in keys:
        an_environment.iPosCtrls[key].positionMove(an_environment._numpyArray2DVector(initSequence1[key]))
    for key in keys:
        an_environment._waitMotionDone2(key, an_environment._numpyArray2DVector(initSequence1[key]))
    
    print "second init"
    for key in keys:
        an_environment.iPosCtrls[key].positionMove(an_environment._numpyArray2DVector(initSequence2[key]))
    for key in keys:
        an_environment._waitMotionDone2(key, an_environment._numpyArray2DVector(initSequence2[key]))
    
    
    

def check_limit(point):
    for i in range(len(point)):
        if point[i] < low_limit_x[i] or point[i] > high_limit_x[i]:
            return False
    return True

def move(q, do_wait = False):
    #Right arm
    if not USE_FINGERS:
        for i in range(7,len(q)):
            q[i] = 0
        q[7] = 15

    if USE_ARM:
        an_environment.iPosCtrls[KEY_RIGHT_ARM].positionMove(an_environment._numpyArray2DVector(q[:16]))

    #head
    
    #an_environment.iPosCtrls[an_environment.KEY_HEAD].positionMove(an_environment._numpyArray2DVector([0., 0., -10., 0., 0., 0.]))
    
    #Torso
    if USE_TORSO:
        an_environment.iPosCtrls[KEY_TORSO].positionMove(an_environment._numpyArray2DVector(q[16:]))
    if HOME_TORSO:
        an_environment.iPosCtrls[KEY_TORSO].positionMove(an_environment._numpyArray2DVector(initSequence2[KEY_TORSO]))
    if HOME_HEAD:
        an_environment.iPosCtrls[KEY_HEAD].positionMove(an_environment._numpyArray2DVector(initSequence2[KEY_HEAD]))
    
    #left arm high
    if HOME_LEFT_ARM:
        an_environment.iPosCtrls[KEY_LEFT_ARM].positionMove(an_environment._numpyArray2DVector(initSequence2[KEY_LEFT_ARM]))
    
    time.sleep(move_wait)
    if do_wait:
        if USE_ARM:
            an_environment._waitMotionDone2(KEY_RIGHT_ARM, an_environment._numpyArray2DVector(q[:16]))

        if USE_TORSO:
            an_environment._waitMotionDone2(KEY_TORSO, an_environment._numpyArray2DVector(q[16:]))
        if HOME_TORSO:
            an_environment._waitMotionDone2(KEY_TORSO, an_environment._numpyArray2DVector(initSequence2[KEY_TORSO]))
        if HOME_HEAD:
            an_environment._waitMotionDone2(KEY_HEAD, an_environment._numpyArray2DVector(initSequence2[KEY_HEAD]))
        if HOME_LEFT_ARM:
            an_environment._waitMotionDone2(KEY_LEFT_ARM, an_environment._numpyArray2DVector(initSequence2[KEY_LEFT_ARM]))
        
def home():
    move(home_q, do_wait = True)                                                     
                                                     
def get_X():
    
    hand_pos = None
    while hand_pos == None:
        hand_pos = an_environment.getHandPos()
        hand_orientation = an_environment.getHandOrientation()
        print hand_pos
    #together = np.append(hand_pos, hand_orientation * orientation_factor)
    return llm.vec(hand_pos)

start_up()
home()
time.sleep(2.)
home_x = get_X()

if not check_limit(home_x):
    print home_x
    print low_limit_x
    print high_limit_x
    raise "Home position not within limits"

n_x = len(home_x)
n_q = len(home_q)


global explore
explore = explorer.Explorer(n_x, n_q, d, sigma, small_sigma, home_q, home_chance, low_limit_x, high_limit_x, orientation_factor)
print "home_x", home_x
explore.init_map(home_x, home_q)

if len(sys.argv) >= 2:
    filename = sys.argv[1]
    f = open(filename,'r')
    f.close()
    explore.map = llm.load_linear_map(filename)
    explore.map.set_sigma(sigma, small_sigma)
    explore.map.set_d(d)
    print "LOADED MAP"
    time.sleep(2.)


#starting up experiment
n_steps = 20
alpha = .2

start_goal_pos, go_home = explore.home_x, False

start_q, _, _ = explore.map.activate(start_goal_pos)
move(start_q)
start_real_pos = get_X()

previous_real_pos = start_real_pos.copy()
previous_goal_pos = start_real_pos.copy()
previous_q = start_q.copy()

positions = []

icub_eyes = eyes.iCub_Eyes()

a_timer = time.time()


def train_controller():
    global previous_real_pos
    global previous_goal_pos
    global previous_q
    global a_timer                
    it_collided = False

    while(True):
        if time.time() - a_timer > 60.:
            print "SAVING"
            time.sleep(1.)
            explore.map.save("map.save")
            a_timer = time.time()
            
        start_goal_pos, go_home = explore.choose_new_point()
        start_q = previous_q #used in case of home
    
        for step in range(n_steps):
            fraction = 1. * step / n_steps
            print "step: %s, fraction: %s" % (step, fraction)
        
        
            new_goal_pos = explore.step_func(fraction)
 
            new_q, _, _ = explore.map.activate(new_goal_pos)
            #explore_deviation = explore.get_deviation(new_goal_pos)
            #new_q = np.add(new_q, explore_deviation)
            if go_home:
                new_q = (explore.home_q - start_q) * fraction + start_q
                print new_q
            #print explore_deviation.transpose()

            #Moving ICUB
            move(new_q)
            new_real_pos = get_X()
            
            if an_environment.get_collision():
                it_collided = True
                print "bumping"
                reflex_wait = .5
                while an_environment.get_collision():
                    time.sleep(reflex_wait)
                previous_real_pos = get_X()
                previous_q, _, _ = explore.map.activate(new_goal_pos)
                explore.goal_pos = home_x#previous_real_pos
                print "breaking"
                break
                    
            
            #Updating
            #print previous_real_pos, new_real_pos, previous_goal_pos, new_goal_pos, previous_q, new_q
            tha_weight = explore.calculate_weight(previous_real_pos, new_real_pos, previous_goal_pos, new_goal_pos, previous_q, new_q, an_environment)
            print "tha weight", tha_weight
            print "goal pos", explore.start_pos.transpose(), new_goal_pos.transpose(), new_real_pos.transpose(), (new_goal_pos - new_real_pos).transpose()
            
            reference_q, _, _ = explore.map.activate(new_real_pos)

            explore.map.activate_and_train(new_real_pos, new_q, tha_weight * alpha)
            #explore.map.activate_and_train(new_goal_pos, reference_q, tha_weight * alpha)
            
            previous_real_pos = new_real_pos
            previous_goal_pos = new_goal_pos
            previous_q = new_q

            #saving images

        
        print "map contains %s points" % len(explore.map.points)
        print "it collided:", it_collided
        if go_home:
            print "homing HOMEY"
            home()

def move_for_sf():
    vr = .02
    m = .75
    #vr = .6
    #m =.8

    n_steps = 40000
    
    global previous_real_pos
    global previous_goal_pos
    global previous_q
    global a_timer

    global low_limit_x, high_limit_x
    #[-0.384979  0.213882  0.06758 ]

    outfile_q, outfile_x = open('q.data', 'w+'), open('x.data', 'w+')
    outfile_q_dot, outfile_x_dot = open('q_dot.data', 'w+'), open('x_dot.data', 'w+')
    
    #high_limit_x = [.01, .87, .35]
    #low_limit_x = [-.08, .72, .25]

    #high_limit_x = [-.2, .3, .1]
    #low_limit_x = [-.4, .1, 0]
    print "ESTIMATE:"
    low_limit_x, high_limit_x = explore.map.estimate_limits()
    print "DONE"
    
    positions = []
    
    previous_speed = llm.vec([0] * len(start_real_pos))
    total_steps = 0
    
    for step in range(n_steps):
        print(step)
        start_q = previous_q #used in case of home

        while True:
            new_goal_pos = np.add(np.add(previous_goal_pos, previous_speed * m), llm.vec(np.random.normal([0] * len(previous_goal_pos), vr)) * (1.0 - m))
            print low_limit_x, high_limit_x
            print new_goal_pos
            if check_limit(new_goal_pos):
                break
            previous_speed *= .5
        print("bla")
        new_q, _, _ = explore.map.activate(new_goal_pos)
        print("bla")
        move(new_q)

        new_real_pos = llm.vec(an_environment.getHandPos())
        positions.append([float(i) for i in new_real_pos])

        #print(new_q - previous_q, new_goal_pos - previous_goal_pos, new_real_pos - previous_real_pos)
        print " ".join([str(float(i)) for i in new_q - previous_q])

        write_str = " ".join([str(float(i)) for i in new_q - previous_q])
        outfile_q_dot.write(write_str)
        outfile_q_dot.write("\n")
        write_str = " ".join([str(float(i)) for i in new_real_pos - previous_real_pos])
        outfile_x_dot.write(write_str)
        outfile_x_dot.write("\n")

        write_str = " ".join([str(float(i)) for i in new_q])
        outfile_q.write(write_str)
        outfile_q.write("\n")
        write_str = " ".join([str(float(i)) for i in new_real_pos])
        outfile_x.write(write_str)
        outfile_x.write("\n")

        previous_speed = new_goal_pos - previous_goal_pos
        print "previous speed", previous_speed
        print "home", new_real_pos.transpose()
        
        previous_real_pos = new_real_pos
        previous_goal_pos = new_goal_pos


        previous_q = new_q

        icub_eyes.grabImages()
        icub_eyes.saveImages(save_dir = "/tmp/dataset/", time_stamp = total_steps, right_image = 0)
        total_steps += 1

    print "saving positions"
    cPickle.dump(positions, open("tmp/positions.pickle", "w+"))

def move_with_mouse():
    global previous_real_pos
    global previous_goal_pos
    global previous_q
    global a_timer

    global low_limit_x, high_limit_x

    explore.set_sigma(0.0001, 0.0001)
    #low_limit_x = [-.17, .72, .25] #orientation was divided by 10
    #high_limit_x = [.01, .87, .35]
    #low_limit_x = [-.08, .72, .25]

    positions = []
    
    previous_speed = llm.vec([0] * len(start_real_pos))
    total_steps = 0

    while True:
        time.sleep(.1)
        new_goal_pos = home_x.copy()
        (x, y) = grab_mouse.mouse_pos()
        x /= 2000.
        y /= 1000.
        new_goal_pos[0] = y * (high_limit_x[0] - low_limit_x[0]) + low_limit_x[0]
        new_goal_pos[1] = x * (high_limit_x[1] - low_limit_x[1]) + low_limit_x[1]
        new_goal_pos[2] = home_x[2]#y * (high_limit_x[2] - low_limit_x[2]) + low_limit_x[2] #home_x[2]
        print new_goal_pos.transpose()
        if not check_limit(new_goal_pos):
            continue
        new_q, _, _ = explore.map.activate(new_goal_pos)
        #print new_goal_pos, new_q
        move(new_q)
        print an_environment.getHandPos()
        
        #icub_eyes.grabImages()
        #icub_eyes.saveImages(save_dir = "/tmp/dataset/", time_stamp = total_steps, right_image = 0)
        #total_steps += 1

def move_infinite():
    global previous_real_pos
    global previous_goal_pos
    global previous_q
    global a_timer

    global low_limit_x, high_limit_x

    explore.set_sigma(0.0001, 0.0001)
    #low_limit_x = [-.17, .72, .25] #orientation was divided by 10
    #high_limit_x = [.01, .87, .35]
    #low_limit_x = [-.08, .72, .25]

    positions = []
    
    previous_speed = llm.vec([0] * len(start_real_pos))
    total_steps = 0

    angle1, angle2 = 0., 0.
    distance1, distance2 = .14, .08
    
    while True:
        time.sleep(.05)
        new_goal_pos = home_x.copy()

        if 1:
            new_goal_pos[0] -= np.sin(angle1) * distance1
            #new_goal_pos[2] -= .18
            new_goal_pos[2] -= np.sin(angle2) * distance2
        if 0:
            new_goal_pos[0] -= np.sin(angle1) * distance1
            new_goal_pos[1] -= .18
            new_goal_pos[2] -= np.sin(angle2) * distance2 - .07
            
        new_q, _, _ = explore.map.activate(new_goal_pos)

        move(new_q)
        angle1 += .05
        angle2 += .1
    
def print_pos():
    global a_timer

    counter = 0
    while(True):
        if time.time() - a_timer > 1:
            result = yarp.Bottle()
            print "reading"
            #print an_environment.get_collision()
            a_timer = time.time()

            print an_environment.getHandPos().transpose()
            print an_environment.get_hand_orientation()
            print an_environment.get_observations()
            print an_environment.get_hand_orientation_measure()
            counter += 1

#train_controller()
move_for_sf()

#move_with_mouse()

#print_pos()
