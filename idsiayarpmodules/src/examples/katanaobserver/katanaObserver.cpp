#include <iostream>

#include <yarp/os/Property.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/ImageDraw.h>

#include "RobotFilter.h"
#include "DetectionParams.h"
#include "Blob.h"
#include "utils.h"

using namespace std;
using namespace yarp::os;

DetectionParams gripperParams;
DetectionParams targetParams;

void initDetectionParams();
void detectBlobs(yarp::sig::ImageOf<yarp::sig::PixelRgb> & img);

int main(int argc, char **argv) {
	yarp::os::Property params;
	params.fromCommand(argc, argv);

	if (!params.check("file")) {
		fprintf(stderr, "Please specify the name of a configuration file\n");
		fprintf(stderr, "--file name (e.g. ./observerconfig.ini)\n");
		return -1;
	}
	std::string configFileName = params.find("file").asString().c_str();

	params.fromConfigFile(configFileName.c_str(), false);

	// Grabber name
	if (!params.check("grabber")) {
		fprintf(stderr,
				"Please specify the YARP name of the remote grabber (~ camera)\n");
		fprintf(stderr, "--grabber name (e.g. /guppy1)\n");
		return -1;
	}
	std::string grabberName = params.find("grabber").asString().c_str();

	// Brightness
	if (!params.check("brightness")) {
		fprintf(stderr,
				"Please specify a value for brightness in the range [0.0, 1.0]\n");
		return -1;
	}
	double brightness = params.find("brightness").asDouble();

	// Exposure
	if (!params.check("exposure")) {
		fprintf(stderr,
				"Please specify a value for exposure in the range [0.0, 1.0]\n");
		return -1;
	}
	double exposure = params.find("exposure").asDouble();

	// Whitebalance1
	if (!params.check("whitebalance1")) {
		fprintf(stderr,
				"Please specify a value for whitebalance1 in the range [0.0, 1.0]\n");
		return -1;
	}
	double whitebalance1 = params.find("whitebalance1").asDouble();

	// Whitebalance2
	if (!params.check("whitebalance2")) {
		fprintf(stderr,
				"Please specify a value for whitebalance2 in the range [0.0, 1.0]\n");
		return -1;
	}
	double whitebalance2 = params.find("whitebalance2").asDouble();

	// Gamma
	if (!params.check("gamma")) {
		fprintf(stderr,
				"Please specify a value for gamma in the range [0.0, 1.0]\n");
		return -1;
	}
	double gamma = params.find("gamma").asDouble();

	// Shutter
	if (!params.check("shutter")) {
		fprintf(stderr,
				"Please specify a value for shutter in the range [0.0, 1.0]\n");
		return -1;
	}
	double shutter = params.find("shutter").asDouble();

	// Gain
	if (!params.check("gain")) {
		fprintf(stderr,
				"Please specify a value for gain in the range [0.0, 1.0]\n");
		return -1;
	}
	double gain = params.find("gain").asDouble();

	// initialize the parameters for gripper and target detection
	initDetectionParams();

	// create the RobotFilter
	RobotInterface ri;
	RobotFilter rf;

	// add a remote frame grabber
	yarp::dev::IFrameGrabberControls *fgCtrls;
	yarp::dev::IFrameGrabberImage *fgImage;
	Property remoteFgConfig;

	// check whether the specified framegrabber is available


	remoteFgConfig.put("device", "remote_grabber");
	remoteFgConfig.put("local", "/localgrabber");
	remoteFgConfig.put("remote", grabberName.c_str());
	yarp::dev::PolyDriver remoteFg(remoteFgConfig);
	if (remoteFg.isValid() && remoteFg.view(fgCtrls) && remoteFg.view(fgImage)) {
		printf("Remote framegrabber device successfully created\n");

		// setting some usefull camera settings
		fgCtrls->setBrightness(brightness);
		fgCtrls->setExposure(exposure);
		fgCtrls->setWhiteBalance(whitebalance1, whitebalance2);
		fgCtrls->setGamma(gamma);
		fgCtrls->setShutter(shutter);
		fgCtrls->setGain(gain);

		// open the RobotFilter
		if (true == rf.open(ri)) {
			long imgNbr = 0;
			char * fileName = new char[15];
			fileName[14] = 0;
			while (true) {
				// whenever an image arrives announce it to the console and save it to a file
				yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
				fgImage->getImage(img);

				// set up the image file name
				sprintf(fileName, "image%05ld.pbm", imgNbr);

				// announce to the console
//				printf("image nr %ld received\n", imgNbr);

				// detect gripper and target blobs
				detectBlobs(img);

				unsigned char* raw = new unsigned char[img.getRawImageSize()];
				raw = img.getRawImage();
				int width = img.width();
				int height = img.height();
				int bytes = img.getRawImageSize();

				// save the file
				FILE* imagefile = fopen(fileName, "wb");
				if (imagefile == NULL) {
					perror("Can't create file");
				} else {
					fprintf(imagefile, "P6\n%u %u\n255\n", width, height);
					fwrite(raw, 1, bytes, imagefile);
					fclose(imagefile);
					printf("wrote: %s \n", fileName);
					++imgNbr;
				}
			}
			delete[] fileName;
			rf.close();
		} else {
			cout << "##  COULD NOT OPEN THE FILTER!  Program is stopping."
					<< endl;
			cout.flush();
		}

		// close the remote framegrabber device
		remoteFg.close();
	} else {
		cout
				<< "##  COULD NOT OPEN THE REMOTE FRAMEGRABBER!  Program is stopping."
				<< endl;
		cout.flush();
	}

	return 0;
}

