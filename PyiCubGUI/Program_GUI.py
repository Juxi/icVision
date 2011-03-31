#########################################
### Name: Program_GUI.py
### Author: Varun Raj Kompella
### Affliation: IDSIA-Manno Switzerland
######################################### 

import sys, scipy, scipy.misc,os, time
from time import sleep
import numpy as np
from PyQt4 import QtGui,QtCore
from PyQt4.Qt import QMutex
import mdp

class randomWalk:
  ''' This class generates random walk over the configuration 
      space and generates a state table. It is based on velocity
      and a momentum term. Refer the paper:
      Slowness and Sparseness Lead to Place, Head-Direction, and Spatial-
      View Cells - Mathias Franzius, Henning Sprekeler, and Laurenz Wiskott
      PLoS Computational Biology.
  '''
  def __init__(self,lim,dataset_len):
    self.dataset_len = dataset_len
    self.state = np.zeros([self.dataset_len,2])
    self.vpos = 4.0
    self.m = 0.75
    self.stateNr = 0
    self.lim = lim
    #self.lim = np.array([[0.0,55.0],[5.0,40.0]])
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
    
  def printStateTable(self,filename,wp):
    state_pr = np.zeros([self.state.shape[0],self.state.shape[1]+1])
    state_pr[:,0] = np.arange(self.state.shape[0])
    a = np.round(self.state[self.stateNr]).astype('int')
    for i in xrange(a.shape[0]):
      a[i] = a[i].clip(self.lim[i,0], self.lim[i,1])
    state_pr[:,1:] = a
    np.savetxt(wp.workDir[4] + filename,state_pr)
    np.savetxt(filename,state_pr)

  def getState(self):
    a = np.round(self.state[self.stateNr]).astype('int')
    for i in xrange(a.shape[0]):
      a[i] = a[i].clip(self.lim[i,0], self.lim[i,1])
    return a
    
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



class ProgramGui(QtGui.QWidget):

  def __init__(self, body, eyes, wspce=None):
    QtGui.QWidget.__init__(self)
    
    self.setWindowTitle('PyiCubGUI: Program')

    self.finalLayout = QtGui.QVBoxLayout()
    self.intLayout = QtGui.QHBoxLayout()

    self.programGroup = QtGui.QGroupBox()
    programbox = QtGui.QGridLayout()
    programbox.setColumnStretch(4,8)

    self.timelabel = QtGui.QLabel("T")
    self.timelabel.setAlignment(QtCore.Qt.AlignCenter)
    self.actorlabel = QtGui.QLabel("Actor")
    self.actorlabel.setAlignment(QtCore.Qt.AlignCenter)
    self.actorposlabel = QtGui.QLabel("Pose")
    self.actorposlabel.setAlignment(QtCore.Qt.AlignCenter)
    #self.motiondonelabel = QtGui.QLabel("Check Motion")
    #self.motiondonelabel.setAlignment(QtCore.Qt.AlignCenter)
    self.timeText = QtGui.QLineEdit()
    self.timeText.setMinimumSize(40,25)
    self.actorText = QtGui.QLineEdit()
    self.actorText.setMinimumSize(40,25)
    self.actorposText = QtGui.QLineEdit()
    self.actorposText.setMinimumSize(40,25)
    #self.motiondoneText = QtGui.QLineEdit()
    #self.motiondoneText.setMinimumSize(40,25)
   
    self.timeText.setFocusPolicy(QtCore.Qt.NoFocus)
    self.actorText.setReadOnly(True)
    self.actorposText.setReadOnly(True)
    #self.motiondoneText.setReadOnly(True)
    
    self.stepButton = QtGui.QPushButton ("Step")
    self.runButton = QtGui.QPushButton ("Run")
    self.stopButton = QtGui.QPushButton ("Stop")
    self.resetButton = QtGui.QPushButton ("Reset")
 
    programbox.addWidget(self.timelabel,0,0)
    programbox.addWidget(self.actorlabel,1,0)
    programbox.addWidget(self.actorposlabel,2,0)
    #programbox.addWidget(self.motiondonelabel,3,0)
    programbox.addWidget(self.timeText,0,1)
    programbox.addWidget(self.actorText,1,1)
    programbox.addWidget(self.actorposText,2,1)
    #programbox.addWidget(self.motiondoneText,3,1)
    programbox.addWidget(self.stepButton,0,2)
    programbox.addWidget(self.runButton,1,2)
    programbox.addWidget(self.stopButton,2,2)
    programbox.addWidget(self.resetButton,3,2)
    
    self.programGroup.setLayout(programbox)

    ###-----

    self.RecordGroup = QtGui.QGroupBox()
    Recordbox = QtGui.QGridLayout()
    #Recordbox.setColumnStretch(4,8)

    self.Recordlabel = QtGui.QLabel("Record")
    self.Recordlabel.setAlignment(QtCore.Qt.AlignCenter)
    
    self.RecordStartButton = QtGui.QPushButton ("Start")
    
    Recordbox.addWidget(self.Recordlabel,0,0)
    Recordbox.addWidget(self.RecordStartButton,1,0)

    self.userprog = UserProgram(body,eyes,wspce,parent=self)

    self.connect(self.timeText, QtCore.SIGNAL('returnPressed()'), self.userprog.setTime)
    self.connect(self.stepButton, QtCore.SIGNAL('clicked()'), self.userprog.step)
    self.connect(self.runButton, QtCore.SIGNAL('clicked()'), self.userprog.run1)
    self.connect(self.stopButton, QtCore.SIGNAL('clicked()'), self.userprog.stop)
    self.connect(self.resetButton, QtCore.SIGNAL('clicked()'), self.userprog.reset)

    self.connect(self.RecordStartButton, QtCore.SIGNAL('clicked()'), self.userprog.record)

    self.RecordGroup.setLayout(Recordbox)

    self.intLayout.addWidget(self.programGroup)
    self.intLayout.addWidget(self.RecordGroup)
    self.finalLayout.addLayout(self.intLayout)
    
    self.setLayout(self.finalLayout)


