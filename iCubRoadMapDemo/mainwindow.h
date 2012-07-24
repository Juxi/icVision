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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graphwidget.h"
#include "roadmap.h"
#include "newMapDialog.h"
#include "iCubController.h"
#include "controlThread.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
	~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
	void resizeEvent( QResizeEvent* event );
//! [0]
	
signals:
	void resizedMainWindow( QResizeEvent* event );

//! [1]
private slots:
	void newMap();
	void loadMap( bool display = true );
	void stealthLoadMap() { loadMap(false); }
	void saveMap();
	void connectMap();
	void projectMap();
	void importNesMap();
	
	void connectToRobot();
	void disconnectFromRobot();
	void multiEdgeExplore();
	void goTo();
	void stopController();
	void setVelocity();

	void test_shit();

//! [1]

//! [2]
private:
    void createActions();
    void createMenus();
//! [2]

//! [3]
    QMenu *mapMenu;
	QAction *newMapAction;
	QAction *loadMapAction;
	QAction *stealthLoadMapAction;
	QAction *saveMapAction;
	QAction *connectMapAction;
	QAction *projectMapAction;
	QAction *testShitAction;
	QAction *importNesMapAction;
	
	QMenu *controllerMenu;
	QAction *connectToRobotAction;
	QAction *disconnectFromRobotAction;
	QAction *MEExploreAction;
	QAction *GoToAction;
	QAction *stopControllerAction;
	QAction *setVelocityAction;

	iCubController	iCub;
	std::vector<bool> jMask;
	
	Roadmap			roadmap;
	GraphWidget		graphWidget;
	
	friend class ControlThread;
	ControlThread	ctrlThread;
	
};
//! [3]

#endif
