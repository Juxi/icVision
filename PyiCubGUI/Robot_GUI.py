#########################################
### Name: Robot_GUI.py
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

class RobotGui(QtGui.QWidget):

  def __init__(self,body,eyes):
    QtGui.QWidget.__init__(self)

    self.body = body
    self.eyes = eyes
    self.setWindowTitle('PyiCubGUI: Robot')

    self.robotGroup = QtGui.QGroupBox()
    #self.robotGroup.setTitle("Robot")
    bodybox = QtGui.QGridLayout()

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

    #Load/Save Workspace
    self.label = QtGui.QLabel("Load/Save WorkSpace: ")
    self.filenametext = QtGui.QLineEdit("filename..")
    self.filenametext.setMinimumSize(40,25)
    self.loadButton = QtGui.QPushButton("Load")
    self.saveButton = QtGui.QPushButton("Save")

    bodybox.addWidget(self.poslabel,0,3)
    bodybox.addWidget(self.vellabel,0,4)
    bodybox.addWidget(self.bodyid,1,0)
    bodybox.addWidget(self.motorid,1,1)
    bodybox.addWidget(self.motorvalText,1,2)
    bodybox.addWidget(self.PosSlider,1,3)
    bodybox.addWidget(self.VelSlider,1,4)
    bodybox.addWidget(self.displayButton,2,0)
    bodybox.addWidget(self.bodyResetButton,2,1)
    
    bodybox.addWidget(self.label,3,0)
    bodybox.addWidget(self.filenametext,3,1)
    bodybox.addWidget(self.loadButton,3,2)
    bodybox.addWidget(self.saveButton,3,3)

    self.connect(self.bodyid, QtCore.SIGNAL('activated(int)'), self.bodyidFunc)
    self.connect(self.motorid, QtCore.SIGNAL('activated(int)'), self.motoridFunc)
    self.connect(self.motorvalText, QtCore.SIGNAL('returnPressed()'), self.motorvalFunc)
    self.connect(self.PosSlider, QtCore.SIGNAL('valueChanged(int)'), self.posslidFunc1)
    self.connect(self.displayButton, QtCore.SIGNAL('clicked()'), self.display)
    self.connect(self.bodyResetButton, QtCore.SIGNAL('clicked()'), self.bodyReset)
    self.connect(self.loadButton, QtCore.SIGNAL('clicked()'), self.loadWrkSpace)
    self.connect(self.saveButton, QtCore.SIGNAL('clicked()'), self.saveWrkSpace)

    self.robotGroup.setLayout(bodybox)

    self.finalLayout = QtGui.QVBoxLayout()
    self.intLayout = QtGui.QHBoxLayout()

    self.intLayout.addWidget(self.robotGroup)
    self.finalLayout.addLayout(self.intLayout)
    self.setLayout(self.finalLayout)
    
    self.loadwrkspceflag = 0    
    #self.MotionDone = 1


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
    self.motorvalText.setText(str(int(prevVal)))  
    mi = self.body.bodylimits[self.bodyCurrId][self.motorCurrId,0]
    ma = self.body.bodylimits[self.bodyCurrId][self.motorCurrId,1]
    self.PosSlider.setMaximum(int(ma))
    self.PosSlider.setMinimum(int(mi))
    self.PosSlider.setValue(int(prevVal))
    
  def motorvalFunc(self):
    if self.motorCurrId is not None:
      mi = self.body.bodylimits[self.bodyCurrId][self.motorCurrId,0]
      ma = self.body.bodylimits[self.bodyCurrId][self.motorCurrId,1]
      if (mi <= int(self.motorvalText.text())) and (int(self.motorvalText.text()) <= ma):
	self.bodyWrkSpace[self.bodyCurrId][self.motorCurrId] = int(self.motorvalText.text())
	self.PosSlider.setValue(int(self.motorvalText.text()))
	self.body.movePosition(self.bodyCurrId, self.motorCurrId, int(self.motorvalText.text()), 'absolute')
	#self.checkMotionDone()
	#while self.MotionDone is 0:
	  #self.checkMotionDone()
	  #print self.MotionDone
	
  #def checkMotionDone(self):
    #c = self.body.checkMotionDone(self.bodyCurrId, self.motorCurrId)
    #if c is False:
      #self.MotionDone = 0
      #return 0
    #self.MotionDone = 1

  def bodyReset(self):
    self.body.bodyReset()
    self.bodyWrkSpace = self.body.bodyWrkSpace
    if self.motorCurrId is not None:
      val = self.bodyWrkSpace[self.bodyCurrId][self.motorCurrId]
      self.motorvalText.setText(str(val))
      self.PosSlider.setValue(int(val))

  def posslidFunc1(self):
    if self.motorCurrId is not None:
      self.bodyWrkSpace[self.bodyCurrId][self.motorCurrId] = self.PosSlider.value()
      self.motorvalText.setText(str(self.PosSlider.value()))
      self.body.movePosition(self.bodyCurrId, self.motorCurrId, int(self.PosSlider.value()), 'absolute')

  def loadrobot(self):
    for i in range(len(self.bodyWrkSpace)):
      for j in range(len(self.bodyWrkSpace[i])):
	val = self.bodyWrkSpace[i][j]
    	self.body.movePosition(i, j, val, 'absolute')

  def loadWrkSpace(self):
    filename = str(self.filenametext.text())
    self.loadwrkspceflag = 1
    fh = open(filename,'r')
    wrkspace = cPickle.load(fh)
    fh.close()
    self.bodyWrkSpace = wrkspace
    self.loadrobot()
    self.loadwrkspceflag = 0
    
  def saveWrkSpace(self):
    filename = str(self.filenametext.text())
    fh = open(filename,'wb')
    cPickle.dump(self.bodyWrkSpace,fh)
    fh.close()
    
  def display(self):
    self.eyes.grabImages()
    self.eyes.display()

