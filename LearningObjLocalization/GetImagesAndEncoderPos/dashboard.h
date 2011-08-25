/*
 *  dashboard.h
 *  HelloQT
 *
 *  Created by Juxi Leitner on 3/16/11.
 *
 */


#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QtGui>

/**
 *@brief    Dashboard
 */
class Dashboard : public QWidget {
	Q_OBJECT
	
//	friend class Window;
public:
	// Constructor and Destructor
	Dashboard();
	~Dashboard();	
	
	// Methods
	void update();
	QPushButton *btn_connect;	
	
public slots:
	void updateConnectionStatus(bool);
		
private:
	// Variables
	bool iCubSimulator_accessible;

	// GUI
	QHBoxLayout *layout;
	QLabel *lbl_iCubSim_Status, *lbl_Connection_Status;

	
protected:
	// Methods
	void checkiCubSimulator();
	
};

#endif 