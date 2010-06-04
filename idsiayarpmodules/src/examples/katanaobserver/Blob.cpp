/*
 *  Blob.cpp
 *
 *  Created by Alexander Fšrster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Blob.h"
#include "utils.h"

Blob::Blob(int x, int y) {
	last_row_start_col = x;
	last_row_end_col = x;
	first_x = x;
	last_x = x;
	last_y = y;
	min_row = y;
	max_row = y;
	min_col = x;
	max_col = x;
	sum_x = x;
	sum_y = y;
	pixels = 1;
	needUpdate = true;
}

float Blob::getX() const {
	if (needUpdate)
		update();
	return avg_x;
}

float Blob::getY() const {
	if (needUpdate)
		update();
	return avg_y;
}

int Blob::getNPixels() const {
	return pixels;
}

void Blob::update() const {
	avg_x = (float) sum_x / pixels;
	avg_y = (float) sum_y / pixels;
	needUpdate = false;
}

void Blob::joinWithBlob(const Blob& blob) {
	needUpdate = true;
	if (max_row > blob.max_row) {
		last_row_start_col = minimum(last_row_start_col, blob.first_x);
		last_row_end_col = maximum(last_row_end_col, blob.last_x);
		//first_x;
		//last_x;
		//last_y;			
		min_row = minimum(min_row, blob.min_row);
		min_col = minimum(min_col, blob.min_col);
		max_col = maximum(max_col, blob.max_col);
		sum_x += blob.sum_x;
		sum_y += blob.sum_y;
		pixels += blob.pixels;
	} else if (max_row < blob.max_row) {
		last_row_start_col = minimum(blob.last_row_start_col, first_x);
		last_row_end_col = maximum(blob.last_row_end_col, last_x);
		first_x = blob.first_x;
		last_x = blob.last_x;
		last_y = blob.last_y;
		min_row = minimum(min_row, blob.min_row);
		min_col = minimum(min_col, blob.min_col);
		max_col = maximum(max_col, blob.max_col);
		sum_x += blob.sum_x;
		sum_y += blob.sum_y;
		pixels += blob.pixels;
	} else // (max_row==blob.max_row)
	{
		last_row_start_col = minimum(blob.last_row_start_col,
				last_row_start_col);
		last_row_end_col = maximum(blob.last_row_end_col, last_row_end_col);
		first_x = minimum(first_x, blob.first_x);
		last_x = maximum(blob.last_x, last_x);
		//last_y;			
		min_row = minimum(min_row, blob.min_row);
		min_col = minimum(min_col, blob.min_col);
		max_col = maximum(max_col, blob.max_col);
		sum_x += blob.sum_x;
		sum_y += blob.sum_y;
		pixels += blob.pixels;

	}
}

void Blob::addPixel(int x, int y) {
	needUpdate = true;
	if (y > max_row) {
		max_row = y;
		last_row_start_col = first_x;
		last_row_end_col = last_x;
		first_x = x;
	};
	if (x < min_col) {
		min_col = x;
	};
	if (x > max_col) {
		max_col = x;
	};
	sum_x += x;
	sum_y += y;
	last_x = x;
	last_y = y;
	pixels++;
}

bool Blob::isValid(const DetectionParams& param) const {
	return (pixels <= param.pixelsMax && pixels >= param.pixelsMin && max_col
			- min_col + 1 <= param.sizeMax && max_col - min_col + 1
			>= param.sizeMin && max_row - min_row + 1 <= param.sizeMax
			&& max_row - min_row + 1 >= param.sizeMin);
}
