#include "grabberthread.h"
#include <QTime>
#include "utils.h"
#include <cstdio>

// TODO: conversion.c/.h aus libdc1394 benutzen statt alles selbst machen!!

GrabberThread::GrabberThread(yarp::dev::PolyDriver& driver, QObject *parent) :
	QThread(parent), driver(driver), modifyImage(1) {

	if (driver.view(fgCtrls) == false)
		exit(1);
	if (driver.view(fgImage) == false)
		exit(1);

	//setImageType(STEREO); // Videre Design STOC coding
	setImageType(MONO); // normal camera
	bayerFilterType = RGGB; // AVT Guppy F-046C
	// bayerFilterType = GBRG; // Imaging Source DFK 21BF04-Z.H
	// bayerFilterType = GRBG; // Videre Design STOC-C

	//imageConverter=YUV422;
	//imageConverter=YUV411;
	//imageConverter=GRAY;
	//imageConverter=MASK;
	//imageConverter=SIMPLE;
	//imageConverter=SIMPLE2;
	imageConverter = LINEAR;
	framerate = 0.0;

	abort = false;
}

GrabberThread::~GrabberThread() {
	abort = true;
	wait();
}

void GrabberThread::setImageType(ImageType imageType) {
	//	if (imageType == STEREO) {
	//		modifyImage.acquire();
	//		image = QImage(fgImage->width() * 2, fgImage->height(),
	//				QImage::Format_RGB32);
	//		modifyImage.release();
	//	} else {
	modifyImage.acquire();
//	image = QImage(fgImage->width(), fgImage->height(), QImage::Format_RGB32);

	printf("@@@ image width: %d\n", fgImage->width());
	printf("@@@ image height: %d\n", fgImage->height());

	image = QImage(640, 480, QImage::Format_RGB32);
	modifyImage.release();
	//	};
	//	this->imageType = imageType;
}

