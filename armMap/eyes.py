import yarp
from opencv import cv
from opencv import highgui as hg
from time import sleep
import sys, scipy, scipy.misc,os
import numpy as np
import mdp
from PyQt4 import QtGui,QtCore
from PyQt4.Qt import QMutex
import cPickle

global robot_name
robot_name = "/icubSim/"    #for simulator
#robot_name = "/icubSimF/"    #for simulator with safety filter

#robot_name = "/icub/"        #for the real robot
#robot_name = "/icubF/"        #for the real robot with safety filter

global robot_name1
robot_name1 = "/icubSim/"    #for simulator
#robot_name1 = "/icub/"        #for the real robot with safety filter

class iCub_Eyes():
  ''' This class takes image/video input from iCub Eyes 
      to carry out Image processing/vision tasks.
  '''
  def __init__(self):
    global robot_name1
    robotname1 = robot_name1

    global robot_name
    robotname = robot_name

    self.eyes = yarp.Port()
    self.leftEye = yarp.BufferedPortImageRgb()
    self.rightEye = yarp.BufferedPortImageRgb()
    self.leftEye.open('/iCub_Control/leftEye')
    self.rightEye.open('/iCub_Control/rightEye')
    self.eyes.open('/iCub_Control/eyes')
    yarp.Network.connect(str(robotname1) +'cam/left','/iCub_Control/leftEye')
    yarp.Network.connect(str(robotname1) + 'cam/right','/iCub_Control/rightEye')
    yarp.Network.connect('/iCub_Control/eyes',str(robotname) + 'head/rpc:i')
    self._init_disp_flag = 0
    self._cnvt2opncvFlag = 0

  def _init_display(self,left_eye=1,right_eye=1):
    if left_eye:
      hg.cvNamedWindow("leftEye",0)
    if right_eye:
      hg.cvNamedWindow("rightEye",0)
    if not left_eye and not right_eye:
      hg.cvNamedWindow("Dummy",0)
    self._init_disp_flag = 1

  def grabImages(self):
    self.image_left = self.leftEye.read()
    self.image_right = self.rightEye.read()
    self._cnvt2opncvFlag = 0
    if (self.image_left == None):
      raise NameError ('Unable to grab the left Image')
    if (self.image_right == None):
      raise NameError ('Unable to grab the right Image')

  def convert2opencv(self):
    _image_leftcv = cv.cvCreateImage(cv.cvSize(self.image_left.width(),
    self.image_left.height()),8,3)
    _image_rightcv = cv.cvCreateImage(cv.cvSize(self.image_right.width(),
    self.image_right.height()),8,3)
    cv.cvCvtColor(self.image_left.getIplImage(), _image_leftcv,cv.CV_RGB2BGR)
    cv.cvCvtColor(self.image_right.getIplImage(), _image_rightcv,cv.CV_RGB2BGR)
    self.image_leftcv = _image_leftcv
    self.image_rightcv = _image_rightcv
    self._cnvt2opncvFlag = 1
    
  def saveImages(self,left_image=1,right_image=1,time_stamp=None,Mode=None,save_dir=None):
    if not self._cnvt2opncvFlag:
      self.convert2opencv()
    if not self._init_disp_flag:
      self._init_display(left_image,right_image)
    if time_stamp is None:
      print('Time stamp not provided, saving images without time stamp')
      time_stamp = 0
    print "in save images"
    if left_image:
      limgfname = str(save_dir) + 'l_' + str(time_stamp) + '.png'
      print "saving left image", limgfname
      hg.cvSaveImage(limgfname,self.image_leftcv)
    if right_image:
      rimgfname = str(save_dir) + 'r_' + str(time_stamp) + '.png'
      print "saving right image", rimgfname
      hg.cvSaveImage(rimgfname,self.image_rightcv)
      
  def display(self,left_image=1,right_image=1):
    if (self._cnvt2opncvFlag != 1):
      self.convert2opencv()
    if (self._init_disp_flag != 1):
      self._init_display(left_image,right_image)
    if left_image:
      hg.cvShowImage("leftEye",self.image_leftcv)
    if right_image:
      hg.cvShowImage("rightEye",self.image_rightcv)
    
  def moveEyes(self,tiltAngle=None, panAngle=None):
    query = yarp.Bottle()
    answer = yarp.Bottle()
    sep = ' '
    if tiltAngle is not None:
      query.clear()
      answer.clear()
      s = 'set pos '+str(3)+sep+str(tiltAngle)
      query.fromString(s)
      self.eyes.write(query,answer)
    elif panAngle is not None:
      query.clear()
      answer.clear()
      s = 'set pos '+str(4)+sep+str(tiltAngle)
      query.fromString(s)
      self.eyes.write(query,answer)
    else:
      print('Neither Tilt Angle nor Pan Angle are specified')
      return -1
    return 0
    
