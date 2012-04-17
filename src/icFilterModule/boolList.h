// Copyright: (C) 2011-2012 Juxi Leitner, Simon Harding
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _BOOLLIST_H
#define _BOOLLIST_H

class boolList {
private:
	bool values[7];
	
public:
	boolList() {
		reset();
	}
	
	~boolList() {}
	
	void Add (int v) { values[v] = true; } 
	
	bool uses(int v) { return values[v]; } 
	void reset(void) { for(int i = 0; i < 7; i++) values[i] = false; } 		
	
};

#endif