// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "FirewireCamera2.h"

//#include <yarp/os/Property.h>
//#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/sig/all.h>

#include <stdio.h>
#include <iostream>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

void testCamera(PolyDriver& driver) {
	IFrameGrabber *fg;
	IFrameGrabberRgb *fgRgb;
	IFrameGrabberImage *fgImage;
	IFrameGrabberControls *fgCtrls;
	FirewireCamera2 *fwCam2;

	if (driver.view(fgCtrls)) {
		double blue, red;
		printf("  Brightness: %f\n", fgCtrls->getBrightness());
		printf("  Exposure:   %f\n", fgCtrls->getExposure());
		printf("  Sharpness:  %f\n", fgCtrls->getSharpness());
		fgCtrls->getWhiteBalance(blue, red);
		printf("  White bal   blue: %f  red: %f\n", blue, red);
		printf("  Hue:        %f\n", fgCtrls->getHue());
		printf("  Saturation: %f\n", fgCtrls->getSaturation());
		printf("  Gamma:      %f\n", fgCtrls->getGamma());
		printf("  Shutter:    %f\n", fgCtrls->getShutter());
		printf("  Gain:       %f\n", fgCtrls->getGain());
		printf("  Iris:       %f\n", fgCtrls->getIris());

		fgCtrls->setBrightness(0.05);
		fgCtrls->setExposure(0.10);
		fgCtrls->setSharpness(0.15);
		fgCtrls->setWhiteBalance(0.20, 0.25);
		fgCtrls->setHue(0.30);
		fgCtrls->setSaturation(0.35);
		fgCtrls->setGamma(0.40);
		fgCtrls->setShutter(0.45);
		fgCtrls->setGain(0.50);
		fgCtrls->setIris(0.55);

		printf("  Brightness: %f\n", fgCtrls->getBrightness());
		printf("  Exposure:   %f\n", fgCtrls->getExposure());
		printf("  Sharpness:  %f\n", fgCtrls->getSharpness());
		fgCtrls->getWhiteBalance(blue, red);
		printf("  White bal   blue: %f  red: %f\n", blue, red);
		printf("  Hue:        %f\n", fgCtrls->getHue());
		printf("  Saturation: %f\n", fgCtrls->getSaturation());
		printf("  Gamma:      %f\n", fgCtrls->getGamma());
		printf("  Shutter:    %f\n", fgCtrls->getShutter());
		printf("  Gain:       %f\n", fgCtrls->getGain());
		printf("  Iris:       %f\n", fgCtrls->getIris());

		fgCtrls->setBrightness(0.6);
		fgCtrls->setExposure(0.6);
		fgCtrls->setSharpness(0.6);
		fgCtrls->setWhiteBalance(0.6, 0.6);
		fgCtrls->setHue(0.6);
		fgCtrls->setSaturation(0.6);
		fgCtrls->setGamma(0.6);
		fgCtrls->setShutter(0.6);
		fgCtrls->setGain(0.6);
		fgCtrls->setIris(0.6);
	} else {
		printf("  could not find IFrameGrabberControls interface\n");
	}

	//	if (driver.view(fwCam2)) {
	//		printf("Camera has feature BRIGHTNESS: %d\n", fwCam2->hasFeatureDC1394(
	//				416));
	//		double brightness = fwCam2->getFeatureDC1394(416);
	//		printf("  brightness: %f\n", brightness);
	//		fwCam2->setFeatureDC1394(416, brightness + 1.0);
	//		brightness = fwCam2->getFeatureDC1394(416);
	//		printf("  brightness: %f\n", brightness);
	//
	//		fwCam2->PrintSettings();
	//	} else {
	//		printf("  could not find FirewireCamera2 interface\n");
	//	}
	//
	//	if (driver.view(fgImage)) {
	//		ImageOf<PixelRgb> img;
	//		fgImage->getImage(img);
	//		printf("Got a %dx%d image\n", img.width(), img.height());
	//	} else {
	//		printf("  could not find IFrameGrabberImage interface\n");
	//	}
}

