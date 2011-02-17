// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <FirewireCamera2.h>

#include <yarp/os/Semaphore.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <ace/Log_Msg.h>
#include <ace/OS.h>

#include "FirewireCamera2Set.h"

using namespace yarp::os;
using namespace yarp::dev;

// Most of this is not used (here because the driver was ported).
class FirewireCameraResources {
public:
	FirewireCameraResources(void) {
	}

	~FirewireCameraResources() {
		_uninitialize();
	}

private:
	static CFWCamera2Set* m_pCameraSet;
	static Semaphore m_InitCloseMutex;

	unsigned int unit_number;

public:
	int sizeX;
	int sizeY;
	int buffLength;

	inline bool _initialize(const FirewireCameraOpenParameters & params);
	inline bool _uninitialize();

	inline bool _hasFeatureDC1394(int feature);
	inline bool _setFeatureDC1394(int feature, double value);
	inline double _getFeatureDC1394(int feature);

	inline bool _setBrightness(double value);
	inline bool _setExposure(double value);
	inline bool _setSharpness(double value);
	inline bool _setWhiteBalance(double blueValue, double redValue);
	inline bool _setHue(double value);
	inline bool _setSaturation(double value);
	inline bool _setGamma(double value);
	inline bool _setShutter(double value);
	inline bool _setGain(double value);
	inline bool _setIris(double value);

	inline double _getBrightness();
	inline double _getExposure();
	inline double _getSharpness();
	inline bool _getWhiteBalance(double& blue, double& red);
	inline double _getHue();
	inline double _getSaturation();
	inline double _getGamma();
	inline double _getShutter();
	inline double _getGain();
	inline double _getIris();

	inline bool _setAuto(bool bAuto);

	inline bool _setAutoBrightness(bool bAuto = true);
	inline bool _setAutoWhiteBalance(bool bAuto = true);
	inline bool _setAutoShutter(bool bAuto = true);
	inline bool _setAutoGain(bool bAuto = true);

	inline void _printSettings() {
		m_pCameraSet->PrintSettings(unit_number);
	}

	inline bool _capture(unsigned char *buff) {
		return m_pCameraSet->Capture(unit_number, buff);
	}

	inline bool _capture_raw(unsigned char *buff) {
		return m_pCameraSet->CaptureRaw(unit_number, buff);
	}
};

CFWCamera2Set* FirewireCameraResources::m_pCameraSet = NULL;
Semaphore FirewireCameraResources::m_InitCloseMutex;

/// full initialize and startup of the grabber.
inline bool FirewireCameraResources::_initialize(
		const FirewireCameraOpenParameters& params) {
	m_InitCloseMutex.wait();

	if (!m_pCameraSet) {
		m_pCameraSet = new CFWCamera2Set();
		if (!m_pCameraSet->Init()) // default port=0
		{
			delete m_pCameraSet;
			m_pCameraSet = NULL;
			m_InitCloseMutex.post();
			return false;
		}
	}

	unit_number = params._unit_number;
	sizeX = params._size_x;
	sizeY = params._size_y;
	buffLength = sizeX * sizeY * 3;

	if (!m_pCameraSet->StartCamera(unit_number, sizeX, sizeY)) // default dma=true
	{
		ACE_OS::fprintf(stderr,
				"FirewirecameraResources: can't open camera %d", unit_number);
		return false;
	}

	m_InitCloseMutex.post();

	// Setup Camera Parameters, Magic Numbers :-)
	m_pCameraSet->SetBrightness(unit_number, params._brightness);
	m_pCameraSet->SetShutter(unit_number, params._shutter);
	m_pCameraSet->SetGain(unit_number, params._gain);

	if (params._whiteR > 0.0) {
		m_pCameraSet->SetWhiteBalance(unit_number, params._whiteB,
				params._whiteR);
	} else {
		m_pCameraSet->SetWhiteBalance(unit_number, 0.5, 0.5);
	}

	return true;
}

inline bool FirewireCameraResources::_uninitialize() {
	m_InitCloseMutex.wait();

	if (m_pCameraSet) {
		m_pCameraSet->ShutdownCamera(unit_number);
		if (m_pCameraSet->GetCameraNum() <= 0) {
			m_pCameraSet->Shutdown();
			// TODO: It should be deleted but I can't - WHY????
			//			delete m_pCameraSet;
			m_pCameraSet = NULL;
		}
	}

	m_InitCloseMutex.post();

	return true;
}

inline bool FirewireCameraResources::_hasFeatureDC1394(int feature) {
	return m_pCameraSet->hasFeatureDC1394(unit_number, feature);
}

inline bool FirewireCameraResources::_setFeatureDC1394(int feature, double value) {
	return m_pCameraSet->setFeatureDC1394(unit_number, feature, value);
}

inline double FirewireCameraResources::_getFeatureDC1394(int feature) {
	return m_pCameraSet->getFeatureDC1394(unit_number, feature);
}

