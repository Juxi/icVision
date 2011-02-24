#########################################
### Name: iCub_Control.py
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
from iCub_init import *

class workspace:
  def __init__(self):
    #Set up working directory
    while(1):
      projDir = raw_input("Hello! Kindly enter the project workspace directory: ")
      if not(os.path.exists(projDir)):
	print "Creating the project directory...";sys.stdout.flush()
	os.mkdir(projDir)
	break
      else:
	response = raw_input("Directory already exists, Do you want to continue? (y/n): ")
	if response == 'y':
	  break
    self.workDir = []
    self.dircount = 0
    self.workDir.append(projDir)				#workDir[0] - <projDir>/
    self.dircount +=1
    if not(os.path.exists(self.workDir[0] + 'pics_dump')):    
      os.mkdir(self.workDir[0] + 'pics_dump')
    self.workDir.append(self.workDir[0] + 'pics_dump/')		#workDir[1] - <projDir>/pics_dump/
    self.dircount +=1
    if not(os.path.exists(self.workDir[1] + 'training')):    
      os.mkdir(self.workDir[1] + 'training')
    self.workDir.append(self.workDir[1] + 'training/')		#workDir[2] - <projDir>/pics_dump/training/
    self.dircount +=1
    if not(os.path.exists(self.workDir[1] + 'testing')):    
      os.mkdir(self.workDir[1] + 'testing')
    self.workDir.append(self.workDir[1] + 'testing/')		#workDir[3] - <projDir>/pics_dump/testing/
    self.dircount +=1
    if not(os.path.exists(self.workDir[0] + 'state_tables')):    
      os.mkdir(self.workDir[0] + 'state_tables')
    self.workDir.append(self.workDir[0] + 'state_tables/')	#workDir[4] - <projDir>/state_tabels/
    self.dircount +=1
  
  def addDir(self,newDir):
    os.mkdir(self.workDir[0] + newDir)
    self.workDir.append(self.workDir[0] + newDir)
    self.dircount +=1
    return self.dircount
  
  def getDir(self):
    return self.workDir
  
  def setDir(self,newDir,dirno):
    self.workDir[dirno] = self.workDir[0] + newDir
  
  def reset(self):
    self.__init__();


class randomWalk:
  ''' This class generates random walk over the configuration 
      space and generates a state table. It is based on velocity
      and a momentum term. Refer the paper:
      Slowness and Sparseness Lead to Place, Head-Direction, and Spatial-
      View Cells - Mathias Franzius, Henning Sprekeler, and Laurenz Wiskott
      PLoS Computational Biology.
  '''
  def __init__(self,wp,dataset_len):
    self.wp = wp
    self.dataset_len = dataset_len
    self.state = np.zeros([self.dataset_len,2])
    self.vpos = 0.07
    self.m = 0.75
    self.stateNr = 0
    self.lim = np.array([[-0.3,0.3],[0.3,0.6]])
    self.state[0] = np.array([0.5*(self.lim[0,0]+self.lim[0,1]),0.5*(self.lim[1,0]+self.lim[1,1])])
    self.state[1] = np.array([0.47*(self.lim[0,0]+1.1*self.lim[0,1]),0.5*(self.lim[1,0]+self.lim[1,1])])

  def isInsideDimRange(self, pos):
    flag = 1
    for i in range(pos.shape[0]):
      if ((pos[i] < self.lim[i,0]) or (pos[i] > self.lim[i,1])):
	flag = 0
    return flag

  def robotPosFut(self):
    #random walk in position
    mean = [0,0]
    cov = [[1,0],[0,1]]
    currentvelocity = self.state[self.stateNr] - self.state[self.stateNr-1]
    while 1:
      noise = np.random.multivariate_normal(mean,cov)*self.vpos
      self.state[self.stateNr+1] = self.state[self.stateNr] + self.m*currentvelocity + (1-self.m)*noise
      if self.isInsideDimRange(self.state[self.stateNr+1]):
	break
      currentvelocity = currentvelocity/2

  def generateStateTable(self):
    print 'Generating Random Walk State Table ... '
    for i in mdp.utils.progressinfo(range(self.dataset_len)):
      if (i < (self.dataset_len-2)):
	self.step()
	self.robotPosFut()
    self.stateNr = 0
    
  def printStateTable(self,filename):
    state_pr = np.zeros([self.state.shape[0],self.state.shape[1]+1])
    state_pr[:,0] = np.arange(self.state.shape[0])
    state_pr[:,1:] = self.state
    np.savetxt(self.wp.workDir[4] + filename,state_pr)

  def getState(self):
    return self.state[self.stateNr]
    
  def getStateNr(self):
    return self.stateNr

  def setStateNr(self,stateNr):
    if(stateNr < self.dataset_len):
      self.stateNr = stateNr
      return 1
    else:
      print "Invalid state"
      return 0

  def step(self):
    if ((self.stateNr+1) < self.dataset_len):
      self.stateNr += 1
      return 1
    else:
      print ' '
      print "Final state Reached"
      return 0

  def printToDisp(self):
    print ("Generating Training Dataset...")

