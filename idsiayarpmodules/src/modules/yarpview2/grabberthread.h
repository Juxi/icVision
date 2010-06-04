#ifndef GRABBERTHREAD_ISDEF
#define GRABBERTHREAD_ISDEF

#include <QThread>
#include <QImage>
#include <QSemaphore>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/sig/all.h>

// #include "camera.h"

class GrabberThread: public QThread {
Q_OBJECT

public:
	GrabberThread(yarp::dev::PolyDriver& driver, QObject *parent = 0);
	~GrabberThread();
	enum BayerFilterType {
		GRBG, RGGB, BGGR, GBRG
	};
	enum ImageType {
		MONO, STEREO
	};
	enum ImageConverter {
		YUV422, YUV411, GRAY, MASK, SIMPLE, SIMPLE2, LINEAR
	};

	ImageConverter getImageConverter() {
		return imageConverter;
	}

	BayerFilterType getBayerFilterType() {
		return bayerFilterType;
	}

	ImageType getImageType() {
		return imageType;
	}

	void setImageConverter(ImageConverter imageConverter) {
		this->imageConverter = imageConverter;
	}

	void setBayerFilterType(BayerFilterType bayerFilterType) {
		this->bayerFilterType = bayerFilterType;
	}

	void setImageType(ImageType imageType);

	float framerate;

protected:
	void run();

private:
	yarp::dev::PolyDriver& driver;
	yarp::dev::IFrameGrabberControls *fgCtrls;
	yarp::dev::IFrameGrabberImage *fgImage;

	bool abort;
	QImage image;
//	void convertYUV422ToRGB(unsigned char* frameimage, unsigned int offset = 0);
//	void convertYUV411ToRGB(unsigned char* frameimage, unsigned int offset = 0);
//	void
//	convertYUV422ToGray(unsigned char* frameimage, unsigned int offset = 0);
//	void
//	convertYUV411ToGray(unsigned char* frameimage, unsigned int offset = 0);
//	void convertGray(unsigned char* frameimage, unsigned int offset = 0);
//	void convertMask(unsigned char* frameimage, unsigned int offset = 0);
//	void convertSimple(unsigned char* frameimage, unsigned int offset = 0);
//	void convertSimple2(unsigned char* frameimage, unsigned int offset = 0);
//	void convertLinear(unsigned char* frameimage, unsigned int offset = 0);
//	// void convert3D(dc1394video_frame_t* frame);
	BayerFilterType bayerFilterType;
	ImageConverter imageConverter;
	ImageType imageType;
	QSemaphore modifyImage;

signals:
void newFrame(const QImage& image);
};

#endif