inline bool FirewireCameraResources::_setBrightness(double value) {
	return m_pCameraSet->SetBrightness(unit_number, value);
}

inline bool FirewireCameraResources::_setExposure(double value) {
	return m_pCameraSet->SetExposure(unit_number, value);
}

inline bool FirewireCameraResources::_setSharpness(double value) {
	return m_pCameraSet->SetSharpness(unit_number, value);
}

inline bool FirewireCameraResources::_setWhiteBalance(double blue, double red) {
	return m_pCameraSet->SetWhiteBalance(unit_number, blue, red);
}

inline bool FirewireCameraResources::_setHue(double value) {
	return m_pCameraSet->SetHue(unit_number, value);
}

inline bool FirewireCameraResources::_setSaturation(double value) {
	return m_pCameraSet->SetSaturation(unit_number, value);
}

inline bool FirewireCameraResources::_setGamma(double value) {
	return m_pCameraSet->SetGamma(unit_number, value);
}

inline bool FirewireCameraResources::_setShutter(double value) {
	return m_pCameraSet->SetShutter(unit_number, value);
}

inline bool FirewireCameraResources::_setGain(double value) {
	return m_pCameraSet->SetGain(unit_number, value);
}

inline bool FirewireCameraResources::_setIris(double value) {
	return m_pCameraSet->SetIris(unit_number, value);
}

inline double FirewireCameraResources::_getBrightness() {
	return m_pCameraSet->GetBrightness(unit_number);
}

inline double FirewireCameraResources::_getExposure() {
	return m_pCameraSet->GetExposure(unit_number);
}

inline double FirewireCameraResources::_getSharpness() {
	return m_pCameraSet->GetSharpness(unit_number);
}

inline bool FirewireCameraResources::_getWhiteBalance(double& blue, double& red) {
	return m_pCameraSet->GetWhiteBalance(unit_number, blue, red);
}

inline double FirewireCameraResources::_getHue() {
	return m_pCameraSet->GetHue(unit_number);
}

inline double FirewireCameraResources::_getSaturation() {
	return m_pCameraSet->GetSaturation(unit_number);
}

inline double FirewireCameraResources::_getGamma() {
	return m_pCameraSet->GetGamma(unit_number);
}

inline double FirewireCameraResources::_getShutter() {
	return m_pCameraSet->GetShutter(unit_number);
}

inline double FirewireCameraResources::_getGain() {
	return m_pCameraSet->GetGain(unit_number);
}

inline double FirewireCameraResources::_getIris() {
	return m_pCameraSet->GetIris(unit_number);
}

inline bool FirewireCameraResources::_setAuto(bool bAuto) {
	bool bOk = true;

	bOk = bOk && _setAutoBrightness(bAuto);
	bOk = bOk && _setAutoWhiteBalance(bAuto);
	bOk = bOk && _setAutoShutter(bAuto);
	bOk = bOk && _setAutoGain(bAuto);

	return bOk;
}

inline bool FirewireCameraResources::_setAutoBrightness(bool bAuto) {
	return m_pCameraSet->SetAutoBrightness(unit_number, bAuto);
}

inline bool FirewireCameraResources::_setAutoWhiteBalance(bool bAuto) {
	return m_pCameraSet->SetAutoWhiteBalance(unit_number, bAuto);
}

inline bool FirewireCameraResources::_setAutoShutter(bool bAuto) {
	return m_pCameraSet->SetAutoShutter(unit_number, bAuto);
}

inline bool FirewireCameraResources::_setAutoGain(bool bAuto) {
	return m_pCameraSet->SetAutoGain(unit_number, bAuto);
}

/*
 inline void DragonflyResources::_subSampling(void)
 {
 }
 */