class gridWalk:
  ''' This class generates grid-walk trajectories 
      mainly used for generating a Test Dataset that 
      covers accross the configuration space.
  '''
  def __init__(self,wp):
    self.wp = wp
    self.xDim = 10
    self.yDim = 10
    self.dataset_len = self.xDim*self.yDim
    self.state = np.zeros([self.dataset_len,2])
    self.stateNr = 0
    self.lim = np.array([[-0.3,0.3],[0.3,0.6]])

  def generateStateTable(self):
    #Grid walk in position
    print 'Generating Grid Walk State Table ... '
    for xpos in mdp.utils.progressinfo(np.linspace(self.lim[0][0],self.lim[0][1],self.xDim,endpoint=True)):
      for ypos in np.linspace(self.lim[1][0],self.lim[1][1],self.yDim,endpoint=True):
	self.state[self.stateNr] = np.array([xpos,ypos])
	self.stateNr += 1
    self.stateNr = 0

  def printStateTable(self,filename):
    state_pr = np.zeros([self.state.shape[0],self.state.shape[1]+1])
    state_pr[:,0] = np.arange(self.state.shape[0])
    state_pr[:,1:] = self.state
    np.savetxt(self.wp.workDir[4] + filename, state_pr)

  def getState(self):
    return self.state[self.stateNr]
    
  def getStateNr(self):
    return self.stateNr

  def setStateNr(self,stateNr):
    if(stateNr < self.dataset_len):
      self.stateNr = stateNr
      return 1
    else:
      print "Invalid state"
      return 0

  def step(self):
    if ((self.stateNr+1) < self.dataset_len):
      self.stateNr += 1
      return 1
    else:
      print ' '
      print "Final state Reached"
      return 0

  def printToDisp(self):
    print ("Generating Testing Dataset...")


