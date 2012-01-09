/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include "newMapDialog.h"

//! [0]
MainWindow::MainWindow() : ctrlThread( &iCub, &roadmap )
{
	setCentralWidget(&graphWidget);
	
    createActions();
    createMenus();

    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);

    setWindowTitle(tr("Menus"));
    setMinimumSize(160, 160);
	
	connect( this, SIGNAL(resizedMainWindow(QResizeEvent*)),	&graphWidget, SLOT(resize(QResizeEvent*)));
	resize(480, 320);
	
	connect( &roadmap, SIGNAL(appendedNode(vertex_t, qreal, qreal)),			&graphWidget, SLOT(addNode(vertex_t, qreal, qreal)));
	connect( &roadmap, SIGNAL(appendedEdge(edge_t,QtGraphNode*,QtGraphNode*)),	&graphWidget, SLOT(addEdge(edge_t,QtGraphNode*,QtGraphNode*)));
	
	connect( &graphWidget, SIGNAL(newQtGraphNode(vertex_t,QtGraphNode*)),		&roadmap, SLOT(setQtGraphNode( vertex_t, QtGraphNode* )));
	connect( &graphWidget, SIGNAL(newQtGraphEdge(edge_t,QtGraphEdge*)),			&roadmap, SLOT(setQtGraphEdge( edge_t, QtGraphEdge* )));
	
	connect( &roadmap, SIGNAL(update2DPosition(QtGraphNode*,QPointF)),			&graphWidget, SLOT(setNodePosition(QtGraphNode*,QPointF)));
	//connect( &roadmap, SIGNAL(removeQtGraphEdge(QtGraphEdge*)),					&graphWidget, SLOT(removeEdge(QtGraphEdge*)));

	connect( &roadmap, SIGNAL(newNodeColor(QtGraphNode*,QColor,QColor)),		&graphWidget, SLOT(setNodeColor(QtGraphNode*,QColor,QColor)));
	connect( &roadmap, SIGNAL(newEdgeColor(QtGraphEdge*,QColor)),				&graphWidget, SLOT(setEdgeColor(QtGraphEdge*,QColor)));
	
	//QVBoxLayout *mainLayout = new QVBoxLayout;
	//mainLayout->addWidget(&graphWidget);
	//setLayout(mainLayout);
}
//! [2]

MainWindow::~MainWindow()
{
	if ( ctrlThread.isRunning() )
		ctrlThread.stop();
}

void MainWindow::resizeEvent( QResizeEvent* event )
{
	emit resizedMainWindow(event);
}

void MainWindow::connectToRobot()
{
	bool ok;
	QString robotName = QInputDialog::getText(this, tr("QInputDialog::getText()"),
											  tr("Robot Name:"), QLineEdit::Normal,
											  "icubSimF", &ok);
	if (ok && !robotName.isEmpty())
	{
		//roadmap = new Roadmap();
		if ( iCub.open(robotName.toStdString().c_str()) )
		{
			roadmap.setDimensionality( iCub.getNumJoints() );
			printf("Opened iCub Robot with %d joints!!!\n", iCub.getNumJoints());
		}
		else { printf("failed to open iCub\n"); }
	}
}

void MainWindow::setRefVel()
{
	bool ok;
	int vel = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
						 tr("Percentage:"), 5, 1, 20, 1, &ok);
	
	if (ok)
	{
		iCub.setRefVels(vel);
	}
}

void MainWindow::disconnectFromRobot()
{
	iCub.close();
}

void MainWindow::explore()
{
	ctrlThread.restart();
}

void MainWindow::stopController()
{
	ctrlThread.stop();
}

//! [3]
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    //menu.addAction(cutAct);
    //menu.addAction(copyAct);
    //menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}
//! [3]

void MainWindow::newMap()
{
	NewMapDialog newMapDialog;
	
	connect( &newMapDialog, SIGNAL(requestRandomSample()),			&iCub, SLOT(getRandomPose()));
	connect( &newMapDialog, SIGNAL(requestCurrentPose()),			&iCub, SLOT(getCurrentPose()));
	connect( &iCub, SIGNAL(newSample( std::vector<double> )),		&newMapDialog, SLOT(insertSample( std::vector<double> )));
	
	newMapDialog.exec();
}

