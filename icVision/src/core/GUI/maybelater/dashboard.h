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

class StatusWidget;

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
	void addWidget(QWidget* w);	

	QPushButton *btn_connect;	
	QPushButton *btn_initialize;
	
public slots:
	void updateConnectionStatus(bool);
		
private:
	// Variables
	bool iCubSimulator_accessible;
	std::string robotName;

	// GUI
	QWidget *vwidget;
	QVBoxLayout *layout;
	QVBoxLayout *vlayout;
	
	StatusWidget *status_yarp;
	StatusWidget *status_Robot;	
	StatusWidget *status_Cameras;	
	StatusWidget *status_Connection;
	StatusWidget *status_VisionModule;	

	
protected:
	// Methods
	void checkiCubSimulator();

	
};

class StatusWidget : public QWidget {
private:
	std::string name; 
	
	QHBoxLayout *layout;
	
	QLabel *lbl_Indicator;
	QLabel *lbl_Name;
	QLabel *lbl_StatusMsg;	
	
	void init() {
		layout = new QHBoxLayout();
		
		lbl_Indicator = new QLabel();
		lbl_Indicator->setMinimumSize(15, 15);
		lbl_Indicator->setMaximumSize(15, 15);	
		lbl_Indicator->setStyleSheet("background-color: red;");
		
		std::string s = "<b>" + name + " </b>";
		lbl_Name = new QLabel(s.c_str());
		lbl_Name->setMinimumSize(80, 15);
		lbl_Name->setMaximumHeight(15);	
		
		lbl_StatusMsg = new QLabel();
		lbl_StatusMsg->setStyleSheet("font-size: 10pt");
		lbl_StatusMsg->setAlignment(Qt::AlignBottom);
		lbl_StatusMsg->setMinimumSize(80, 15);
		lbl_StatusMsg->setMaximumHeight(15);	
		
		layout->addWidget(lbl_Indicator);
		layout->addWidget(lbl_Name);	
		layout->addWidget(lbl_StatusMsg);	
		
		layout->setMargin(2);
		
		this->setLayout(layout);
		this->show();
	}
	
public:
	StatusWidget() {}
	StatusWidget(const char *s) {
		this->name = s;
		init();
	}
	~StatusWidget() {
		delete layout;
	}
	
	void setStatus(bool b, const char* msg = NULL) {
		if( b ) {
			lbl_Indicator->setStyleSheet("background-color: green;");
		} else {
			lbl_Indicator->setStyleSheet("background-color: red;");
		}
		
		if( msg ) {
			lbl_StatusMsg->setText(msg);
		} else {
			lbl_StatusMsg->setText("");
		}
		
	}
};

#endif 