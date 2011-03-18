#########################################
### Name: iCub_init.py
### Author: Varun Raj Kompella
### Affliation: IDSIA-Manno Switzerland
######################################### 

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

class iCub_Eyes():
  ''' This class takes image/video input from iCub Eyes 
      to carry out Image processing/vision tasks.
  '''
  def __init__(self):
    self.eyes = yarp.Port()
    self.leftEye = yarp.BufferedPortImageRgb()
    self.rightEye = yarp.BufferedPortImageRgb()
    self.leftEye.open('/iCub_Control/leftEye')
    self.rightEye.open('/iCub_Control/rightEye')
    self.eyes.open('/iCub_Control/eyes')
    yarp.Network.connect('/icubSim/cam/left','/iCub_Control/leftEye')
    yarp.Network.connect('/icubSim/cam/right','/iCub_Control/rightEye')
    yarp.Network.connect('/iCub_Control/eyes','/icubSim/head/rpc:i')
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
    
  def saveImages(self,left_image=1,right_image=1,time_stamp=None,Mode=None,wspce=None):
    if wspce is None:
      print ('Mention the workspace directory')
      return 0
    self.wspce = wspce
    self.workDir = self.wspce.getDir()
    if not self._cnvt2opncvFlag:
      self.convert2opencv()
    if not self._init_disp_flag:
      self._init_display(left_image,right_image)
    if time_stamp is None:
      print('Time stamp not provided, saving images without time stamp')
      time_stamp = 0
    if Mode is None:
      print ('Enter the mode of the simulation')
      return -1
    if left_image:
      limgfname = str(self.workDir[Mode+2]) + 'l_' + str(time_stamp) + '.png'
      hg.cvSaveImage(limgfname,self.image_leftcv)
    if right_image:
      rimgfname = str(self.workDir[Mode+2]) + 'r_' + str(time_stamp) + '.png'
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
    
class sim_World():
  '''This class generates the world that the iCub 
     can interact with.
  '''
  def __init__(self):
    self.world = yarp.Port()
    self.world.open('/iCub_Control/world')
    yarp.Network.connect('/iCub_Control/world','/icubSim/world')
    
  def getCoor(self,objtype=None):
    if objtype is None:
      print('Object Type not specified')
      return -1
    query = yarp.Bottle()
    answer = yarp.Bottle()
    string = objtype.split()
    obj = string[0]
    if obj == 'Box':
      objstr = 'box' + ' ' + str(string[1])
    elif obj == 'Sphere':
      objstr = 'sph' + ' ' + str(string[1])
    elif obj == 'Cylinder':
      objstr = 'cyl' + ' ' + str(string[1])
    if obj == 'SBox':
      objstr = 'sbox' + ' ' + str(string[1])
    elif obj == 'SSphere':
      objstr = 'ssph' + ' ' + str(string[1])
    elif obj == 'SCylinder':
      objstr = 'scyl' + ' ' + str(string[1])
    elif obj == 'cube':
      objstr = 'cube'
    elif obj == 'ball':
      objstr = 'ball'
    s = 'world get ' + objstr
    query.fromString(s)
    self.world.write(query,answer)
    return answer.toString()
    
  def setCoor(self,objtype=None,x=None,y=None,z=None):
    if objtype is None:
      print('Object Type not specified')
      return -1
    query = yarp.Bottle()
    answer = yarp.Bottle()
    sep = ' '
    string = objtype.split()
    obj = string[0]
    if obj == 'Box':
      obj = 'box' + sep + str(string[1])
    elif obj == 'Sphere':
      obj = 'sph' + sep + str(string[1])
    elif obj == 'Cylinder':
      obj = 'cyl' + sep + str(string[1])
    if obj == 'SBox':
      obj = 'sbox' + ' ' + str(string[1])
    elif obj == 'SSphere':
      obj = 'ssph' + ' ' + str(string[1])
    elif obj == 'SCylinder':
      obj = 'scyl' + ' ' + str(string[1])
    elif obj == 'cube':
      objstr = 'cube'
    elif obj == 'ball':
      objstr = 'ball'
    s = 'world set '+obj+sep+str(x)+sep+str(y)+sep+str(z)
    query.fromString(s)
    self.world.write(query,answer)
    return 0
    
  def createObject(self,objtype=None,size=None,x=None,y=None,z=None,color=None, sflag = 0):
    if objtype is None:
      print('Object Type not specified')
      return -1
    obj = objtype
    query = yarp.Bottle()
    answer = yarp.Bottle()
    sep = ' '
    if obj == 'Box':
      if sflag == 0: obj = 'box'
      else: obj = 'sbox'
    elif obj == 'Sphere':
      if sflag == 0: obj = 'sph'
      else: obj = 'ssph'
    elif obj == 'Cylinder':
      if sflag == 0: obj = 'cyl'
      else: obj = 'scyl'
    else:
      print ('Object type not recognized')
      return -1
    s = 'world mk '+obj+sep+str(size)+sep+str(x)+sep+str(y)+sep+str(z)+sep+str(color)
    query.fromString(s)
    self.world.write(query,answer)
    return 0
    