void MainWindow::saveMap()
{
	std::vector< std::vector<double> > graphNodes;
	std::vector< std::pair<int,int> > graphEdges;
	roadmap.data( &graphNodes, &graphEdges );
	
	QString fileName = QFileDialog::getSaveFileName(this,
													tr("Save Roadmap"), "",
													tr("All Files (*)"));
	if (fileName.isEmpty())
		return;
	else {
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::information(this, tr("Unable to open file"),
									 file.errorString());
			return;
		}
		QTextStream out(&file);
		std::vector< std::vector<double> >::iterator v;
		std::vector<double>::iterator j;
		for ( v = graphNodes.begin(); v !=graphNodes.end(); ++v )
		{
			for ( j = v->begin(); j !=v->end(); ++j )
			{
				out << *j << " ";
			}
			out << "\n";
		}
		
		out << "\n" << "EDGES" << "\n";
		
		std::vector< std::pair<int,int> >::iterator e;
		for ( e=graphEdges.begin(); e!=graphEdges.end(); ++e )
		{
			out << e->first << "\t" << e->second << "\n";
		}
		
		file.close();
	}
}

void MainWindow::loadMap()
{
	if ( !iCub.isValid() )
	{
		printf("Cannot load map file because iCub is not valid!!!\n");
		return;
	}
	
	QString fileName = QFileDialog::getOpenFileName(this,
													tr("Open Address Book"), "",
													tr("All Files (*)"));
	if (fileName.isEmpty())
		return;
	else {
		
		QFile file(fileName);
		std::vector< std::vector<double> > graphNodes;
		std::vector< std::pair<int,int> > graphEdges;
		
		if (!file.open(QIODevice::ReadOnly)) {
			QMessageBox::information(this, tr("Unable to open file"),
									 file.errorString());
			return;
		}
		
		QTextStream in(&file);
		QString line;

		/*** HANDLE VERTICES ***/
		double n;				// dimension of p
		std::vector<double> p;	// an n dimensional point
		
		//int count = 0;
		while ( !in.atEnd() )
		{
			line = in.readLine();
			if ( line == "EDGES" )
				break;
			if ( line != "" )
			{
				QTextStream lineStream(&line);
				p.clear();
				
				// put the line into a std::vector and don't worry about number of entries
				for ( int i=0; i < roadmap.dimensionality()+2; i++)
				{
					if ( !lineStream.atEnd() ) {
						lineStream >> n;
						p.push_back(n);
					} else {
						p.push_back(0.0);
					}
				}
				if ( p.size() != roadmap.dimensionality()+2 )
				{
					printf("file parse error. wrong size point.");
					return;
				}
				graphNodes.push_back(p);
				//printf("lineCount: %d\n", count );
				//count ++;
			}
		}
	
		/*** HANDLE EDGES ***/
		int i,j;
		while ( !in.atEnd() )
		{
			line = in.readLine();
			if ( line != "" && line != "EDGES" )
			{
				QTextStream lineStream(&line);
				lineStream >> i >> j;
				graphEdges.push_back( std::pair<int,int>(i,j) );
			}
		}
		
		printf("loaded file: %d nodes, %d edges\n",graphNodes.size(),graphEdges.size());
		roadmap.load( graphNodes, graphEdges );
		
		file.close();
	}
}

void MainWindow::connectMap()
{
	bool ok;
	int i = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
								 tr("Number of Neighbors:"), 3, 1, 100, 1, &ok);
	if (ok)
		roadmap.graphConnect(i);
}

void MainWindow::projectMap()
{
	roadmap.project2D();
}

