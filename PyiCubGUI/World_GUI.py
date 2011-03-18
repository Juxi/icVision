#########################################
### Name: World_GUI.py
### Author: Varun Raj Kompella
### Affliation: IDSIA-Manno Switzerland
######################################### 
 

import yarp
from opencv import cv
from opencv import highgui as hg
from time import sleep
import sys, scipy, scipy.misc,os
import numpy as np
from PyQt4 import QtGui,QtCore
from PyQt4.Qt import QMutex
import cPickle


class WorldGui(QtGui.QWidget):

  def __init__(self, iCubParts, world):
    QtGui.QWidget.__init__(self)

    self.iCubParts = iCubParts
    self.world = world
    self.objectWrkSpace = list() 
    self.setWindowTitle('PyiCubGUI: Simulator-World')
    
    self.worldGroup = QtGui.QGroupBox()
    #self.worldGroup.setTitle("Simulator-World")
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
    
    ##Set/Get Pose Object
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
    self.filenametext.setMinimumSize(40,25)
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

    self.finalLayout = QtGui.QVBoxLayout()
    self.intLayout = QtGui.QHBoxLayout()

    self.intLayout.addWidget(self.worldGroup)
    self.finalLayout.addLayout(self.intLayout)
    self.setLayout(self.finalLayout)
    
    self.loadwrkspceflag = 0    

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

  def loadWrkSpace(self):
    filename = str(self.filenametext.text())
    self.loadwrkspceflag = 1
    fh = open(filename,'r')
    wrkspace = cPickle.load(fh)
    fh.close()
    self.objectWrkSpace = wrkspace
    self.loadworld()
    self.loadwrkspceflag = 0

  def loadworld(self):
    for i in range(len(self.objectWrkSpace)):
      self.create(i)

  def saveWrkSpace(self):
    filename = str(self.filenametext.text())
    fh = open(filename,'wb')
    cPickle.dump(self.objectWrkSpace,fh)
    fh.close()