inline FirewireCameraResources& RES(void *res) {
	return *(FirewireCameraResources *) res;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


FirewireCamera2::FirewireCamera2(void) {
	printf("FirewireCamera2::FirewireCamera2(void)\n"); fflush(stdout);
	system_resources = (void *) new FirewireCameraResources;
	ACE_ASSERT(system_resources!=NULL);
}

FirewireCamera2::~FirewireCamera2() {
	printf("FirewireCamera2::~FirewireCamera2()\n"); fflush(stdout);
	if (system_resources != NULL)
		delete (FirewireCameraResources *) system_resources;

	system_resources = NULL;
}

///
bool FirewireCamera2::open(const FirewireCameraOpenParameters &par) {
	printf("FirewireCamera2::open(const FirewireCameraOpenParameters &par)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._initialize(par);
}

bool FirewireCamera2::close(void) {
	printf("FirewireCamera2::close(void)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._uninitialize();
}

bool FirewireCamera2::getRawBuffer(unsigned char *buff) {
	printf("FirewireCamera2::getRawBuffer(unsigned char *buff)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._capture_raw(buff);
}

bool FirewireCamera2::getRgbBuffer(unsigned char *buff) {
	printf("FirewireCamera2::getRgbBuffer(unsigned char *buff)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._capture(buff);
}

bool FirewireCamera2::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
//	printf("FirewireCamera2::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	image.resize(d.sizeX, d.sizeY);

	if (image.getRawImageSize() == d.buffLength) {
		d._capture(image.getRawImage());
		return true;
	}

	return false;
}

int FirewireCamera2::getRawBufferSize() {
	printf("FirewireCamera2::getRawBufferSize()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);
	return d.buffLength;
}

int FirewireCamera2::width() const {
	printf("FirewireCamera2::width()\n"); fflush(stdout);
	return RES(system_resources).sizeX;
}

int FirewireCamera2::height() const {
	printf("FirewireCamera2::height()\n"); fflush(stdout);
	return RES(system_resources).sizeY;
}

bool FirewireCamera2::hasFeatureDC1394(int feature) {
	printf("FirewireCamera2::hasFeatureDC1394(int feature)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._hasFeatureDC1394(feature);
}

bool FirewireCamera2::setFeatureDC1394(int feature, double value) {
	printf("FirewireCamera2::setFeatureDC1394(int feature, double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setFeatureDC1394(feature, value);
}

double FirewireCamera2::getFeatureDC1394(int feature) {
	printf("FirewireCamera2::getFeatureDC1394(int feature)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getFeatureDC1394(feature);
}


bool FirewireCamera2::setBrightness(double value) {
	printf("FirewireCamera2::setBrightness(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setBrightness(value);
}

bool FirewireCamera2::setExposure(double value) {
	printf("FirewireCamera2::setExposure(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setExposure(value);
}

bool FirewireCamera2::setSharpness(double value) {
	printf("FirewireCamera2::setSharpness(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setSharpness(value);
}

bool FirewireCamera2::setWhiteBalance(double red, double blue) {
	printf("FirewireCamera2::setWhiteBalance(double red, double blue)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setWhiteBalance(blue, red);
}

bool FirewireCamera2::setHue(double value) {
	printf("FirewireCamera2::setHue(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setHue(value);
}

bool FirewireCamera2::setSaturation(double value) {
	printf("FirewireCamera2::setSaturation(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setSaturation(value);
}

bool FirewireCamera2::setGamma(double value) {
	printf("FirewireCamera2::setGamma(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setGamma(value);
}

bool FirewireCamera2::setShutter(double value) {
	printf("FirewireCamera2::setShutter(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setShutter(value);
}

bool FirewireCamera2::setGain(double value) {
	printf("FirewireCamera2::setGain(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setGain(value);
}

bool FirewireCamera2::setIris(double value) {
	printf("FirewireCamera2::setIris(double value)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setIris(value);
}

double FirewireCamera2::getBrightness() {
	printf("FirewireCamera2::getBrightness()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getBrightness();
}

double FirewireCamera2::getExposure() {
	printf("FirewireCamera2::getExposure()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getExposure();
}

double FirewireCamera2::getSharpness() {
	printf("FirewireCamera2::getSharpness()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getSharpness();
}

bool FirewireCamera2::getWhiteBalance(double &red, double &blue) {
	printf("FirewireCamera2::getWhiteBalance(double &red, double &blue)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getWhiteBalance(blue, red);
}

double FirewireCamera2::getHue() {
	printf("FirewireCamera2::getHue()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getHue();
}

double FirewireCamera2::getSaturation() {
	printf("FirewireCamera2::getSaturation()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getSaturation();
}

double FirewireCamera2::getGamma() {
	printf("FirewireCamera2::getGamma()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getGamma();
}

double FirewireCamera2::getShutter() {
	printf("FirewireCamera2::getShutter()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getShutter();
}

double FirewireCamera2::getGain() {
	printf("FirewireCamera2::getGain()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getGain();
}

double FirewireCamera2::getIris() {
	printf("FirewireCamera2::getIris()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._getIris();
}

bool FirewireCamera2::setAuto(bool bAuto) {
	printf("FirewireCamera2::setAuto(bool bAuto)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setAuto(bAuto);
}

bool FirewireCamera2::setAutoBrightness(bool bAuto) {
	printf("FirewireCamera2::setAutoBrightness(bool bAuto)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setAutoBrightness(bAuto);
}

bool FirewireCamera2::setAutoShutter(bool bAuto) {
	printf("FirewireCamera2::setAutoShutter(bool bAuto)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setAutoShutter(bAuto);
}

bool FirewireCamera2::setAutoGain(bool bAuto) {
	printf("FirewireCamera2::setAutoGain(bool bAuto)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setAutoGain(bAuto);
}

bool FirewireCamera2::setAutoWhiteBalance(bool bAuto) {
	printf("FirewireCamera2::setAutoWhiteBalance(bool bAuto)\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	return d._setAutoWhiteBalance(bAuto);
}

void FirewireCamera2::PrintSettings() {
	printf("FirewireCamera2::PrintSettings()\n"); fflush(stdout);
	FirewireCameraResources& d = RES(system_resources);

	d._printSettings();
}