//// TODO: YUV444
//void GrabberThread::convertYUV422ToGray(unsigned char *frameimage,
//		unsigned int offset) {
//	int width = fgImage->width(), height = fgImage->height();
//	int i = 0;
//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width; ++x) {
//			unsigned int rgbvalue;
//#if 1
//			int yy = frameimage[i + 1];
//#else
//			int yy = frameimage[i];
//#endif
//			rgbvalue = 0xFF000000 | yy << 16 | yy << 8 | yy;
//			image.setPixel(x + offset, y, rgbvalue);
//			i += 2;
//		}
//	}
//}
//
//void GrabberThread::convertYUV422ToRGB(unsigned char *frameimage,
//		unsigned int offset) {
//	int width = fgImage->width(), height = fgImage->height();
//	int i = 0;
//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width; x += 2) {
//			unsigned int rgbvalue;
//			int red = 0;
//			int green = 0;
//			int blue = 0;
//#if 1
//			int u = frameimage[i] - 128;
//			int y1 = frameimage[i + 1];
//			int v = frameimage[i + 2] - 128;
//			int y2 = frameimage[i + 3];
//#else
//			int y1 = frameimage[i];
//			int u = frameimage[i+1]-128;
//			int y2 = frameimage[i+2];
//			int v = frameimage[i+3]-128;
//#endif
//			yuv2rgb(y1, u, v, red, green, blue);
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + offset, y, rgbvalue);
//			yuv2rgb(y2, u, v, red, green, blue);
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + 1 + offset, y, rgbvalue);
//			i += 4;
//		}
//	}
//}
//
//void GrabberThread::convertYUV411ToGray(unsigned char *frameimage,
//		unsigned int offset) {
//	int width = fgImage->width(), height = fgImage->height();
//	int i = 0;
//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width - 3; x += 4) {
//			unsigned int rgbvalue;
//			int y1 = frameimage[i + 1];
//			int y2 = frameimage[i + 2];
//			int y3 = frameimage[i + 4];
//			int y4 = frameimage[i + 5];
//			rgbvalue = 0xFF000000 | y1 << 16 | y1 << 8 | y1;
//			image.setPixel(x + offset, y, rgbvalue);
//			rgbvalue = 0xFF000000 | y2 << 16 | y2 << 8 | y2;
//			image.setPixel(x + 1 + offset, y, rgbvalue);
//			rgbvalue = 0xFF000000 | y3 << 16 | y3 << 8 | y3;
//			image.setPixel(x + 2 + offset, y, rgbvalue);
//			rgbvalue = 0xFF000000 | y4 << 16 | y4 << 8 | y4;
//			image.setPixel(x + 3 + offset, y, rgbvalue);
//			i += 6;
//		}
//	}
//}
//
//void GrabberThread::convertYUV411ToRGB(unsigned char *frameimage,
//		unsigned int offset) {
//	int width = fgImage->width(), height = fgImage->height();
//	int i = 0;
//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width - 3; x += 4) {
//			unsigned int rgbvalue;
//			int red = 0;
//			int green = 0;
//			int blue = 0;
//			// Extract YUV components
//			int u = frameimage[i] - 128;
//			int y1 = frameimage[i + 1];
//			int y2 = frameimage[i + 2];
//			int v = frameimage[i + 3] - 128;
//			int y3 = frameimage[i + 4];
//			int y4 = frameimage[i + 5];
//			yuv2rgb(y1, u, v, red, green, blue);
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + offset, y, rgbvalue);
//			yuv2rgb(y2, u, v, red, green, blue);
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + 1 + offset, y, rgbvalue);
//			yuv2rgb(y3, u, v, red, green, blue);
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + 2 + offset, y, rgbvalue);
//			yuv2rgb(y4, u, v, red, green, blue);
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + 3 + offset, y, rgbvalue);
//			i += 6;
//		}
//	}
//}
//
//void GrabberThread::convertGray(unsigned char *frameimage, unsigned int offset) {
//	int width = fgImage->width(), height = fgImage->height();
//	int i = 0;
//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width; ++x) {
//			unsigned char value = frameimage[i];
//			unsigned int rgbvalue = 0xFF000000 | value << 16 | value << 8
//					| value;
//			image.setPixel(x + offset, y, rgbvalue);
//			++i;
//		}
//	}
//}
//
///* Bayer paterns starting at offset (0,0) the modes are defined by the four upper left pixel colors:
// * GRBG for
// * G R G R ...
// * B G B G
// * G R G R
// * .       .
// * .        .
// * .         .
// * RGGB for
// * R G R G ...
// * G B G B
// * R G R G
// * .       .
// * .        .
// * .         .
// * and so on.
// * The positions are coded as the x and y offset modulo 2
// * 0 1
// * 2 3
// */
//void GrabberThread::convertMask(unsigned char *frameimage, unsigned int offset) {
//	int width = fgImage->width(), height = fgImage->height();
//	int i = 0;
//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width; ++x) {
//			unsigned char value = frameimage[i];
//			unsigned int rgbvalue;
//			unsigned int position = -1;
//			if (x % 2 == 0 && y % 2 == 0)
//				position = 0;
//			if (x % 2 == 1 && y % 2 == 0)
//				position = 1;
//			if (x % 2 == 0 && y % 2 == 1)
//				position = 2;
//			if (x % 2 == 1 && y % 2 == 1)
//				position = 3;
//			unsigned int red = 0;
//			unsigned int green = 0;
//			unsigned int blue = 0;
//			switch (bayerFilterType) {
//			case GRBG:
//				switch (position) {
//				case 0:
//					green = value;
//					break;
//				case 1:
//					red = value;
//					break;
//				case 2:
//					blue = value;
//					break;
//				case 3:
//					green = value;
//					break;
//				}
//				;
//				break;
//			case RGGB:
//				switch (position) {
//				case 0:
//					red = value;
//					break;
//				case 1:
//					green = value;
//					break;
//				case 2:
//					green = value;
//					break;
//				case 3:
//					blue = value;
//					break;
//				}
//				;
//				break;
//			case BGGR:
//				switch (position) {
//				case 0:
//					blue = value;
//					break;
//				case 1:
//					green = value;
//					break;
//				case 2:
//					green = value;
//					break;
//				case 3:
//					red = value;
//					break;
//				}
//				;
//				break;
//			case GBRG:
//				switch (position) {
//				case 0:
//					green = value;
//					break;
//				case 1:
//					blue = value;
//					break;
//				case 2:
//					red = value;
//					break;
//				case 3:
//					green = value;
//					break;
//				}
//				;
//				break;
//			};
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + offset, y, rgbvalue);
//			++i;
//		}
//	}
//}
///* the pixels values around the current pixel (value) are labeled like the folowing number matrix:
// *   1
// * 2 3
// */
//void GrabberThread::convertSimple(unsigned char *frameimage,
//		unsigned int offset) {
//	int width = cfgImage->width(), height = fgImage->height();
//	int i = 0;
//	for (int y = 0; y < height - 1; ++y) {
//		for (int x = 0; x < width - 1; ++x) {
//			unsigned char value = frameimage[i];
//			unsigned char value1 = frameimage[i + 1];
//			unsigned char value2 = frameimage[i + width];
//			unsigned char value3 = frameimage[i + width + 1];
//
//			unsigned int rgbvalue;
//			unsigned int position = -1;
//			if (x % 2 == 0 && y % 2 == 0)
//				position = 0;
//			if (x % 2 == 1 && y % 2 == 0)
//				position = 1;
//			if (x % 2 == 0 && y % 2 == 1)
//				position = 2;
//			if (x % 2 == 1 && y % 2 == 1)
//				position = 3;
//			unsigned int red = 0;
//			unsigned int green = 0;
//			unsigned int blue = 0;
//			switch (bayerFilterType) {
//			case GRBG: /*not tested*/
//				switch (position) {
//				case 0:
//					red = value1;
//					green = (value + value3) >> 1;
//					blue = value2;
//					break;
//				case 1:
//					red = value;
//					green = (value1 + value2) >> 1;
//					blue = value3;
//					break;
//				case 2:
//					red = value3;
//					green = (value1 + value2) >> 1;
//					blue = value;
//					break;
//				case 3:
//					red = value2;
//					green = (value + value3) >> 1;
//					blue = value1;
//					break;
//				}
//				;
//				break;
//			case RGGB:
//				switch (position) {
//				case 1:
//					red = value1;
//					green = (value + value3) >> 1;
//					blue = value2;
//					break;
//				case 0:
//					red = value;
//					green = (value1 + value2) >> 1;
//					blue = value3;
//					break;
//				case 3:
//					red = value3;
//					green = (value1 + value2) >> 1;
//					blue = value;
//					break;
//				case 2:
//					red = value2;
//					green = (value + value3) >> 1;
//					blue = value1;
//					break;
//				}
//				;
//				break;
//			case BGGR: /*not tested*/
//				switch (position) {
//				case 2:
//					red = value1;
//					green = (value + value3) >> 1;
//					blue = value2;
//					break;
//				case 3:
//					red = value;
//					green = (value1 + value2) >> 1;
//					blue = value3;
//					break;
//				case 0:
//					red = value3;
//					green = (value1 + value2) >> 1;
//					blue = value;
//					break;
//				case 1:
//					red = value2;
//					green = (value + value3) >> 1;
//					blue = value1;
//					break;
//				}
//				;
//				break;
//			case GBRG: /*not tested*/
//				switch (position) {
//				case 3:
//					red = value1;
//					green = (value + value3) >> 1;
//					blue = value2;
//					break;
//				case 2:
//					red = value;
//					green = (value1 + value2) >> 1;
//					blue = value3;
//					break;
//				case 1:
//					red = value3;
//					green = (value1 + value2) >> 1;
//					blue = value;
//					break;
//				case 0:
//					red = value2;
//					green = (value + value3) >> 1;
//					blue = value1;
//					break;
//				}
//				;
//				break;
//			};
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + offset, y, rgbvalue);
//			++i;
//		}
//		++i;
//	}
//}
//
//void GrabberThread::convertSimple2(unsigned char *frameimage,
//		unsigned int offset) {
//	int width = fgImage->width(), height = fgImage->height();
//	int i = 0;
//	for (int y = 0; y < height - 1; ++y) {
//		for (int x = 0; x < width - 1; ++x) {
//			unsigned char value = frameimage[i];
//			unsigned char value1 = frameimage[i + 1];
//			unsigned char value2 = frameimage[i + width];
//			unsigned char value3 = frameimage[i + width + 1];
//
//			unsigned int rgbvalue;
//			unsigned int position = -1;
//			if (x % 2 == 0 && y % 2 == 0)
//				position = 0;
//			if (x % 2 == 1 && y % 2 == 0)
//				position = 1;
//			if (x % 2 == 0 && y % 2 == 1)
//				position = 2;
//			if (x % 2 == 1 && y % 2 == 1)
//				position = 3;
//			unsigned int red = 0;
//			unsigned int green = 0;
//			unsigned int blue = 0;
//			switch (bayerFilterType) {
//			case GRBG: /*not tested*/
//				switch (position) {
//				case 0:
//					red = value1;
//					green = value;
//					blue = value2;
//					break;
//				case 1:
//					red = value;
//					green = value1;
//					blue = value3;
//					break;
//				case 2:
//					red = value3;
//					green = value1;
//					blue = value;
//					break;
//				case 3:
//					red = value2;
//					green = value;
//					blue = value1;
//					break;
//				}
//				;
//				break;
//			case RGGB:
//				switch (position) {
//				case 1:
//					red = value1;
//					green = value;
//					blue = value2;
//					break;
//				case 0:
//					red = value;
//					green = value1;
//					blue = value3;
//					break;
//				case 3:
//					red = value3;
//					green = value1;
//					blue = value;
//					break;
//				case 2:
//					red = value2;
//					green = value;
//					blue = value1;
//					break;
//				}
//				;
//				break;
//			case BGGR: /*not tested*/
//				switch (position) {
//				case 2:
//					red = value1;
//					green = value;
//					blue = value2;
//					break;
//				case 3:
//					red = value;
//					green = value1;
//					blue = value3;
//					break;
//				case 0:
//					red = value3;
//					green = value1;
//					blue = value;
//					break;
//				case 1:
//					red = value2;
//					green = value;
//					blue = value1;
//					break;
//				}
//				;
//				break;
//			case GBRG: /*not tested*/
//				switch (position) {
//				case 3:
//					red = value1;
//					green = value;
//					blue = value2;
//					break;
//				case 2:
//					red = value;
//					green = value1;
//					blue = value;
//					break;
//				case 1:
//					red = value3;
//					green = value1;
//					blue = value;
//					break;
//				case 0:
//					red = value2;
//					green = value;
//					blue = value1;
//					break;
//				}
//				;
//				break;
//			};
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + offset, y, rgbvalue);
//			++i;
//		}
//		++i;
//	}
//}
//
///* the pixels values around the current pixel (value4) are labeled like the folowing number matrix:
// * 0 1 2
// * 3 4 5
// * 6 7 8
// */
//void GrabberThread::convertLinear(unsigned char *frameimage,
//		unsigned int offset) {
//	int width = fgImage->width(), height = fgImage->height();
//	int i = width + 1;
//	for (int y = 1; y < height - 1; ++y) {
//		for (int x = 1; x < width - 1; ++x) {
//			unsigned char value0 = frameimage[i - width - 1];
//			unsigned char value1 = frameimage[i - width];
//			unsigned char value2 = frameimage[i - width + 1];
//			unsigned char value3 = frameimage[i - 1];
//			unsigned char value4 = frameimage[i];
//			unsigned char value5 = frameimage[i + 1];
//			unsigned char value6 = frameimage[i + width - 1];
//			unsigned char value7 = frameimage[i + width];
//			unsigned char value8 = frameimage[i + width + 1];
//
//			unsigned int rgbvalue;
//			unsigned int position = -1;
//			if (x % 2 == 0 && y % 2 == 0)
//				position = 0;
//			if (x % 2 == 1 && y % 2 == 0)
//				position = 1;
//			if (x % 2 == 0 && y % 2 == 1)
//				position = 2;
//			if (x % 2 == 1 && y % 2 == 1)
//				position = 3;
//			unsigned int red = 0;
//			unsigned int green = 0;
//			unsigned int blue = 0;
//			switch (bayerFilterType) {
//			case GRBG: //0123
//				switch (position) {
//				case 0:
//					red = (value3 + value5) >> 1;
//					green = value4;
//					blue = (value1 + value7) >> 1;
//					break;
//				case 1:
//					red = value4;
//					green = (value1 + value3 + value5 + value7) >> 2;
//					blue = (value0 + value2 + value6 + value8) >> 2;
//					break;
//				case 2:
//					red = (value0 + value2 + value6 + value8) >> 2;
//					green = (value1 + value3 + value5 + value7) >> 2;
//					blue = value4;
//					break;
//				case 3:
//					red = (value1 + value7) >> 1;
//					green = value4;
//					blue = (value3 + value5) >> 1;
//					break;
//				}
//				;
//				break;
//			case RGGB: //1032
//				switch (position) {
//				case 1:
//					red = (value3 + value5) >> 1;
//					green = value4;
//					blue = (value1 + value7) >> 1;
//					break;
//				case 0:
//					red = value4;
//					green = (value1 + value3 + value5 + value7) >> 2;
//					blue = (value0 + value2 + value6 + value8) >> 2;
//					break;
//				case 3:
//					red = (value0 + value2 + value6 + value8) >> 2;
//					green = (value1 + value3 + value5 + value7) >> 2;
//					blue = value4;
//					break;
//				case 2:
//					red = (value1 + value7) >> 1;
//					green = value4;
//					blue = (value3 + value5) >> 1;
//					break;
//				}
//				;
//				break;
//			case BGGR: //2301
//				switch (position) {
//				case 2:
//					red = (value3 + value5) >> 1;
//					green = value4;
//					blue = (value1 + value7) >> 1;
//					break;
//				case 3:
//					red = value4;
//					green = (value1 + value3 + value5 + value7) >> 2;
//					blue = (value0 + value2 + value6 + value8) >> 2;
//					break;
//				case 0:
//					red = (value0 + value2 + value6 + value8) >> 2;
//					green = (value1 + value3 + value5 + value7) >> 2;
//					blue = value4;
//					break;
//				case 1:
//					red = (value1 + value7) >> 1;
//					green = value4;
//					blue = (value3 + value5) >> 1;
//					break;
//				}
//				;
//				break;
//			case GBRG: //3210
//				switch (position) {
//				case 3:
//					red = (value3 + value5) >> 1;
//					green = value4;
//					blue = (value1 + value7) >> 1;
//					break;
//				case 2:
//					red = value4;
//					green = (value1 + value3 + value5 + value7) >> 2;
//					blue = (value0 + value2 + value6 + value8) >> 2;
//					break;
//				case 1:
//					red = (value0 + value2 + value6 + value8) >> 2;
//					green = (value1 + value3 + value5 + value7) >> 2;
//					blue = value4;
//					break;
//				case 0:
//					red = (value1 + value7) >> 1;
//					green = value4;
//					blue = (value3 + value5) >> 1;
//					break;
//				}
//				;
//				break;
//			};
//			rgbvalue = 0xFF000000 | red << 16 | green << 8 | blue;
//			image.setPixel(x + offset, y, rgbvalue);
//			++i;
//		}
//		i += 2;
//	}
//}

