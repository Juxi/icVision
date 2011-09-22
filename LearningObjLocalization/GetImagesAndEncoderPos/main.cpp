/*
 *  main.cpp
 *  Main class for the program
 *
 *  Copyright: Juxi Leitner <juxi.leitner@gmail.com>, 2011
 */

#include <QApplication>

#include "window.h"
#include "iCub_controller.h"

// TODO:

int main(int argc, char *argv[]) {
	QString version = "0.05";
	QString title = "LearningObjLocalization";

	QApplication app(argc, argv);

	QThread ctrl_t;

	// Objects
	iCubController ctrl;
	ctrl.moveToThread(&ctrl_t);
	ctrl_t.start();
	
	// Main Window
	Window window(title, version, &ctrl);
    window.setFocusPolicy(Qt::StrongFocus);
	window.setupSignalsAndSlots();
	window.show();
	
	int ret = app.exec();	

	// cleanup
	ctrl_t.exit();
	return ret;
}