class iCub_Body():
  def __init__(self):
    
    robotname = "/icubSim/"
    
    self.headOptions = yarp.Property()
    self.headOptions.put("device", "remote_controlboard")
    self.headOptions.put("local", "/iCub_Control/head")
    self.headOptions.put("remote", str(robotname + "head"))

    self.right_armOptions = yarp.Property()
    self.right_armOptions.put("device", "remote_controlboard")
    self.right_armOptions.put("local", "/iCub_Control/right_arm")
    self.right_armOptions.put("remote", str(robotname + "right_arm"))

    self.left_armOptions = yarp.Property()
    self.left_armOptions.put("device", "remote_controlboard")
    self.left_armOptions.put("local", "/iCub_Control/left_arm")
    self.left_armOptions.put("remote", str(robotname + "left_arm"))

    self.torsoOptions = yarp.Property()
    self.torsoOptions.put("device", "remote_controlboard")
    self.torsoOptions.put("local", "/iCub_Control/torso")
    self.torsoOptions.put("remote", str(robotname + "torso"))

    self.head = yarp.PolyDriver(self.headOptions)
    self.right_arm = yarp.PolyDriver(self.right_armOptions)
    self.left_arm = yarp.PolyDriver(self.left_armOptions)
    self.torso = yarp.PolyDriver(self.torsoOptions)

    fh = open('./iCub_home_vals','r')
    self.iCub_Home = cPickle.load(fh)
    fh.close()

    self.body = [self.head, self.right_arm, self.left_arm, self.torso]

    self.bodypos = list()
    self.bodylim = list()
    self.bodyjnts = list()
    self.bodylimits = list()
    self.bodyWrkSpace = list()
    for i in range(len(self.body)):
      self.bodypos.append(self.body[i].viewIPositionControl())
      self.bodylim.append(self.body[i].viewIControlLimits())
      self.bodyjnts.append(self.bodypos[i].getAxes())
      partlimits = np.zeros([self.bodyjnts[i],2])
      partWrkSpace = list()
      for j in range(self.bodyjnts[i]):
	partWrkSpace.append(self.iCub_Home[i][j])
	self.bodypos[i].setRefSpeed(j,10.0)
	mi = yarp.DVector(1)
	ma = yarp.DVector(1)
	self.bodylim[i].getLimits(j,mi,ma)
	partlimits[j,0] = mi[0]
	partlimits[j,1] = ma[0]
      self.bodylimits.append(partlimits)
      self.bodyWrkSpace.append(partWrkSpace)

    self.bodyString = ["Head", "Right Arm", "Left Arm", "Torso"]
    
    self.motorString = list()
    for i in range(len(self.body)):
	temp = list()
	for j in range(self.bodyjnts[i]):
	  temp.append(j)
	self.motorString.append(temp)

  def movePosition(self,bodyId, motorId, motorVal, movementType):
    if bodyId is None or motorId is None:
      return 0
    if movementType == 'absolute':
      self.bodypos[bodyId].positionMove(motorId,motorVal)
    elif movementType == 'relative':
      self.bodypos[bodyId].relativeMove(motorId,motorVal)
  
  def close(self):
    for i in range(len(self.body)):
      self.body[i].close()

  def checkMotionDone(self,bodyId,motorId):
    if bodyId is None or motorId is None:
      return 0
    c = yarp.BVector(1)
    self.bodypos[bodyId].checkMotionDone(motorId,c)
    return c[0]
  
  def bodyReset(self):
    for i in range(len(self.bodyWrkSpace)):
      for j in range(len(self.bodyWrkSpace[i])):
	self.bodyWrkSpace[i][j] = self.iCub_Home[i][j]
	self.movePosition(i, j, self.iCub_Home[i][j], 'absolute')

    