int main(int argc, char *argv[]) {
	Drivers::factory().add(new DriverCreatorOf<FirewireCamera2> (
			"firewirecamera2", "framegrabber", "FirewireCamera2"));

	//	printf("============================================================\n");
	//	printf("check if our device can be instantiated directly\n");
	//
	//	Property directConfig;
	//	if (!directConfig.fromConfigFile("firewirecamera2.ini")) {
	//		std::cout << "Configuration file \"firewirecamera2.ini\" not found!!"
	//				<< std::endl;
	//		exit(1);
	//	}
	//	std::cout << directConfig.toString().c_str() << std::endl;
	//	std::cout << std::endl;
	//
	//	PolyDriver direct(directConfig);
	//	if (direct.isValid()) {
	//		printf("Direct instantiation worked\n");
	//		testCamera(direct);
	//		//		printf("Press a key to stop\n");
	//		//		printf("\n");
	//		//		scanf("c");
	//	} else {
	//		printf("Direct instantiation failed\n");
	//	}
	//	direct.close();
	//
	// check if our device can be wrapped in the grabber network wrapper
	printf("\n\n");
	printf("============================================================\n");
	printf("check if our device can be wrapped in a grabber\n");

	Property indirectConfig;
	if (!indirectConfig.fromConfigFile("indirectFirewirecamera2.ini", true)) {
		std::cout
				<< "Configuration file \"indirectFirewirecamera2.ini\" not found!!"
				<< std::endl;
		exit(1);
	}
	std::cout << indirectConfig.toString().c_str() << std::endl;
	std::cout << std::endl;

	PolyDriver indirect(indirectConfig);
	if (indirect.isValid()) {
		printf("Indirect instantiation worked\n");
		testCamera(indirect);
		while (true) {
			yarp::os::Time::delay(5);
		}
	} else {
		printf("Indirect instantiation failed\n");
	}
	indirect.close();

//		// check if our device can be wrapped in the grabber network wrapper
//		// and accessed remotely
//		printf("\n\n");
//		printf("============================================================\n");
//		printf("check if our device can be accessed via remote_grabber\n");
//
//		Property remoteCameraConfig;
//		if (!remoteCameraConfig.fromConfigFile("remoteFirewirecamera2.ini")) {
//			std::cout
//					<< "Configuration file \"remoteFirewirecamera2.ini\" not found!!"
//					<< std::endl;
//			exit(1);
//		}
//		std::cout << remoteCameraConfig.toString().c_str() << std::endl;
//		std::cout << std::endl;
//
//		PolyDriver server(remoteCameraConfig);
//		if (server.isValid()) {
//			printf("Server instantiation worked\n");
//			Property remoteClientConfig;
//			remoteClientConfig.put("device", "remote_grabber");
//			remoteClientConfig.put("local", "/grabber");
//			remoteClientConfig.put("remote", remoteCameraConfig.find("name"));
//			PolyDriver client(remoteClientConfig);
//			if (client.isValid()) {
//				printf("Client instantiation worked\n");
//
//				printf("\n");
//				printf("  yarpview /viewer\n");
//				printf("  yarp connect /guppy /viewer\n");
//				printf("\n");
//				printf("Press a key\n");
//				printf("\n");
//				scanf("c");
//
//				testCamera(client);
//
//				printf("Test over\n");
//				int i = 1;
//				while (true) {
//					IFrameGrabberImage *fgImage;
//					if (client.view(fgImage)) {
//						ImageOf<PixelRgb> img;
//						fgImage->getImage(img);
//						printf("Got a %dx%d image (%d)\n", img.width(), img.height(), i);
//						printf("  image depth: %d\n", img.getPixelSize());
//
//						unsigned char* raw = new unsigned char[img.getRawImageSize()];
//						raw = img.getRawImage();
//
//						int width = img.width(); // img.getRowSize();
//						int height = img.height();
//						int bytes = img.getRawImageSize();
//
//						FILE* imagefile=fopen("image.pbm", "wb");
//					    if( imagefile == NULL && i == 1) {
//					        perror( "Can't create file");
//					    } else {
//					    	fprintf(imagefile,"P6\n%u %u\n255\n", width, height);
//					    	fwrite(raw, 1, bytes, imagefile);
//					    	fclose(imagefile);
//					    	printf("wrote: %s \n","image.pbm");
//					    }
//						++i;
//
//						printf("    ");
//						for (int j = 0; j < img.getRawImageSize(); j++) {
//							printf("0x%X ", raw[j]);
//							if ((j+1)%32==0) {
//								printf("\n");
//							}
//						}
//						printf("\n");
//					} else {
//						printf("  could not find IFrameGrabberImage interface\n");
//						break;
//					}
//				}
//
//			} else {
//				printf("Client instantiation failed\n");
//			}
//			client.close();
//		}
//		server.close();

	printf("Test program finished\n");

	return 0;
}
