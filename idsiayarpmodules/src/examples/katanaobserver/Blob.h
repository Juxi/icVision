/*
 *  Blob.h
 *
 *  Created by Alexander Fšrster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Blob_ISDEF
#define Blob_ISDEF

#include "DetectionParams.h"

class Blob {
public:
	int last_row_start_col;
	int last_row_end_col;
	int min_row;
	int max_row;
	int min_col;
	int max_col;
	int last_x;
	int last_y;
	int first_x;
	Blob(int x, int y);
	void joinWithBlob(const Blob& blob);
	void addPixel(int x, int y);
	bool isValid(const DetectionParams& param) const;
	float getX() const;
	float getY() const;
	int getNPixels() const;
private:
	int sum_x;
	int sum_y;
	int pixels;
protected:
	void update() const;
	mutable bool needUpdate;
	mutable float avg_x;
	mutable float avg_y;
};

#endif
