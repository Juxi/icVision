// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ICVISION_MODULE_INFO_H_
#define _ICVISION_MODULE_INFO_H_

#include "icFilterPort.h"

class CoreModule;

class ModuleInfo {
	friend class CoreModule;
private:
	std::string name;
	std::string shortName;
	std::string portname;
	bool isUsedInOutput;
	int ID;	
	icFilterPort *port;
	
public:
	ModuleInfo() {
		name = "Constructor"; shortName = "Cons"; portname = "constructor";
		isUsedInOutput = true;
		ID = -1;
		port = NULL;
	}
	
	void set(int in_ID, const Bottle &b) {
		ID = in_ID;
		for(int i = 0; i < b.size(); i++) {
			switch(i) {
				case 0: break; // "add"
				case 1: name = b.get(1).asString();
					portname = "/icVision/" + name + "/rawoutput:o";
					shortName = name.substr(0, 8);
					break;
			}
		}
	}
	std::string toStdString() {
		std::ostringstream oss;
		oss << "Module Info: ";
		oss << "ID=" << ID;		oss << "\t";
		oss << "Name=" << name;
		return oss.str();
	}
	void toBottle(Bottle &b) {
		b.addInt(ID);
		b.addString(name.c_str());
		b.addString(shortName.c_str());
	}
	void setPort(icFilterPort *p_in, const char *str) { port = p_in; portname = str;}
	
	std::string getName() { return name; }
	
}; 

#endif
