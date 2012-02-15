// Copyright: (C) 2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ICVISION_CORE_MODULE_QT_H_
#define _ICVISION_CORE_MODULE_QT_H_

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;

class CoreModuleQT : public CoreModule
{
private:
//	// 
//	ConstString name;	
//	
//	class ModuleInfo {
//		private:
//			std::string name;
//			std::string shortName;
//			bool isRunning;
//		
//		public:
//			int ID;
//
//			void set(int in_ID, const Bottle &b) {
//				ID = in_ID;
//				for(int i = 0; i < b.size(); i++) {
//					switch(i) {
//						case 0: break; // "add"
//						case 1: name = b.get(1).asString(); break;
//					}
//				}
//			}
//			std::string toStdString() {
//				std::ostringstream oss;
//				oss << "Module Info: ";
//				oss << "ID=" << ID;		oss << "\t";
//				oss << "Name=" << name;
//				return oss.str();
//			}
//			void toBottle(Bottle &b) {
//				b.addInt(ID);
//				b.addString(name.c_str());
//				b.addString(shortName.c_str());
//			}
//	}; 
//	// list of started modules	
//	vector<ModuleInfo> listOfModules;
//	
//	int nextModuleID;
//	
signals:
	void moduleAdded(ModuleInfo &i);
	void moduleRemoved(ModuleInfo &i);

	
public:
	CoreModuleQT();
	virtual ~CoreModuleQT();
	
	virtual bool respond		(const Bottle& command, Bottle& reply);	
};

#endif
