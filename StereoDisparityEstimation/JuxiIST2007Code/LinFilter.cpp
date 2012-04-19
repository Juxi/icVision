/* LinFilter.cpp
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-12
 * Class declaration file for the LinFilter class used in the DisparityMapper class to filter
 * images produced in the neighborhood step with a simple linear filter.
 */

#include "stdafx.h"

// the main function, it applies the filter to the filteredLikelihoodImage of the current index
void LinFilter::applyFilter(int i) {
	IplImage *img = mapper->getFilteredLikelihoodImages()[i];
	// TODO: border effects?!?!?
	
	// forward filtering for all pixels
	for(int y=0;y < img->height;y++) {
		for(int x=0;x < img->width;x++) {
		
			// be carefull with the indices
			if(x+1 < img->width) {
				// horizontal filtering
				if(img->depth == 32) {
					float *pixel = &((float*)(img->imageData + img->widthStep * y))[x * img->nChannels];
					float *nextpixel = &((float*)(img->imageData + img->widthStep * y))[(x+1) * img->nChannels];
					for(int i=0;i < img->nChannels;i++) {
						nextpixel[i] = (float)(pixel[i]*(1-m_filterWeight) + nextpixel[i]*m_filterWeight);
					}
				}else if(img->depth == 8) {
					uchar *pixel = &((uchar*)(img->imageData + img->widthStep * y))[x * img->nChannels];
					uchar *nextpixel = &((uchar*)(img->imageData + img->widthStep * y))[(x+1) * img->nChannels];
					for(int i=0;i < img->nChannels;i++)
						nextpixel[i] = (uchar)(nextpixel[i]*m_filterWeight + pixel[i]*(1-m_filterWeight));
				}
			}

			// be carefull with the indices
			if(y+1 < img->height) {
				// vertical filtering
				if(img->depth == 32) {
					float *pixel = &((float*)(img->imageData + img->widthStep * y))[x * img->nChannels];
					float *nextpixel = &((float*)(img->imageData + img->widthStep * (y+1)))[x * img->nChannels];
					for(int i=0;i < img->nChannels;i++) {
						nextpixel[i] = (float)(pixel[i]*(1-m_filterWeight) + nextpixel[i]*m_filterWeight);
					}
				}else if(img->depth == 8) {
					uchar *pixel = &((uchar*)(img->imageData + img->widthStep * y))[x * img->nChannels];
					uchar *nextpixel = &((uchar*)(img->imageData + img->widthStep * (y+1)))[x * img->nChannels];
					for(int i=0;i < img->nChannels;i++)
						nextpixel[i] = (uchar)(nextpixel[i]*m_filterWeight + pixel[i]*(1-m_filterWeight));
				}
			}
		}
	}

	// backward filtering for all pixels
	for(int y=img->height-1;y >= 0;y--) {
		for(int x=img->width-1;x >= 0;x--) {

			// be carefull with the indices
			if(y-1 > 0) {
				// vertical filtering
				if(img->depth == 32) {
					float *pixel = &((float*)(img->imageData + img->widthStep * y))[x * img->nChannels];
					float *prevpixel = &((float*)(img->imageData + img->widthStep * (y-1)))[x * img->nChannels];
					for(int i=0;i < img->nChannels;i++)
						prevpixel[i] = (float)(prevpixel[i]*m_filterWeight + pixel[i]*(1-m_filterWeight));
				}else if(img->depth == 8) {
					uchar *pixel = &((uchar*)(img->imageData + img->widthStep * y))[x * img->nChannels];
					uchar *prevpixel = &((uchar*)(img->imageData + img->widthStep * (y-1)))[x * img->nChannels];
					for(int i=0;i < img->nChannels;i++)
						prevpixel[i] = (uchar)(prevpixel[i]*m_filterWeight + pixel[i]*(1-m_filterWeight));
				}
			}

			// be carefull with the indices
			if(x-1 > 0) {
				// horizontal filtering
				if(img->depth == 32) {
					float *pixel = &((float*)(img->imageData + img->widthStep * y))[x * img->nChannels];
					float *prevpixel = &((float*)(img->imageData + img->widthStep * y))[(x-1) * img->nChannels];
					for(int i=0;i < img->nChannels;i++)
						prevpixel[i] = (float)(prevpixel[i]*m_filterWeight + pixel[i]*(1-m_filterWeight));
				}else if(img->depth == 8) {
					uchar *pixel = &((uchar*)(img->imageData + img->widthStep * y))[x * img->nChannels];
					uchar *prevpixel = &((uchar*)(img->imageData + img->widthStep * y))[(x-1) * img->nChannels];
					for(int i=0;i < img->nChannels;i++)
						prevpixel[i] = (uchar)(prevpixel[i]*m_filterWeight + pixel[i]*(1-m_filterWeight));
				}
			}
		}
	}
}