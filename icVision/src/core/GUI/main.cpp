// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <QApplication>
#include <cstdio>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "window.h"
#include "module_thread.h"
#include "../core_module.h"

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	QString version = "v0.5";
	QString title = "icVision GUI";
		
	Network yarp;

	printf("Launching icVision Core Module ...\n");
	
	if(argc > 1 && strcmp(argv[1], "--UItest") == 0) {
//		std::cout << argv[1] << strcmp(argv[1], "--UItest") << std::endl;
		std::cout << "TESTING the User Interface!! (no yarp!)" << std::endl;
		
		QApplication app(argc, argv);
		
		// Main Window
		Window window(title, version, NULL);
		window.setFocusPolicy(Qt::StrongFocus);
		window.setupSignalsAndSlots();
		window.show();
		
		app.exec();
	} else {
		// start the vision core module
		//icVision::CoreModule
		CoreModule *module = new CoreModule();
		ModuleThread ctrl_t(module, argc, argv); 
		ctrl_t.start();	

		QApplication app(argc, argv);
		
		// Main Window
		Window window(title, version, module);
		window.setFocusPolicy(Qt::StrongFocus);
		window.setupSignalsAndSlots();
		window.show();
		
		int ret = app.exec();	
		
		// cleanup
		ctrl_t.stop();

		return ret;
	}
	
	return 0;
}