class UserProgram(QtCore.QThread):
  def __init__(self, body, eyes, wspce, parent = None):
    QtCore.QThread.__init__(self)

    self.body = body
    self.eyes = eyes
    self.wspce = wspce
    self.parent = parent
    
    self.actors = np.array([[1,1],[1,3]])    #[bodyCurrId, motorCurrId]
    
    self.bodyWrkSpace = self.body.bodyWrkSpace
    self.lim = np.zeros([self.actors.shape[0],2])
    self.actorstring = ''
    for i in xrange(self.actors.shape[0]):
      self.lim[i,0] = self.body.bodylimits[self.actors[i,0]][self.actors[i,1],0]
      self.lim[i,1] = self.body.bodylimits[self.actors[i,0]][self.actors[i,1],1]
      self.actorstring += str(self.actors[i,0]) + ':' + str(self.actors[i,1]) + ','
  
    #Random walk Training Dataset
    self.train_dataset_len = 10000				#Training Dataset Length
    self.fixedlim = np.array([[self.lim[0,0],55],[self.lim[1,0],40]])
    self.rw = randomWalk(self.fixedlim,self.train_dataset_len)	#initialize randomwalk states
    self.rw.generateStateTable()			#Generate state table
    self.rw.printStateTable('training_state_table', wp=self.wspce)#Print the state table (For ground truth)

    ##Grid Test Dataset
    #self.gw = gridWalk(self.wspce)			#initialize gridwalk states
    #self.gw.generateStateTable()			#Generate state table
    #self.gw.printStateTable('test_state_table')		#Print the state table (For ground truth)

    self.t = 0
    self.running = 0
    self.MotionDone = True
    self.pose = self.rw.getState()
    self.updateParent()
    
    self.record_running = 0

  def setTime(self):
    self.t = self.timeText.text()
    self.rw.setStateNr(self.t)
    self.step()

  def step(self):
    state = self.rw.getState()
    self.pose = state
    for i in xrange(self.actors.shape[0]):
      if (self.lim[i,0] <= state[i]) and (state[i] <= self.lim[i,1]):
	self.body.movePosition(int(self.actors[i,0]), int(self.actors[i,1]), int(state[i]), 'absolute')
      else:
	print 'State value is out of limits for the joint: ', i, state[i], self.mi[i], self.ma[i]
	sys.exit(0)
    self.rw.step()
    self.t +=1
    self.checkMotionDone()
    while not self.MotionDone:
      self.msleep(100)
      self.checkMotionDone()
    if self.running == 1:
      self.updateParent()
    self.display()
    if self.record_running == 1:
      self.eyes.saveImages(0,1,self.t,Mode=0,wspce=self.wspce)
    
  def run(self):
    while self.running:
      self.step()
      if self.t >= self.train_dataset_len:
	self.running = 0

  def run1(self):
    self.running = 1
    self.start()
    
  def stop(self):
    self.running = 0
    self.start()
    self.updateParent()

  def reset(self):
    self.t = 0
    self.running = 0
    self.MotionDone = True
    self.rw.setStateNr(self.t)
    self.step()
    self.start()
    self.updateParent()

  def checkMotionDone(self):
    for i in xrange(self.actors.shape[0]):
      c = self.body.checkMotionDone(int(self.actors[i,0]), int(self.actors[i,1]))
      if c is False:
	self.MotionDone = False
	return 0
    self.MotionDone = True

  def updateParent(self):
    self.parent.timeText.setText(str(self.t))
    self.parent.actorposText.setText(str(self.pose))
    self.parent.actorText.setText(self.actorstring)

  def display(self):
    self.eyes.grabImages()
    self.eyes.display(0,1)
    self.msleep(100)

  def record(self):
    if self.record_running == 1:
      self.parent.RecordStartButton.setText('Start')
      self.record_running = 0
    else:
      self.parent.RecordStartButton.setText('Stop')
      self.record_running = 1
      
    