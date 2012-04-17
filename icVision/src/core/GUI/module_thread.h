// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _MODULE_THREAD_H_
#define _MODULE_THREAD_H_

#include <yarp/os/all.h>

class ModuleThread : public yarp::os::Thread {
private:
	CoreModule *module;
	int argc;
	char **argv;
	
public:
	ModuleThread() {}
	ModuleThread(CoreModule *m, int argc, char * argv[]) {
		this->module = m;
		this->argc = argc;
		this->argv = argv;
	}

	~ModuleThread() {}
	
	void run() {
		// run the module: runModule() calls configure first and, if successful, it then runs		
		module->runModule(argc, argv);
	}
	
	void onStop() {
		std::cout << "in the onStop of the module thread..." <<std::endl;
		module->interruptModule();		
		module->close();
	}
		
};


#endif