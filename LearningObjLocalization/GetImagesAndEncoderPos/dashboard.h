/*
 *  dashboard.h
 *  HelloQT
 *
 *  Created by Juxi Leitner on 3/16/11.
 *
 */


#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <yarp/os/all.h>
#include <QtGui>

/**
 *@brief    Dashboard
 */
class Dashboard : public QWidget {
	Q_OBJECT
	
//	friend class Window;
public:
	// Constructor and Destructor
	Dashboard(const char *robot);
	~Dashboard();	
	
	// Methods
	void update();
	QPushButton *btn_connect;	
	QPushButton *btn_initialize;
	
public slots:
	void updateConnectionStatus(bool);
		
private:
	// Variables
	bool iCub_accessible;
	bool iCubSimulator_accessible;

	QString robotName;
	

	// GUI
	QWidget *vwidget;
	QHBoxLayout *layout;
	QVBoxLayout *vlayout;
	QLabel *lbl_iCubSim_Status, *lbl_Connection_Status;
	
protected:
	// Methods
	void checkiCub();
	void checkiCubSimulator();

	
};

#endif 