class iCubSimGui(QtGui.QWidget):
  def __init__(self, world=None, body=None, eyes=None):
    self.app = QtGui.QApplication(sys.argv)
    QtGui.QWidget.__init__(self)
    self.world = world
    self.body = body
    self.eyes = eyes
    self.setWindowTitle('PyiCubGUI')
    self._initUI()
    self.show()
    self.app.exec_()
	
  def _initUI(self):
    self.finalLayout = QtGui.QVBoxLayout()
    self.intLayout = QtGui.QHBoxLayout()

    self.iCubConfig(config_file=None)	#config_file = 'path to your iCub_parts_activation.ini file'
    print self.iCubParts
    self.worldgui()
    self.robotgui()
    #self.programgui()

    self.intLayout.addWidget(self.worldGroup)
    self.intLayout.addWidget(self.robotGroup)
    self.finalLayout.addLayout(self.intLayout)
    #self.finalLayout.addWidget(self.programGroup)
    self.setLayout(self.finalLayout)
    
    self.loadwrkspceflag = 0    
    
  def display(self):
    self.eyes.grabImages()
    self.eyes.display()
    
  def iCubConfig(self, config_file=None):
    if config_file == None:
      config_file = '../iCub_parts_activation.ini'
    self.iCubParts = 0
    file = open(config_file)
    _read = 0
    a = []
    for line in file:
      if _read:
	a.append(line)
      if line == '[RENDER]\n':
	_read = 1
    for i in range(len(a)):
      b = a[i].split()
      if b[0]=='objects' and b[1]=='on':
	self.iCubParts = 1

  def loadWrkSpace(self):
    filename = str(self.filenametext.text())
    self.loadwrkspceflag = 1
    fh = open(filename,'r')
    wrkspace = cPickle.load(fh)
    fh.close()
    self.objectWrkSpace = wrkspace[0]
    self.bodyWrkSpace = wrkspace[1]
    self.loadworld()
    self.loadrobot()
    self.loadwrkspceflag = 0
    
  def saveWrkSpace(self):
    filename = str(self.filenametext.text())
    wrkspace = list()
    wrkspace.append(self.objectWrkSpace)
    wrkspace.append(self.bodyWrkSpace)
    fh = open(filename,'wb')
    cPickle.dump(wrkspace,fh)
    fh.close()
    
  #################################################################################

  def worldgui(self,):
    self.objectWrkSpace = list()   
    
    self.worldGroup = QtGui.QGroupBox()
    self.worldGroup.setTitle("Simulator-World")
    worldbox = QtGui.QGridLayout()

    self.poslabelx = QtGui.QLabel("x")
    self.poslabelx.setAlignment(QtCore.Qt.AlignCenter)
    self.poslabely = QtGui.QLabel("y")
    self.poslabely.setAlignment(QtCore.Qt.AlignCenter)
    self.poslabelz = QtGui.QLabel("z")
    self.poslabelz.setAlignment(QtCore.Qt.AlignCenter)
    self.label1 = QtGui.QLabel("Pose: ")
    self.xText = QtGui.QLineEdit()
    self.xText.setMinimumSize(40,25)
    self.yText = QtGui.QLineEdit()
    self.yText.setMinimumSize(40,25)
    self.zText = QtGui.QLineEdit()
    self.zText.setMinimumSize(40,25)

    #Create Object
    self.label2 = QtGui.QLabel("Create: ")
    self.object = QtGui.QComboBox()
    self.object.addItem("Object Type")
    self.object.addItem("Box")
    self.object.addItem("Sphere")
    self.object.addItem("Cylinder")
    self.object.insertSeparator(1)
    self.ObjectType = "Object Type"
    self.size = QtGui.QLineEdit("size")
    self.size.setMinimumSize(40,25)
    self.color = QtGui.QLineEdit("Color")
    self.color.setMinimumSize(40,25)
    self.static = QtGui.QCheckBox('Static')
    self.static.setFocusPolicy(QtCore.Qt.NoFocus)
    self.static.toggle()
    self.staticFlag = 1
    self.createButton = QtGui.QPushButton("Create")
    
    #Set/Get Pose Object
    self.label3 = QtGui.QLabel("Set/Get Pose: ")
    if self.iCubParts == 1:
      self.objectString = ["Object ID", "cube", "ball"]
    else:
      self.objectString = ["Object ID"]
    self.currObject = self.objectString[0]
    self.currObjectid = [0,0,0]
    self.objectid = QtGui.QComboBox()
    for i in range(len(self.objectString)):
	    self.objectid.addItem(self.objectString[i])
    self.objectid.insertSeparator(1)
    self.getPosButton = QtGui.QPushButton("Get Pos")
    self.setPosButton = QtGui.QPushButton("Set Pos")

    #Load/Save Workspace
    self.label4 = QtGui.QLabel("Load/Save WorkSpace: ")
    self.filenametext = QtGui.QLineEdit("filename..")
    self.size.setMinimumSize(40,25)
    self.loadButton = QtGui.QPushButton("Load")
    self.saveButton = QtGui.QPushButton("Save")
    
    worldbox.addWidget(self.poslabelx,0,1)
    worldbox.addWidget(self.poslabely,0,2)
    worldbox.addWidget(self.poslabelz,0,3)
    worldbox.addWidget(self.label1,1,0)
    worldbox.addWidget(self.xText,1,1)
    worldbox.addWidget(self.yText,1,2)
    worldbox.addWidget(self.zText,1,3)
    worldbox.addWidget(self.label2,2,0)
    worldbox.addWidget(self.object,2,1)
    worldbox.addWidget(self.size,2,2)
    worldbox.addWidget(self.color,2,3)
    worldbox.addWidget(self.static,3,1)
    worldbox.addWidget(self.createButton,3,3)
    worldbox.addWidget(self.label3,4,0)
    worldbox.addWidget(self.objectid,4,1)
    worldbox.addWidget(self.getPosButton,4,2)
    worldbox.addWidget(self.setPosButton,4,3)
    worldbox.addWidget(self.label4,5,0)
    worldbox.addWidget(self.filenametext,5,1)
    worldbox.addWidget(self.loadButton,5,2)
    worldbox.addWidget(self.saveButton,5,3)

    self.connect(self.object, QtCore.SIGNAL('activated(QString)'), self.objectFunc)
    self.connect(self.objectid, QtCore.SIGNAL('activated(QString)'), self.objectidFunc)
    self.connect(self.getPosButton, QtCore.SIGNAL('clicked()'), self.getPose)
    self.connect(self.setPosButton, QtCore.SIGNAL('clicked()'), self.setPose)
    self.connect(self.static, QtCore.SIGNAL('stateChanged(int)'), self.setStatic)
    self.connect(self.createButton, QtCore.SIGNAL('clicked()'), self.create)
    self.connect(self.loadButton, QtCore.SIGNAL('clicked()'), self.loadWrkSpace)
    self.connect(self.saveButton, QtCore.SIGNAL('clicked()'), self.saveWrkSpace)
    
    self.worldGroup.setLayout(worldbox)

  def objectFunc(self,text):
    self.ObjectType = str(text)

  def objectidFunc(self,text):
    self.currObject = text
      
  def getPose(self):
    if self.currObject == "Object ID":
      return 0
    pose = self.world.getCoor(str(self.currObject))
    pose = pose.split(' ')
    self.xText.setText('%.2f'%(float(pose[0])))
    self.yText.setText('%.2f'%(float(pose[1])))
    self.zText.setText('%.2f'%(float(pose[2])))
	
  def setPose(self):
    if self.currObject == "Object ID":
      return 0
    x = float(self.xText.text())
    y = float(self.yText.text())
    z = float(self.zText.text())
    self.world.setCoor(objtype=str(self.currObject),x=x,y=y,z=z)

  def setStatic(self,value):
    if self.static.isChecked():
      self.staticFlag = 1
    else:
      self.staticFlag = 0

  def create(self, currobj=None):
    if self.loadwrkspceflag:
      config = self.objectWrkSpace[currobj]
      self.ObjectType = config[0]
      typeid = config[1]
      x = config[2]
      y = config[3]
      z = config[4]
      size = config[5]
      color = config[6]
      sflag = config[7]
      
    else:
      if self.ObjectType == "Object Type":
	return 0
      if self.ObjectType == "Box": 
	typeid = 0
      elif self.ObjectType == "Sphere":
	typeid = 1
      elif self.ObjectType ==  "Cylinder":
	typeid = 2
	
      x = float(self.xText.text())
      y = float(self.yText.text())
      z = float(self.zText.text())
      size = self.size.text()
      color = self.color.text()
      sflag = self.staticFlag
      
      config = list()
      config.append(self.ObjectType)
      config.append(typeid)
      config.append(x)
      config.append(y)
      config.append(z)
      config.append(size)
      config.append(color)
      config.append(sflag)
      self.objectWrkSpace.append(config)
    
    r = self.world.createObject(self.ObjectType,size=size,x=x,y=y,z=z,color=color,sflag = sflag)
    if r==0:
	self.currObjectid[typeid] += 1
	if sflag == 1:
	  objectname = 'S'+ self.ObjectType + ' ' + str(self.currObjectid[typeid])
	else:
	  objectname = self.ObjectType + ' ' + str(self.currObjectid[typeid])
	self.objectid.addItem(objectname)
	self.objectString.append(objectname)

  def loadworld(self):
    for i in range(len(self.objectWrkSpace)):
      self.create(i)

  #################################################################################

  def robotgui(self,):
    self.robotGroup = QtGui.QGroupBox()
    self.robotGroup.setTitle("Robot")
    simbox = QtGui.QGridLayout()
    
    self.bodyString = self.body.bodyString
    self.bodyid = QtGui.QComboBox()
    self.bodyid.addItem("Body Part")
    for i in range(len(self.bodyString)):
      self.bodyid.addItem(self.bodyString[i])
    self.bodyid.insertSeparator(1)
    self.bodyCurrId = None

    self.motorid = QtGui.QComboBox()
    self.motorid.addItem("Motor ID")
    self.motorid.insertSeparator(1)
    self.motorString = self.body.motorString
    self.motorCurrId = None

    self.bodyWrkSpace = self.body.bodyWrkSpace

    self.motorvalText = QtGui.QLineEdit("Val: ")
    self.motorvalText.setMinimumSize(40,25)

    self.poslabel = QtGui.QLabel("Pos")
    self.poslabel.setAlignment(QtCore.Qt.AlignCenter)
    self.vellabel = QtGui.QLabel("Vel")
    self.vellabel.setAlignment(QtCore.Qt.AlignCenter)

    self.PosSlider = QtGui.QSlider(QtCore.Qt.Vertical)
    self.VelSlider = QtGui.QSlider(QtCore.Qt.Vertical)

    self.displayButton = QtGui.QPushButton("Robot's Veiw")
    self.bodyResetButton = QtGui.QPushButton("Reset All")

    simbox.addWidget(self.poslabel,0,3)
    simbox.addWidget(self.vellabel,0,4)
    simbox.addWidget(self.bodyid,1,0)
    simbox.addWidget(self.motorid,1,1)
    simbox.addWidget(self.motorvalText,1,2)
    simbox.addWidget(self.PosSlider,1,3)
    simbox.addWidget(self.VelSlider,1,4)
    simbox.addWidget(self.displayButton,2,0)
    simbox.addWidget(self.bodyResetButton,2,1)

    self.robotGroup.setLayout(simbox)

    self.connect(self.bodyid, QtCore.SIGNAL('activated(int)'), self.bodyidFunc)
    self.connect(self.motorid, QtCore.SIGNAL('activated(int)'), self.motoridFunc)
    self.connect(self.motorvalText, QtCore.SIGNAL('returnPressed()'), self.motorvalFunc)
    self.connect(self.PosSlider, QtCore.SIGNAL('valueChanged(int)'), self.posslidFunc1)
    self.connect(self.displayButton, QtCore.SIGNAL('clicked()'), self.display)
    self.connect(self.bodyResetButton, QtCore.SIGNAL('clicked()'), self.bodyReset)
    
  def bodyidFunc(self,val):
    if val == 0:
      self.bodyCurrId = None
      self.motorCurrId = None
      self.motorid.clear()
      self.motorid.addItem("Motor ID")
      self.motorid.insertSeparator(1)
      return 0
      
    self.bodyCurrId = val - 2
    self.motorid.clear()
    self.motorid.addItem("Motor ID")
    self.motorid.insertSeparator(1)
    for i in range(len(self.motorString[self.bodyCurrId])):
      self.motorid.addItem(str(self.motorString[self.bodyCurrId][i]))

  def motoridFunc(self,val):
    if val == 0:
      self.motorCurrId = None
      return 0
    self.motorCurrId = val - 2
    prevVal = self.bodyWrkSpace[self.bodyCurrId][self.motorCurrId]
    self.motorvalText.setText(str(prevVal))  
    self.PosSlider.setValue(float(prevVal))
    mi = self.body.bodylimits[self.bodyCurrId][self.motorCurrId,0]
    ma = self.body.bodylimits[self.bodyCurrId][self.motorCurrId,1]
    self.PosSlider.setMaximum(float(ma))
    self.PosSlider.setMinimum(float(mi))
    
  def motorvalFunc(self):
    if self.motorCurrId is not None:
      mi = self.body.bodylimits[self.bodyCurrId][self.motorCurrId,0]
      ma = self.body.bodylimits[self.bodyCurrId][self.motorCurrId,1]
      if (mi < float(self.motorvalText.text())) and (float(self.motorvalText.text()) < ma):
	self.bodyWrkSpace[self.bodyCurrId][self.motorCurrId] = float(self.motorvalText.text())
	self.PosSlider.setValue(float(self.motorvalText.text()))
	self.body.movePosition(self.bodyCurrId, self.motorCurrId, float(self.motorvalText.text()), 'absolute')

  def bodyReset(self):
    self.motorvalText.setText('0')
    self.PosSlider.setValue(0)
    for i in range(len(self.bodyWrkSpace)):
      for j in range(len(self.bodyWrkSpace[i])):
	self.bodyWrkSpace[i][j] = 0.0
    	self.body.movePosition(i, j, 0, 'absolute')

  def posslidFunc1(self):
    if self.motorCurrId is not None:
      self.bodyWrkSpace[self.bodyCurrId][self.motorCurrId] = self.PosSlider.value()
      self.motorvalText.setText(str(self.PosSlider.value()))
      self.body.movePosition(self.bodyCurrId, self.motorCurrId, float(self.PosSlider.value()), 'absolute')

  def loadrobot(self):
    for i in range(len(self.bodyWrkSpace)):
      for j in range(len(self.bodyWrkSpace[i])):
	val = self.bodyWrkSpace[i][j]
    	self.body.movePosition(i, j, val, 'absolute')
	


  #################################################################################

  #def programgui(self):
    #self.programGroup = QtGui.QGroupBox()
    #self.programGroup.setTitle("User Program")
    #programbox = QtGui.QGridLayout()
    #programbox.setColumnStretch(4,8)
    
    #self.statelabel = QtGui.QLabel("State")
    #self.statelabel.setAlignment(QtCore.Qt.AlignCenter)
    #self.actorlabel = QtGui.QLabel("Actor")
    #self.actorlabel.setAlignment(QtCore.Qt.AlignCenter)
    #self.actorposlabel = QtGui.QLabel("Pose")
    #self.actorposlabel.setAlignment(QtCore.Qt.AlignCenter)
    #self.stateText = QtGui.QLineEdit()
    #self.stateText.setMinimumSize(40,25)
    #self.actorText = QtGui.QLineEdit()
    #self.actorText.setMinimumSize(40,25)
    #self.actorposText = QtGui.QLineEdit()
    #self.actorposText.setMinimumSize(40,25)
    
    #self.resetButton3 = QtGui.QPushButton ("Reset")

    #programbox.addWidget(self.statelabel,0,0)
    #programbox.addWidget(self.actorlabel,0,1)
    #programbox.addWidget(self.actorposlabel,0,2)
    #programbox.addWidget(self.stateText,1,0)
    #programbox.addWidget(self.actorText,1,1)
    #programbox.addWidget(self.actorposText,1,2)
    #programbox.addWidget(self.resetButton3,1,3)
    
    #self.programGroup.setLayout(programbox)



if __name__ == '__main__':
  yarp.Network.init()		#initialize Yarp Network

  icubEyes = iCub_Eyes()	#initialize iCub Vision System
  body = iCub_Body()		#initialize iCub Head motion
  world = sim_World()		#initialize iCub Simulator World

  gui = iCubSimGui(world, body, icubEyes)
  
  body.close()

  yarp.Network.fini(); 	#Close the yarp Network


##########################################################################################

