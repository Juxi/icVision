/*
 *  VisionRobot.h
 *  camview
 *
 *  Created by Alexander Förster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VisionRobot_ISDEF
#define VisionRobot_ISDEF
class VisionRobot {
public:
	int id;
	float x;
	float y;
	float orientation;
	int unseen;
};

#endif