//! [4]
void MainWindow::createActions()
{
	// CONTROLLER MENU
	connectToRobotAction = new QAction(tr("&Connect To Robot"), this);
	connectToRobotAction->setShortcut( QKeySequence(tr("Ctrl+R")) );
	connectToRobotAction->setStatusTip(tr("Connect to an iCub Robot"));
	connect(connectToRobotAction, SIGNAL(triggered()), this, SLOT(connectToRobot()));
	
	disconnectFromRobotAction = new QAction(tr("&Disconnect From Robot"), this);
	disconnectFromRobotAction->setShortcut( QKeySequence(tr("Ctrl+D")) );
	disconnectFromRobotAction->setStatusTip(tr("Disconnect from the iCub"));
	connect(disconnectFromRobotAction, SIGNAL(triggered()), this, SLOT(disconnectFromRobot()));
	
	setRefVelAction = new QAction(tr("&Set Ref Velocity"), this);
	setRefVelAction->setShortcut( QKeySequence(tr("Ctrl+V")) );
	setRefVelAction->setStatusTip(tr("Set the reference velocity on all joints"));
	connect(setRefVelAction, SIGNAL(triggered()), this, SLOT(setRefVel()));
	
	exploreAction = new QAction(tr("&Explore"), this);
	exploreAction->setShortcut( QKeySequence(tr("Ctrl+E")) );
	exploreAction->setStatusTip(tr("Move the iCub around on the Roadmap"));
	connect(exploreAction, SIGNAL(triggered()), this, SLOT(explore()));
	
	stopControllerAction = new QAction(tr("&Stop Controller"), this);
	stopControllerAction->setShortcut( QKeySequence(tr("Ctrl+X")) );
	stopControllerAction->setStatusTip(tr("Stop controlling the iCub"));
	connect(stopControllerAction, SIGNAL(triggered()), this, SLOT(stopController()));
	
	// MAP MENU
	newMapAction = new QAction(tr("&New Map"), this);
	newMapAction->setShortcuts(QKeySequence::New);
    newMapAction->setStatusTip(tr("Create a new Roadmap"));
    connect(newMapAction, SIGNAL(triggered()), this, SLOT(newMap()));
	
	loadMapAction = new QAction(tr("&Load Map"), this);
	loadMapAction->setShortcuts(QKeySequence::Open);
    loadMapAction->setStatusTip(tr("Load a Roadmap from file"));
    connect(loadMapAction, SIGNAL(triggered()), this, SLOT(loadMap()));
	
	saveMapAction = new QAction(tr("&Save Map"), this);
	saveMapAction->setShortcuts(QKeySequence::Save);
    saveMapAction->setStatusTip(tr("Save the current map to file"));
    connect(saveMapAction, SIGNAL(triggered()), this, SLOT(saveMap()));
	
	connectMapAction = new QAction(tr("&Connect Map"), this);
	connectMapAction->setShortcut( QKeySequence(tr("Ctrl+C")) );
    connectMapAction->setStatusTip(tr("Connect Nodes to their N nearest neighbors"));
    connect(connectMapAction, SIGNAL(triggered()), this, SLOT(connectMap()));
	
	projectMapAction = new QAction(tr("&Project Map"), this);
	projectMapAction->setShortcut( QKeySequence(tr("Ctrl+P")) );
    projectMapAction->setStatusTip(tr("Make a new 2D map projection"));
    connect(projectMapAction, SIGNAL(triggered()), this, SLOT(projectMap()));
}
//! [7]

//! [8]
void MainWindow::createMenus()
{
	controllerMenu = menuBar()->addMenu(tr("&Controller"));
	controllerMenu->addAction(connectToRobotAction);
	controllerMenu->addAction(disconnectFromRobotAction);
	controllerMenu->addAction(exploreAction);
	controllerMenu->addAction(stopControllerAction);
	
    mapMenu = menuBar()->addMenu(tr("&Map"));
	mapMenu->addAction(newMapAction);
	mapMenu->addAction(loadMapAction);
	mapMenu->addAction(saveMapAction);
	mapMenu->addAction(connectMapAction);
	mapMenu->addAction(projectMapAction);
}
//! [12]
