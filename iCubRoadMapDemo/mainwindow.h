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
	void loadMap();
	void saveMap();
	void connectMap();
	void projectMap();
	
	void connectToRobot();
	void disconnectFromRobot();
	void explore();
	void stopController();
	void setVelocity();

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
	QAction *saveMapAction;
	QAction *connectMapAction;
	QAction *projectMapAction;
	
	QMenu *controllerMenu;
	QAction *connectToRobotAction;
	QAction *disconnectFromRobotAction;
	QAction *exploreAction;
	QAction *stopControllerAction;
	QAction *setVelocityAction;

	iCubController	iCub;
	Roadmap			roadmap;
	GraphWidget		graphWidget;
	
	friend class ControlThread;
	ControlThread	ctrlThread;
	
	typedef std::vector< std::vector<float> > poses_vector_t;
	typedef std::map< std::string, poses_vector_t > poses_map_t;
	
	poses_map_t read_poses(std::string filename) {
		std::ifstream in_file(filename.c_str());
		
		poses_map_t the_map;
		poses_vector_t the_poses;
		
		std::string current_name;
		while (true) {
			std::string line;
			getline(in_file, line);
			if (!in_file)
				break;
			
			if (std::isalpha(line[0])) {
				if (current_name.size())
					the_map[current_name] = the_poses;
				std::remove(line.begin(), line.end(), ' ');
				current_name = line;
			} else {
				std::istringstream line_reader(line);
				std::vector<float> pose;
				while (true) {
					float number;
					line_reader >> number;
					if (!line_reader)
						break;
					pose.push_back(number);
				}
				the_poses.push_back(pose);
			}
		}
		the_map[current_name] = the_poses;
		return the_map;
	}
};
//! [3]

#endif