void GrabberThread::run() {
	//	unsigned char *fimage1 = new unsigned char[fgImage->width()
	//			* fgImage->height()];
	//	unsigned char *fimage2 = new unsigned char[fgImage->width()
	//			* fgImage->height()];
	unsigned int rgbvalue;
	yarp::sig::ImageOf<yarp::sig::PixelRgb> img;

	QTime time;
	time.start();

	while (!abort) {
//		try {
			time.restart();

			fgImage->getImage(img);
			unsigned char* raw; // = new unsigned char[img.getRawImageSize()];
			raw = img.getRawImage();

			modifyImage.acquire();

//			printf("image width: %d, height: %d\n", image.width(), image.height());

			for (int row = 0; row < img.height(); ++row) {
				for (int col = 0; col < img.width(); ++col) {
//					rgbvalue = 0xFF0000FF;
					rgbvalue = 0xFF000000 | img.pixel(col,row).r << 16 | img.pixel(col,row).g << 8 | img.pixel(col,row).b;
					image.setPixel(col, row, rgbvalue);
				}
			}

			//			dc1394video_frame_t* frame = camera.captureFrame();

			//			if (imageType == STEREO) {
			//				// convert3D(frame);
			//				int width = camera.getImageWidth(), height =
			//						camera.getImageHeight();
			//				if (camera.getImageDepth() == 16) {
			//					int i = 0;
			//					for (int y = 0; y < height; ++y) {
			//						for (int x = 0; x < width; ++x) {
			//							unsigned char valueR = frame->image[i];
			//							unsigned char valueL = frame->image[i + 1];
			//							fimage1[x + y * width] = valueL;
			//							fimage2[x + y * width] = valueR;
			//							i += 2;
			//						}
			//					};
			//				} else // we copy the frames
			//				{
			//					int i = 0;
			//					for (int y = 0; y < height; ++y) {
			//						for (int x = 0; x < width; ++x) {
			//							unsigned char valueR = frame->image[i];
			//							unsigned char valueL = frame->image[i];
			//							fimage1[x + y * width] = valueL;
			//							fimage2[x + y * width] = valueR;
			//							i += 1;
			//						}
			//					};
			//				};
			//				switch (imageConverter) {
			//				case YUV422: // TODO; The same check as for mono
			//					convertYUV422ToRGB(fimage1);
			//					convertYUV422ToRGB(fimage2, width);
			//					break;
			//				case YUV411:
			//					convertYUV411ToRGB(fimage1);
			//					convertYUV411ToRGB(fimage2, width);
			//					break;
			//				case GRAY:
			//					convertGray(fimage1);
			//					convertGray(fimage2, width);
			//					break;
			//				case MASK:
			//					convertMask(fimage1);
			//					convertMask(fimage2, width);
			//					break;
			//				case SIMPLE:
			//					convertSimple(fimage1);
			//					convertSimple(fimage2, width);
			//					break;
			//				case SIMPLE2:
			//					convertSimple2(fimage1);
			//					convertSimple2(fimage2, width);
			//					break;
			//				case LINEAR:
			//					convertLinear(fimage1);
			//					convertLinear(fimage2, width);
			//					break;
			//				};
			//			} else // MONO
			//			{
			//				uint32_t nbytes = frame->image_bytes;
			//				unsigned int width = camera.getImageWidth(), height =
			//						camera.getImageHeight();
			//				switch (imageConverter) {
			//				case YUV422:
			//					if (nbytes == (width * height * 4) / 2) // TODO: or check for 16 bit depth
			//					{
			//						convertYUV422ToRGB(frame->image);
			//						//							convertYUV422ToGray(frame->image); // TODO: modify the menu structue (auto, Raw, mono8, yuv422,yuv411, rgb16,rgb8,...) and (gray,mask,...)
			//					}
			//					;
			//					break;
			//				case YUV411:
			//					if (nbytes == (width * height * 6) / 4) // TODO: or check for 12 bit depth
			//					{
			//						convertYUV411ToRGB(frame->image);
			//						//							convertYUV411ToGray(frame->image);
			//					}
			//					;
			//					break;
			//				case GRAY:
			//					convertGray(frame->image);
			//					break;
			//				case MASK:
			//					convertMask(frame->image);
			//					break;
			//				case SIMPLE:
			//					convertSimple(frame->image);
			//					break;
			//				case SIMPLE2:
			//					convertSimple2(frame->image);
			//					break;
			//				case LINEAR:
			//					convertLinear(frame->image);
			//					break;
			//				};
			//			};
			//camera.saveAs(frame,"ImageRGB.ppm");
			//			camera.releaseFrame(frame);
			emit newFrame(image);
			modifyImage.release();
			int miliseconds = time.elapsed();
//			delete [] raw;

			framerate = 1000.0 / miliseconds;

			msleep(5);

//		} catch (CameraError ce) {
//			printf("Camera Error occured!\n");
//		};

	}
	//	camera.stopTransmission();
	//	delete[] fimage1;
	//	delete[] fimage2;
}
