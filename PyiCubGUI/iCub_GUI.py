#########################################
### Name: iCub_GUI.py
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
from World_GUI import *
from Robot_GUI import *
from Program_GUI_exp2 import *


#Set the path for the icub parts config file. search for iCub_parts_activation.ini 
global parts_config_file
parts_config_file = '../iCub_parts_activation.ini'

global SimMode
SimMode = False

global RobotControl
RobotControl = True

global ProgramPanel
ProgramPanel = True


##################################################################################1


class iCubSimGui():
  def __init__(self,args):
    self.app=QtGui.QApplication(args)

    winlist = []

    global SimMode
    if SimMode:
      ### iCub Simulator
      iCubParts = self.iCubConfig(config_file=None)    #config_file = 'path to your iCub_parts_activation.ini file'
      self.world = sim_World()
      win1=WorldGui(iCubParts, self.world)
      win1.show()
      winlist.append(win1)

  
    global RobotControl
    global ProgramPanel
    if RobotControl or ProgramPanel:
      self.body = iCub_Body()
      self.eyes = iCub_Eyes()
      
    if RobotControl:
      ### iCub Robot
      win2=RobotGui(self.body,self.eyes)
      win2.show()
      winlist.append(win2)

    if ProgramPanel:
      ### User Program Panel
      wspce = workspace()
      win3=ProgramGui(self.body,self.eyes,wspce=wspce)
      win3.show()
      winlist.append(win3)

    self.app.connect(self.app, QtCore.SIGNAL("lastWindowClosed()"), self.app, QtCore.SLOT("quit()"))
    self.app.exec_()

  def iCubConfig(self, config_file=None):
    global parts_config_file
    if config_file == None:
      config_file = parts_config_file
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
    return self.iCubParts
    
#################################################################################


if __name__ == '__main__':
  yarp.Network.init()	#initialize Yarp Network

  iCubSimGui(sys.argv)

  yarp.Network.fini()	#Close the yarp Network


##########################################################################################