void initDetectionParams() {
	// params for gripper (found by manual experimentation)
	gripperParams.setHSVBoundaries(90,115,80,180,20,255);
	gripperParams.distMax=10;
	gripperParams.pixelsMin=50;
	gripperParams.pixelsMax=6000;
	gripperParams.sizeMax=60;
	gripperParams.sizeMin=5;

	// params for targets (found by manual experimentation)
	targetParams.setHSVBoundaries(0,10,100,240,40,255);
	targetParams.distMax=5;
	targetParams.pixelsMin=50;
	targetParams.pixelsMax=8000;
	targetParams.sizeMax=50;
	targetParams.sizeMin=15;
}

bool testPixelHSV(int x, int y, int hue, int saturation, int value,
		const std::vector<Blob> &blobs, const DetectionParams& param) {
	if (param.hueMin < param.hueMax) {
		return ((hue >= param.hueMin) && (hue <= param.hueMax) && (saturation
				>= param.saturationMin) && (saturation <= param.saturationMax)
				&& (value >= param.valueMin) && (value <= param.valueMax));
	} else // param.hueMin>=param.hueMax around
	{
		return (((hue >= param.hueMin && hue <= 255) || (hue <= param.hueMax
				&& hue >= 0)) && (saturation >= param.saturationMin)
				&& (saturation <= param.saturationMax) && (value
				>= param.valueMin) && (value <= param.valueMax));
	}
}

void usePixel(int x, int y, std::vector<Blob> &blobs,
		const DetectionParams& param) {
	if (blobs.size() == 0) {
		Blob blob(x, y);
		blobs.push_back(blob);
	}
	int nearestBlob = -1;
	for (unsigned int i = 0; i < blobs.size(); i++) {
		if (y == blobs[i].last_y) {
			if ((x >= blobs[i].last_row_start_col - param.distMax && x
					<= blobs[i].last_row_end_col + param.distMax) || (x
					>= blobs[i].first_x - param.distMax && x <= blobs[i].last_x
					+ param.distMax)) {
				if (nearestBlob == -1) {
					nearestBlob = i;
				} else {
					blobs[nearestBlob].joinWithBlob(blobs[i]);
					blobs.erase(blobs.begin() + i); // TODO: not effective for vectors!!!
					i--;
				}
			}
		} else if (y - blobs[i].last_y < param.distMax) {
			if (x >= blobs[i].first_x - param.distMax && x <= blobs[i].last_x
					+ param.distMax) {
				if (nearestBlob == -1) {
					nearestBlob = i;
				} else {
					blobs[nearestBlob].joinWithBlob(blobs[i]);
					blobs.erase(blobs.begin() + i); // TODO: not effective for vectors!!!
					i--;
				}
			}

		}
	}
	if (nearestBlob != -1) {
		blobs[nearestBlob].addPixel(x, y);
	} else {
		Blob blob(x, y);
		blobs.push_back(blob);
	}
}

void detectBlobs(yarp::sig::ImageOf<yarp::sig::PixelRgb> & img) {
	int height = img.height();
	int width = img.width();

	std::vector<Blob> gripperBlobs;
	std::vector<Blob> targetBlobs;

	// detect gripper and target(s) blobs
	yarp::sig::PixelRgb pixel;
	int hue, saturation, value;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			pixel = img.pixel(x, y);
			rgb2hsv(pixel.r, pixel.g, pixel.b, hue, saturation, value);

			// check for gripper - beware: multiple gripper locations may be detected
			if (testPixelHSV(x, y, hue, saturation, value, gripperBlobs,
					gripperParams)) {
				usePixel(x, y, gripperBlobs, gripperParams);
			}

			// check for target(s)
			if (testPixelHSV(x, y, hue, saturation, value, targetBlobs,
					targetParams)) {
				usePixel(x, y, targetBlobs, targetParams);
			}
		}
	}

	// log detected blobs
	int x, y;
	const yarp::sig::PixelRgb yellow(255, 255, 0);

	// gripper
	if (gripperBlobs.size() > 0) {
		printf("gripper detected: ");
		for (unsigned int k = 0; k < gripperBlobs.size(); k++) {
			if (gripperBlobs[k].isValid(gripperParams)) {
				x = (int) gripperBlobs[k].getX();
				y = (int) gripperBlobs[k].getY();
				printf(" (%d, %d)", x, y);
				yarp::sig::draw::addCrossHair(img, yellow, x, y, 10);
			}
		}
		printf("\n");
		fflush(stdout);
	}

	// target(s)
	if (targetBlobs.size() > 0) {
		printf("target(s) detected: ");
		for (unsigned int k = 0; k < targetBlobs.size(); k++) {
			if (targetBlobs[k].isValid(targetParams)) {
				x = (int) targetBlobs[k].getX();
				y = (int) targetBlobs[k].getY();
				printf(" (%d, %d)", x, y);
				yarp::sig::draw::addCircleOutline(img, yellow, x, y, 5);
			}
		}
		printf("\n");
		fflush(stdout);
	}
}
